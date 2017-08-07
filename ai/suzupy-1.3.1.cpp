#include <iostream>
#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"
#include "distance_map.h"
#include "punter_info.h"
#include "river_manager.h"
#include "union_find.h"

using namespace std;

namespace {

class Suzupy : public AI {
    void Init(int id, int num_punters, const Map* map,
              const Json& settings) override {
        id_ = id;
        map_ = map;
        option_ = HasSetting(settings, "options");
        punters_.resize(num_punters);
    }

    void LoadState(Json&& json) override {
        rivers_ = json["rivers"].get<decltype(rivers_)>();
        punters_ = json["punters"].get<decltype(punters_)>();
        distance_ = json["distance"].get<decltype(distance_)>();
    }

    Json SaveState() override {
        return {{"rivers", rivers_}, {"punters", punters_}, {"distance", distance_}};
    }

    void Setup() override {
        rivers_ = RiverManager(*map_, option_);
        for (size_t i = 0; i < punters_.size(); i++)
            punters_[i] = PunterInfo(*map_, option_);
        distance_.Init(*map_);
    }

    void HandleClaim(int punter, const River& river) override {
        switch (rivers_.NextAction(punter, river)) {
            case kClaim:
                punters_[punter].HandleClaim (river); break;
            case kOption:
                punters_[punter].HandleOption(river); break;
            default:
                assert(false);
        }
        rivers_.HandleClaim(punter, river);
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        int max_score = INT32_MIN;
        River max_river(-1, -1);
        for (const auto& entry : rivers_.map()) {
            const River& river = entry.first;
            if (!CanClaim(id_, river))
                continue;
            const int score = Evaluate(river);
            if (score >= max_score) {  // ひねくれて後ろの辺を優先
                max_score = score;
                max_river = river;
            }
        }
        return {rivers_.NextAction(id_, max_river), max_river};
    }

    bool CanClaim(int punter_id, const River& river) const {
        switch (rivers_.NextAction(punter_id, river)) {
            case kClaim:
                return true;
            case kOption:
                return punters_[punter_id].num_options() > 0;
            default:
                return false;
        }
    }

    // UnionFind があるので微妙に const じゃない
    int Evaluate(const River& river) {
        static constexpr int kRule1Score = 300000000;
        static constexpr int kRule2Score = 200000000;
        static constexpr int kRule3Score = 100000000;

        const SiteId source = river.source;
        const SiteId target = river.target;

        // #1: 妨害工作
        for (int i = 0; i < punters_.size(); i++) {
            if (i == id_)
                continue;
            // optionをまだ使ってない人は防がない
            if (punters_[i].num_options() == map_->mines().size()
                && rivers_.NextAction(id_, river) == kOption)
                continue;
            if (punters_[i].IsConnectingRiver(river)) {
                return punters_[i].Size(source) + punters_[i].Size(target) +
                    kRule1Score;
            }
        }
        // #2: λ鉱脈同士を接続する
        if (!punters_[id_].FindSet(source, target) &&
            punters_[id_].IsReachable(source) &&
            punters_[id_].IsReachable(target)) {
            return punters_[id_].Size(source) + punters_[id_].Size(target) +
                kRule2Score;
        }
        // オプションを#3に使うのは勿体ない
        if (rivers_.NextAction(id_, river) == kOption) {
            return 0;
        }
        // #3: λ鉱脈への経路を伸ばす辺があればとる（獲得点数を考慮）
        if (punters_[id_].IsExpandingRiver(river)) {
            const SiteId older =
                punters_[id_].IsReachable(source) ? source : target;
            const SiteId newer =
                punters_[id_].IsReachable(source) ? target : source;
            int score = 0;
            for (const SiteId mine : map_->mines()) {
                if (punters_[id_].FindSet(older, mine)) {
                    const int dist = distance_.Get(newer, mine);
                    score += dist * dist;
                } else {
                    // 他のλ鉱脈に近い方が良い
                    // TODO: 本当はλを含む木への距離
                    score -= distance_.Get(newer, mine);
                }
            }
            return score + kRule3Score;
        }

        return 0;  // もうなんでもいいです
    }

    int id_;
    const Map* map_;
    bool option_;
    RiverManager rivers_;
    vector<PunterInfo> punters_;
    DistanceMap distance_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
