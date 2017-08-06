#include <iostream>
#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"
#include "distance_map.h"
#include "reachable_set.h"
#include "river_set.h"
#include "union_find.h"

using namespace std;

namespace {

class Suzupy : public AI {
    void Init(int id, int num_punters, const Map* map,
              const Json& settings) override {
        id_ = id;
        num_punters_ = num_punters;
        map_ = map;
    }

    void LoadState(Json&& json) override {
        rivers_ = json["rivers"].get<decltype(rivers_)>();
        forests_ = json["forests"].get<decltype(forests_)>();
        reachables_ = json["reachables"].get<decltype(reachables_)>();
        distance_ = json["distance"].get<decltype(distance_)>();
    }

    Json SaveState() override {
        return {{"rivers", rivers_},
                {"forests", forests_},
                {"reachables", reachables_},
                {"distance", distance_}};
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        for (int i = 0; i < num_punters_; i++) {
            forests_.emplace_back(map_->num_sites());
            reachables_.emplace_back(map_->mines());
        }
        distance_.Init(*map_);
    }

    void HandleClaim(int punter, const River& river) override {
        rivers_.erase(river);
        forests_[punter].UnionSet(river.source, river.target);
        reachables_[punter].Claim(river);
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        int max_score = -1;
        River selected;

        // #1: 妨害工作
        for (const River& river : rivers_) {
            for (int i = 0; i < num_punters_; i++) {
                if (i == id_) continue;
                SiteId older, newer;
                // すでに同じ連結成分に含まれる場合は対象外
                if (forests_[i].FindSet(river.source, river.target)) continue;
                // λ鉱脈に接続されない辺は妨害の対象外
                if (!reachables_[i].Test(river, &older, &newer)) continue;
                // 異なるλ鉱脈同士 or 連結成分とλ鉱脈を接続しようとしている
                if (reachables_[i].Has(newer) || forests_[i].size(newer) >= 2) {
                    int score = forests_[i].size(older) + forests_[i].size(newer);
                    if (score >= max_score) {  // ひねくれて後ろの辺を優先
                        max_score = score;
                        selected = river;
                    }
                }
            }
        }
        if (max_score > 0) {
            cerr << "Rule #1: " << max_score << endl;
            return {kClaim, selected};
        }

        // #2: λ鉱脈同士を接続する
        for (const River& river : rivers_) {
            const SiteId source = river.source;
            const SiteId target = river.target;
            if (!forests_[id_].FindSet(source, target) &&
                reachables_[id_].Has(source) &&
                reachables_[id_].Has(target)) {
                int score = forests_[id_].size(source) + forests_[id_].size(target);
                if (score >= max_score) {  // ひねくれて後ろの辺を優先
                    max_score = score;
                    selected = river;
                }
            }
        }
        if (max_score > 0) {
            cerr << "Rule #2: " << max_score << endl;
            return {kClaim, selected};
        }

        // #3: λ鉱脈への経路を伸ばす辺があればとる（獲得点数を考慮）
        for (const River& river : rivers_) {
            SiteId older, newer;
            if (reachables_[id_].Test(river, &older, &newer) &&
                !reachables_[id_].Has(newer)) {
                int score = 0;
                for (SiteId mine : map_->mines()) {
                    if (forests_[id_].FindSet(older, mine)) {
                        const int dist = distance_.Get(newer, mine);
                        score += dist * dist;
                    }
                }
                if (score >= max_score) {  // ひねくれて後ろの辺を優先
                    max_score = score;
                    selected = river;
                }
            }
        }
        if (max_score > 0) {
            cerr << "Rule #3: " << max_score << endl;
            return {kClaim, selected};
        }

        // もうなんでもいいです
        if (rivers_.empty()) {
            return {kPass, {}};
        } else {
            const River& river = *rivers_.begin();
            return {kClaim, river};
        }
    }

    int id_, num_punters_;
    const Map* map_;
    RiverSet rivers_;
    vector<UnionFind> forests_;
    vector<ReachableSet> reachables_;
    DistanceMap distance_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
