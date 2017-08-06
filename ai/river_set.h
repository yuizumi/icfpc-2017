// -*- C++ -*-
#ifndef RIVER_SET_H_
#define RIVER_SET_H_

#include <set>

#include "../core/ai.h"
#include "../core/json.h"

class CompareRiver {
public:
    bool operator()(const River& r1, const River& r2) const {
        if (r1.source != r2.source) return r1.source < r2.source;
        if (r1.target != r2.target) return r1.target < r2.target;
        return false;
    }
};

using RiverSet = std::set<River, CompareRiver>;

void from_json(const Json& json, River& river);
void to_json(Json& json, const River& river);
void from_json(const Json& json, RiverSet& river_set);
void to_json(Json& json, const RiverSet& river_set);

#endif  // RIVER_SER_H_
