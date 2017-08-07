#ifndef PUNTER_INFO_H_
#define PUNTER_INFO_H_

#include <set>

#include "../core/ai.h"
#include "../core/json.h"
#include "union_find.h"

class PunterInfo {
public:
    PunterInfo(const Map& map, bool option);
    PunterInfo() = default;

    // 残っている option の数
    int num_options() const { return num_options_; }

    bool UnionSet(SiteId x, SiteId y) {
        return forest_.UnionSet(x, y);
    }
    bool FindSet(SiteId x, SiteId y) {
        return forest_.FindSet(x, y);
    }
    int Size(SiteId x) { return forest_.size(x); }

    // site にλ鉱脈がつながっていたら真
    bool IsReachable(SiteId site) const {
        return reachables_.count(site) > 0;
    }

    // river が以下の条件の少なくとも一方を満たすときに真
    // (a) λ鉱脈につながっている（異なる）連結成分同士をつなごうとしている
    // (b) 大きさが 2 以上の連結成分を新たにλ鉱脈につなごうとしている
    bool IsConnectingRiver(const River& river);

    // river のちょうど一方がλ鉱脈につながっているときに真
    bool IsExpandingRiver(const River& river) const {
        // 一方が reachable で他方が !reachable のときに真
        return IsReachable(river.source) != IsReachable(river.target);
    }

    void HandleClaim(const River& river);
    void HandleOption(const River& river);

private:
    friend void from_json(const Json& json, PunterInfo& value);
    friend void to_json(Json& json, const PunterInfo& value);

    bool IsConnectingRiver(SiteId older, SiteId newer);

    int num_options_;
    UnionFind forest_;
    std::set<SiteId> reachables_;
};

void from_json(const Json& json, PunterInfo& value);
void to_json(Json& json, const PunterInfo& value);

#endif  // PUNTER_INFO_H_
