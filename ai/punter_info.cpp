#include "punter_info.h"

PunterInfo::PunterInfo(const Map& map, bool option)
    : num_options_(option),
      forest_(map.num_sites()),
      reachables_(map.mines().begin(), map.mines().end()) {}

bool PunterInfo::IsConnectingRiver(const River& river) {
    if (IsReachable(river.source)) {
        return IsConnectingRiver(river.source, river.target);
    }
    if (IsReachable(river.target)) {
        return IsConnectingRiver(river.target, river.source);
    }
    return false;
}

// older は必ずλにつながっている
bool PunterInfo::IsConnectingRiver(SiteId older, SiteId newer) {
    // !FindSet(...) == older と newer が異なる連結成分にいる
    return !FindSet(older, newer) &&   (IsReachable(newer) || Size(newer) >= 2);
}

void PunterInfo::HandleClaim(const River& river) {
    // TODO(yuizumi): おそらく一方が reachables_ のときだけ？
    reachables_.insert(river.source);
    reachables_.insert(river.target);
    forest_.UnionSet(river.source, river.target);
}

void PunterInfo::HandleOption(const River& river) {
    HandleClaim(river);
    --num_options_;
}

void from_json(const Json& json, PunterInfo& value) {
    value.num_options_ = json["num_options"];
    value.forest_ = json["forest"].get<decltype(value.forest_)>();
    value.reachables_ = json["reachables"].get<decltype(value.reachables_)>();
}

void to_json(Json& json, const PunterInfo& value) {
    json["num_options"] = value.num_options_;
    json["forest"] = value.forest_;
    json["reachables"] = value.reachables_;
}
