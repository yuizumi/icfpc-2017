// -*- C++ -*-
#ifndef DISTANCE_MAP_H_
#define DISTANCE_MAP_H_

#include <unordered_map>
#include <vector>

#include "../core/ai.h"
#include "../core/json.h"

constexpr int kUnreachable = 0;

class DistanceMap {
public:
    explicit DistanceMap(const Map& map);
    DistanceMap() = default;  // For Json::get<>().

    int Get(SiteId site, SiteId mine) const {
        const auto iter = dist_[site].find(mine);
        return (iter != dist_[site].end()) ? iter->second : kUnreachable;
    }

private:
    friend void from_json(const Json& json, DistanceMap& value);
    friend void to_json(Json& json, const DistanceMap& value);

    // dist_[site][mine] = site から mine までの距離
    std::vector<std::unordered_map<SiteId, int>> dist_;
};

void from_json(const Json& json, DistanceMap& value);
void to_json(Json& json, const DistanceMap& value);

#endif  // DISTANCE_MAP_H_
