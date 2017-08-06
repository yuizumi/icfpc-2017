#include <iostream>
#include <tuple>
#include <queue>
#include <set>
#include <map>

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

class Coco : public AI {
    using SiteDistance = tuple<SiteId, int>;

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

        // Siteに対してつながっているriver全てを列挙したmap
        for (const River river : map_->rivers()) {
            riverMap[river.source].push_back(river.target);
            riverMap[river.target].push_back(river.source);
        }

        // 各mineから各Siteへの距離を先に計算するやつ
        for (const SiteId mine : map_->mines()) {
            // mineからの全連結Siteへのdepth
            queue<SiteDistance> queue;
            set<SiteId> visited;
            queue.push(make_tuple(mine, 0));
            visited.insert(mine);
            cerr << "mine:" << mine << endl;
            while (!queue.empty()) {
                SiteDistance sd = queue.front();
                queue.pop();

                int site = get<0>(sd);
                int depth = get<1>(sd);
                cerr << site << ":" << depth << endl;

                for (auto site :riverMap[site]) {
                    if(visited.find(site) != visited.end()) {continue;}
                    visited.insert(site);
                    queue.push(make_tuple(site, depth + 1));
                    mineToSitesDistance[mine][site] = depth + 1;
                }
            }
        }
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
    map<SiteId, vector<SiteId>> riverMap;
    map<SiteId, map<SiteId, int>> mineToSitesDistance;
    set<River, CompareRiver> rivers_;
};

}  // namespace

int main() {
    Coco ai;
    Run(&ai);
    return 0;
}
