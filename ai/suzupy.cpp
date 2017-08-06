#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"

using namespace std;

namespace {

constexpr auto kClaim = Move::Action::kClaim;
constexpr auto kPass = Move::Action::kPass;

class CompareRiver {
public:
    bool operator()(const River& r1, const River& r2) const {
        if (r1.source != r2.source) return r1.source < r2.source;
        if (r1.target != r2.target) return r1.target < r2.target;
        return false;
    }
};

class UnionFind {
public:
    vector<int> data;
    UnionFind(int size): data(size, -1) { }
    UnionFind(vector<int> v) { data = v; }
    bool unionSet(int x, int y){
        x = root(x);
        y = root(y);
        if(x != y) {
            if(data[y] < data[x])
                swap(x, y);
            data[x] += data[y]; data[y] = x;
        }
        return x != y;
    }
    bool findSet(int x, int y) {
        return root(x) == root(y);
    }
    int root(int x) {
        return data[x] < 0 ? x : data[x] = root(data[x]);
    }
    int size(int x) {
        return -data[root(x)];
    }
};

class Suzupy : public AI {
    void Init(int id, int num_punters, const Map* map) override {
        id_ = id;
        num_punters_ = num_punters;
        map_ = map;
    }

    void LoadState(Json&& json) override {
        for (const Json& river_json : json["rivers"]) {
            const SiteId source = river_json["source"];
            const SiteId target = river_json["target"];
            rivers_.insert({source, target});
        }
        for (const Json& trees: json["punterTrees"]) {
            UnionFind uf_trees(trees.get<vector<int>>());
            punterTrees_.push_back(uf_trees);
        }
        for (const Json& punter_reachables_json : json["reachables"]) {
            set<SiteId> punter_reachables;
            punter_reachables.insert(punter_reachables_json.begin(), punter_reachables_json.end());
            reachables_.push_back(punter_reachables);
        }
    }

    Json SaveState() override {
        Json json_punterTrees = Json::array();
        for (const UnionFind trees : punterTrees_) {
            Json json_trees = trees.data;
            json_punterTrees.push_back(json_trees);
        }
        Json json_rivers = Json::array();
        for (const River& river : rivers_) {
            json_rivers.push_back({{"source", river.source}, {"target", river.target}});
        }
        auto json_reachables = Json::array();
        for (set<SiteId> punter_reachables : reachables_) {
            auto punter_reachables_json = Json::array();
            for (const SiteId siteId : punter_reachables)
                punter_reachables_json.push_back(siteId);
            json_reachables.push_back(punter_reachables_json);
        }
        return {{"punterTrees", json_punterTrees}, {"rivers", json_rivers}, {"reachables", json_reachables}};
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        for (int i = 0; i < num_punters_; i++) {
            UnionFind uf_trees(map_->sites().size());
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
                punterTrees_[i].unionSet(m.river.source, m.river.target);
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
            if (reachables_[id_].count(new_) && !punterTrees_[id_].findSet(old_, new_))
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
