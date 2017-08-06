#include "reachable_set.h"

#include <initializer_list>

using namespace std;

bool ReachableSet::Test(const River& river, SiteId* older, SiteId* newer) const {
    if (Has(river.source)) {
        *older = river.source;
        *newer = river.target;
        return true;
    }
    if (Has(river.target)) {
        *older = river.target;
        *newer = river.source;
        return true;
    }
    return false;
}

void ReachableSet::HandleClaim(const River& river) {
    reachables_.insert(river.source);
    reachables_.insert(river.target);
}

void from_json(const Json& json, ReachableSet& value) {
    value.reachables_ = json.get<set<SiteId>>();
}

void to_json(Json& json, const ReachableSet& value) {
    json = value.reachables_;
}
