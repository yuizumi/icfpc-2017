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
        return {{"punterTrees", json_punterTrees}, {"rivers", json_rivers}};
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        for (int i = 0; i < num_punters_; i++) {
            UnionFind uf_trees(map_->sites().size());
            punterTrees_.push_back(uf_trees);
        }
    }

    // greedy
    Move Gameplay(const std::vector<Move>& moves) override {
        // timeoutなどで嘘になるけどpunterIdを知る方法がこれ以外にないので暫定対応
        for(int i = 0; i < moves.size(); i++) {
        // for (const Move& m : moves) {
            auto m = moves[i];
            if (m.action == kClaim) {
                rivers_.erase(m.river);
                punterTrees_[i].unionSet(m.river.source, m.river.target);
            }
        }
        for (const River& river : rivers_) {
            for (const SiteId& mine : map_->mines()) {
                if(river.source == mine || river.target == mine) {
                    return {kClaim, river};
                }
            }
        }
        if (rivers_.empty()) {
            return {kPass, {}};
        } else {
            return {kClaim, *rivers_.begin()};
        }
    }

    int id_;
    int num_punters_;
    const Map* map_;
    vector<UnionFind> punterTrees_;
    set<River, CompareRiver> rivers_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
