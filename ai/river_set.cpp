#include "river_set.h"

void from_json(const Json& json, River& river) {
    river = River(json[0], json[1]);
}
void to_json(Json& json, const River& river) {
    json = Json::array({river.source, river.target});
}

void from_json(const Json& json, RiverSet& river_set) {
    river_set.insert(json.begin(), json.end());
}
void to_json(Json& json, const RiverSet& river_set) {
    json = Json::array();
    for (const River& river : river_set) json.push_back(river);
}
