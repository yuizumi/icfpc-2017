#include "river_manager.h"

RiverManager::RiverManager(const Map& map, bool option)
    : option_(option) {
    for (const River& river : map.rivers())
        map_[river] = -1;
}

Move::Action RiverManager::NextAction(int punter, const River& river) const {
    const auto iter = map_.find(river);
    if (iter == map_.end() || iter->second == punter)
        return kPass;
    return (iter->second < 0) ? kClaim : kOption;
}

void RiverManager::HandleClaim(int punter, const River& river) {
    auto iter = map_.find(river);
    if (option_ && iter->second < 0) {
        iter->second = punter;
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
