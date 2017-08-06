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

Json SerializeRiverSet(const RiverSet& river_set);
RiverSet DeserializeRiverSet(const Json& json);

#endif  // RIVER_SER_H_
