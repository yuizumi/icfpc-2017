#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"
#include "punter_info.h"
#include "river_manager.h"

using namespace std;

namespace {

class Suzupy : public AI {
    void Init(int id, int num_punters, const Map* map, const Json& settings) override {
        id_ = id;
        map_ = map;
        option_ = settings.count("option") && settings["option"];
        punters_.resize(num_punters);
    }

    void LoadState(Json&& json) override {
        rivers_ = json["rivers"].get<decltype(rivers_)>();
        punters_ = json["punters"].get<decltype(punters_)>();
    }

    Json SaveState() override {
        return {{"rivers", rivers_}, {"punters", punters_}};
    }

    void Setup() override {
        rivers_ = RiverManager(*map_, option_);
        for (size_t i = 0; i < punters_.size(); i++)
            punters_[i] = PunterInfo(*map_, option_);
    }

    void HandleClaim(int punter, const River& river) override {
        switch (rivers_.Get(river)) {
            case kClaim:
                punters_[punter].HandleClaim (river); break;
            case kOption:
                punters_[punter].HandleOption(river); break;
            default:
                assert(false);
        }
        rivers_.HandleClaim(river);
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        // 他のmineと接続しそうな人 or mineとつながっていない連結成分をつなごうとしている怪しい人がいたら妨害する
        for (const auto& entry : rivers_.map()) {
            if (entry.second != kClaim) {
                continue;
            }
            const River& river = entry.first;
            for (int i = 0; i < punters_.size(); i++)
                if (i != id_ && punters_[i].IsConnectingRiver(river))
                    return {kClaim, river};
        }
        // cycleを避けつつ他のmineと接続できるなら先につなぐ
        for (const auto& entry : rivers_.map()) {
            if (entry.second != kClaim) {
                continue;
            }
            const River& river = entry.first;
            // 1.1 と微妙に違うけど実質的には同じはず
            if (punters_[id_].IsConnectingRiver(river))
                return {kClaim, river};
        }
        // 既にmineにつながっているところに隣接したriverでまだmineにつながっていないsiteを優先して取る
        for (const auto& entry : rivers_.map()) {
            if (entry.second != kClaim) {
                continue;
            }
            const River& river = entry.first;
            if (punters_[id_].IsExpandingRiver(river))
                return {kClaim, river};
        }
        for (const auto& entry : rivers_.map()) {
            if (entry.second != kClaim) {
                continue;
            }
            return {kClaim, entry.first};
        }
        return {kPass, {}};
    }

    int id_;
    const Map* map_;
    bool option_;
    vector<PunterInfo> punters_;
    RiverManager rivers_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
