#include "river_set.h"

Json SerializeRiverSet(const RiverSet& river_set) {
    Json json = Json::array();
    for (const River& river : river_set) {
        json.push_back({{"source", river.source}, {"target", river.target}});
    }
    return json;
}

RiverSet DeserializeRiverSet(const Json& json) {
    RiverSet river_set;
    for (const Json& river : json) {
        const SiteId source = river["source"];
        const SiteId target = river["target"];
        river_set.insert({source, target});
    }
    return river_set;
}
