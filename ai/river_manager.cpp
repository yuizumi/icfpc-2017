#include "river_manager.h"

RiverManager::RiverManager(const Map& map, bool option)
    : option_(option) {
    for (const River& river : map.rivers()) map_[river] = kClaim;
}

void RiverManager::HandleClaim(const River& river) {
    auto iter = map_.find(river);
    if (option_ && iter->second == kClaim) {
        iter->second = kOption;
    } else {
        map_.erase(iter);
    }
}

void from_json(const Json& json, RiverManager& value) {
    for (const Json& e : json["rivers"])
        value.map_.emplace(e[0], e[1]);
    value.option_ = json["option"];
}

void to_json(Json& json, const RiverManager& value) {
    json["rivers"] = Json::array();
    for (const auto& e : value.map_) {
        json["rivers"].push_back(Json::array({e.first, e.second}));
    }
    json["option"] = value.option_;
}
