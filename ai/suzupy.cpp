#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"
#include "river_set.h"
#include "union_find.h"

using namespace std;

namespace {

class Suzupy : public AI {
    void Init(int id, int num_punters, const Map* map) override {
        id_ = id;
        num_punters_ = num_punters;
        map_ = map;
    }

    void LoadState(Json&& json) override {
        rivers_ = json["rivers"].get<RiverSet>();
        punterTrees_ = json["punterTrees"].get<decltype(punterTrees_)>();
        for (const Json& punter_reachables_json : json["reachables"]) {
            set<SiteId> punter_reachables;
            punter_reachables.insert(punter_reachables_json.begin(), punter_reachables_json.end());
            reachables_.push_back(punter_reachables);
        }
    }

    Json SaveState() override {
        auto json_reachables = Json::array();
        for (set<SiteId> punter_reachables : reachables_) {
            auto punter_reachables_json = Json::array();
            for (const SiteId siteId : punter_reachables)
                punter_reachables_json.push_back(siteId);
            json_reachables.push_back(punter_reachables_json);
        }
        return {{"punterTrees", punterTrees_}, {"rivers", rivers_}, {"reachables", json_reachables}};
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        for (int i = 0; i < num_punters_; i++) {
            UnionFind uf_trees(map_->num_sites());
            punterTrees_.push_back(uf_trees);
        }
        for (int i = 0; i < num_punters_; i++) {
            set<SiteId> punter_reachables;
            for (const SiteId& mine : map_->mines()) {
                punter_reachables.insert(mine);
            }
            reachables_.push_back(punter_reachables);
        }
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        // timeoutなどで嘘になるけどpunterIdを知る方法がこれ以外にないので暫定対応
        for(int i = 0; i < moves.size(); i++) {
        // for (const Move& m : moves) {
            auto m = moves[i];
            if (m.action == kClaim) {
                rivers_.erase(m.river);
                punterTrees_[i].UnionSet(m.river.source, m.river.target);
                reachables_[i].insert(m.river.source);
                reachables_[i].insert(m.river.target);
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
    RiverSet rivers_;
    vector<set<SiteId>> reachables_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
