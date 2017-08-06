#ifndef REACHABLE_SET_H_
#define REACHABLE_SET_H_

#include <set>

#include "../core/ai.h"
#include "../core/json.h"

class ReachableSet {
public:
    explicit ReachableSet(const std::vector<SiteId>& mines)
        : reachables_(mines.begin(), mines.end()) {}
    ReachableSet() = default;  // For Json::get<>().

    // |site| が何らかのλ鉱脈に接続されていれば真
    bool Has(SiteId site) const { return reachables_.count(site) > 0; }

    // |river| の少なくとも一端が何らかのλ鉱脈に接続されていれば真
    // 真のときは |older| がλ鉱脈側、|newer| が反対側の頂点
    bool Test(const River& river, SiteId* older, SiteId* newer) const;

    void Claim(const River& river);

private:
    friend void from_json(const Json& json, ReachableSet& value);
    friend void to_json(Json& json, const ReachableSet& value);

    std::set<SiteId> reachables_;
};

void from_json(const Json& json, ReachableSet& value);
void to_json(Json& json, const ReachableSet& value);

#endif  // REACHABLE_SET_H_
