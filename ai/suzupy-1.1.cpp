#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"
#include "river_set.h"
#include "union_find.h"

using namespace std;

namespace {

class Suzupy : public AI {
    void Init(int id, int num_punters, const Map* map, const Json& settings) override {
        id_ = id;
        num_punters_ = num_punters;
        map_ = map;
    }

    void LoadState(Json&& json) override {
        rivers_ = json["rivers"].get<RiverSet>();
        punterTrees_ = json["punterTrees"].get<decltype(punterTrees_)>();
        reachables_ = json["reachables"].get<decltype(reachables_)>();
    }

    Json SaveState() override {
        return {
            {"rivers", rivers_},
            {"punterTrees", punterTrees_},
            {"reachables", reachables_},
        };
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        for (int i = 0; i < num_punters_; i++) {
            punterTrees_.emplace_back(map_->num_sites());
        }
        for (int i = 0; i < num_punters_; i++) {
            reachables_.push_back(
                set<int>(map_->mines().begin(), map_->mines().end()));
        }
    }

    void HandleClaim(int punter, const River& river) override {
        rivers_.erase(river);
        punterTrees_[punter].UnionSet(river.source, river.target);
        reachables_[punter].insert(river.source);
        reachables_[punter].insert(river.target);
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        // 他のmineと接続しそうな人 or mineとつながっていない連結成分をつなごうとしている怪しい人がいたら妨害する
        for (const River& river : rivers_) {
            for (int i = 0; i < num_punters_; i++)
                if (i != id_) {
                    SiteId old_, new_;
                    if (reachables_[i].count(river.source)) {
                        old_ = river.source;
                        new_ = river.target;
                    } else if (reachables_[i].count(river.target)) {
                        old_ = river.target;
                        new_ = river.source;
                    } else {
                        continue;
                    }
                    if ((reachables_[i].count(new_) || punterTrees_[i].size(new_) >= 2)
                        && !punterTrees_[i].FindSet(old_, new_))
                        return {kClaim, river};
                }
        }
        // cycleを避けつつ他のmineと接続できるなら先につなぐ
        for (const River& river : rivers_) {
            SiteId old_, new_;
            if (reachables_[id_].count(river.source)) {
                old_ = river.source;
                new_ = river.target;
            } else if (reachables_[id_].count(river.target)) {
                old_ = river.target;
                new_ = river.source;
            } else {
                continue;
            }
            if (reachables_[id_].count(new_) && !punterTrees_[id_].FindSet(old_, new_))
                return {kClaim, river};
        }
        // 既にmineにつながっているところに隣接したriverでまだmineにつながっていないsiteを優先して取る
        for (const River& river : rivers_) {
            SiteId old_, new_;
            if (reachables_[id_].count(river.source)) {
                old_ = river.source;
                new_ = river.target;
            } else if (reachables_[id_].count(river.target)) {
                old_ = river.target;
                new_ = river.source;
            } else {
                continue;
            }
            if (!reachables_[id_].count(new_))
                return {kClaim, river};
        }
        if (rivers_.empty()) {
            return {kPass, {}};
        } else {
            const River& river = *rivers_.begin();
            return {kClaim, river};
        }
    }

    int id_;
    int num_punters_;
    const Map* map_;
    vector<UnionFind> punterTrees_;
    set<River, CompareRiver> rivers_;
    vector<set<SiteId>> reachables_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
