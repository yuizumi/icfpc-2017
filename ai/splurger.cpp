#include <iostream>
#include <vector>
#include <set>

#include "../core/ai.h"
#include "../core/driver.h"
#include "reachable_set.h"
#include "river_set.h"
#include "union_find.h"

using namespace std;

namespace {

// splurgeのテスト用
class Splurger : public AI {
    void Init(int id, int num_punters, const Map* map,
              const Json& settings) override {
        id_ = id;
        num_punters_ = num_punters;
        map_ = map;
    }

    void LoadState(Json&& json) override {
        rivers_ = json["rivers"].get<decltype(rivers_)>();
        forests_ = json["forests"].get<decltype(forests_)>();
        reachables_ = json["reachables"].get<decltype(reachables_)>();
        pass_ = json["pass"].get<decltype(pass_)>();
        splurge_size_ = json["splurge_size"].get<decltype(splurge_size_)>();
    }

    Json SaveState() override {
        return {{"rivers", rivers_},
                {"forests", forests_},
                {"reachables", reachables_},
                {"pass", pass_},
                {"splurge_size", splurge_size_}};
    }

    void Setup() override {
        rivers_.insert(map_->rivers().begin(), map_->rivers().end());
        for (int i = 0; i < num_punters_; i++) {
            forests_.emplace_back(map_->num_sites());
            reachables_.emplace_back(map_->mines());
        }
        splurge_size_ = min(5, int(map_->rivers().size() / num_punters_ / 3));
        pass_ = 0;
    }

    void HandleClaim(int punter, const River& river) override {
        rivers_.erase(river);
        forests_[punter].UnionSet(river.source, river.target);
        reachables_[punter].Claim(river);
    }

    void FindSplurge(Move::Route& route, int size) {
        int current_size = int(route.size()) - 1;
        if (current_size >= size) return;
        SiteId now = route[route.size() - 1];
        for (const SiteId adjacent : adjacents_[now]) {
            if (!rivers_.count({now, adjacent})) continue;

            // Riverの重複判定
            bool dup = false;
            for (int i = 0; i < int(route.size()) - 1; i ++) {
                dup = (route[i] == now && route[i + 1] == adjacent)
                    || (route[i] == adjacent && route[i + 1] == now);
                if (dup) break;
            } 
            if (dup) continue;

            // 更に繋げる
            route.push_back(adjacent);
            FindSplurge(route, size);
            return;
        }
    }

    Move Gameplay(const std::vector<Move>& moves) override {
        // 序盤はsplurge、それ以外は以下の雑な行動
        if (splurge_size_ <= 0) {
            for (const River& river : rivers_) {
                SiteId older, newer;
                if (reachables_[id_].Test(river, &older, &newer) &&
                    !reachables_[id_].Has(newer)) {
                    for (SiteId mine : map_->mines()) {
                        if (forests_[id_].FindSet(older, mine)) {
                            return {kClaim, river};
                        }
                    }
                }
                if (rivers_.empty()) {
                    pass_++;
                    return {kPass, {}};
                } else {
                    const River& river = *rivers_.begin();
                    return {kClaim, river};
                }
            }
        }
        // splurgeパワーを貯める
        if (pass_ + 1 < splurge_size_) {
            pass_++;
            return {kPass, {}};
        }
        // 隣接リストを作る
        adjacents_ = vector<vector<SiteId>>(map_->num_sites(), vector<SiteId>());
        for (const River& river : map_->rivers()) {
            adjacents_[river.source].push_back(river.target);
            adjacents_[river.target].push_back(river.source);
        }
        // splurge出来るルートを探す
        for (const River& river : rivers_) {
            SiteId older, newer;
            if (reachables_[id_].Test(river, &older, &newer) &&
                !reachables_[id_].Has(newer)) {
                Move move;
                move.action = kSplurge;
                move.route = {older, newer};
                FindSplurge(move.route, splurge_size_);
                pass_ -= move.route.size() - 2;
                splurge_size_--;
                return move;
            }
        }
        pass_++;
        splurge_size_--;
        return {kPass, {}};
    }

    int id_, num_punters_;
    const Map* map_;
    RiverSet rivers_;
    vector<UnionFind> forests_;
    vector<ReachableSet> reachables_;
    int pass_;
    int splurge_size_;
    vector<vector<SiteId>> adjacents_;
};

}  // namespace

int main() {
    Splurger ai;
    Run(&ai);
    return 0;
}


