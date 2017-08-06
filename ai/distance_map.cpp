#include "distance_map.h"

#include <queue>
#include <tuple>

using namespace std;

DistanceMap::DistanceMap(const Map& map) {
    vector<vector<SiteId>> adj_list(map.num_sites());
    for (const River& r : map.rivers()) {
        adj_list[r.source].push_back(r.target);
        adj_list[r.target].push_back(r.source);
    }
    for (const SiteId mine : map.mines()) {
        queue<tuple<SiteId, int>> q;
        q.push(make_tuple(mine, 0));
        while (!q.empty()) {
            const auto sd = q.front();
            q.pop();
            const SiteId site = get<0>(sd);
            const int dist = get<1>(sd);
            for (const SiteId adj : adj_list[site]) {
                if (dist_[adj].emplace(mine, dist + 1).second) {
                    q.push(make_tuple(adj, dist + 1));
                }
            }
        }
    }
}

void from_json(const Json& json, DistanceMap& value) {
    for (size_t i = 0; i < json.size(); i++) {
        for (const Json& e : json[i])
            value.dist_[i][e[0]] = e[1];
    }
}

void to_json(Json& json, const DistanceMap& value) {
    json = Json::array();
    for (size_t i = 0; i < json.size(); i++) {
        json.push_back(Json::array());
        for (const auto& e : value.dist_[i]) {
            json[i].push_back(Json::array({e.first, e.second}));
        }
    }
}
