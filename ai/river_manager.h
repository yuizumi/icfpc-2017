#ifndef RIVER_MANAGER_H_
#define RIVER_MANAGER_H_

#include <map>

#include "../core/ai.h"
#include "../core/json.h"
#include "river_set.h"

class RiverManager {
public:
    using RiverMap = std::map<River, Move::Action, CompareRiver>;

    explicit RiverManager(const Map& map, bool option);
    RiverManager() = default;  // For Json.

    const RiverMap& map() const { return map_; }

    Move::Action Get(const River& river) const {
        const auto iter = map_.find(river);
        return (iter != map_.end()) ? iter->second : kPass;
    }

    void HandleClaim(const River& river);  // claim or option

private:
    friend void from_json(const Json& json, RiverManager& value);
    friend void to_json(Json& json, const RiverManager& value);

    RiverMap map_;
    bool option_;
};

void from_json(const Json& json, RiverManager& value);
void to_json(Json& json, const RiverManager& value);

#endif  // RIVER_MANAGER_H_