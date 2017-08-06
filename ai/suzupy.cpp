#include <vector>
#include <set>
#include <map>

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
    map<int, int> data;
    UnionFind(): data() { }
    UnionFind(map<int, int> v) { data = v; }
    bool UnionSet(int x, int y) {
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
            UnionFind uf_trees(trees.get<map<int, int>>());
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
            UnionFind uf_trees;
            // for (const SiteId mine : map_.mines())
            //     uf_trees;
            punterTrees_.push_back(uf_trees);
        }
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        for (const Move& m : moves) {
            if (m.action == kClaim) rivers_.erase(m.river);
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
