#include <vector>

#include "../core/ai.h"
#include "../core/driver.h"

using namespace std;

namespace {

constexpr auto kClaim = Move::Action::kClaim;
constexpr auto kPass = Move::Action::kPass;

class UnionFind {
  public:
    vector<int> data;
    UnionFind(int size): data(size - 1, 1e9) { }
    UnionFind(vector<int> v) { data = v; }
    bool UnionSet(int x, int y){
      x = root(x); y = root(y);
      if(x != y) {
        if(data[y] < data[x]) swap(x, y);
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
        map_ = map;
    }

    void LoadState(Json&& json) override {
      for (const Json& trees: json) {
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
      return json_punterTrees;
    }

    void Setup() override {
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        return {kPass, {}}; // nullai相当のダミー
    }

    const Map* map_;
    vector<UnionFind> punterTrees_;
};

}  // namespace

int main() {
    Suzupy ai;
    Run(&ai);
    return 0;
}
