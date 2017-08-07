// -*- C++ -*-
#ifndef RIVER_MANAGER_H_
#define RIVER_MANAGER_H_

#include <map>

#include "../core/ai.h"
#include "../core/json.h"
#include "river_set.h"

class RiverManager {
public:
    // 値は内部状態なので AI では使わないこと
    using RiverMap = std::map<River, int, CompareRiver>;

    explicit RiverManager(const Map& map, bool option);
    RiverManager() = default;  // For Json.

    const RiverMap& map() const { return map_; }

    // river に対して次にとれるアクションを返す
    Move::Action NextAction(int punter, const River& river) const;

    // claim or option
    void HandleClaim(int punter, const River& river);

private:
    friend void from_json(const Json& json, RiverManager& value);
    friend void to_json(Json& json, const RiverManager& value);

    RiverMap map_;
    bool option_;
};

void from_json(const Json& json, RiverManager& value);
void to_json(Json& json, const RiverManager& value);

#endif  // RIVER_MANAGER_H_
