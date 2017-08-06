#include <set>

#include "../core/ai.h"
#include "../core/driver.h"

using namespace std;

namespace {

class CompareRiver {
public:
    bool operator()(const River& r1, const River& r2) const {
        if (r1.source != r2.source) return r1.source < r2.source;
        if (r1.target != r2.target) return r1.target < r2.target;
        return false;
    }
};

class Newbie : public AI {
    void Init(int id, int num_punters, const Map* map) override {
        map_ = map;
    }

    void LoadState(Json&& json) override {
        for (const Json& river_json : json) {
            const SiteId source = river_json["source"];
            const SiteId target = river_json["target"];
            rivers_.insert({source, target});
        }
    }

    Json SaveState() override {
        Json json;
        for (const River& river : rivers_) {
            json.push_back({{"source", river.source}, {"target", river.target}});
        }
        return json;
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        for (const Move& m : moves) {
            if (m.action == kClaim) rivers_.erase(m.river);
        }
        if (rivers_.empty()) {
            return {kPass, {}};
        } else {
            return {kClaim, *rivers_.begin()};
        }
    }

    const Map* map_;
    set<River, CompareRiver> rivers_;
};

}  // namespace

int main() {
    Newbie ai;
    Run(&ai);
    return 0;
}
