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

class Forest : public AI {
    void Init(int id, int num_punters, const Map* map) override {
        map_ = map;
    }

    void LoadState(Json&& json) override {
        mines_.insert(json["mines"].begin(), json["mines"].end());
        for (const Json& river_json : json["rivers"]) {
            const SiteId source = river_json["source"];
            const SiteId target = river_json["target"];
            rivers_.insert({source, target});
        }
    }

    Json SaveState() override {
        auto mines = Json::array();
        for (const SiteId mine : mines_) mines.push_back(mine);
        Json rivers;
        for (const River& river : rivers_) {
            rivers.push_back({{"source", river.source}, {"target", river.target}});
        }
        return {{"rivers", rivers}, {"mines", mines}};
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        mines_.insert(map_->mines().begin(), map_->mines().end());
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        for (const Move& m : moves) {
            if (m.action == kClaim) rivers_.erase(m.river);
        }
        for (const River& river : rivers_) {
            if (mines_.count(river.source) || mines_.count(river.target)) {
                mines_.insert(river.source);
                mines_.insert(river.target);
                return {kClaim, river};
            }
        }
        if (rivers_.empty()) {
            return {kPass, {}};
        } else {
            const River& river = *rivers_.begin();
            mines_.insert(river.source);
            mines_.insert(river.target);
            return {kClaim, river};
        }
    }

    const Map* map_;
    set<River, CompareRiver> rivers_;
    set<SiteId> mines_;
};

}  // namespace

int main() {
    Forest ai;
    Run(&ai);
    return 0;
}
