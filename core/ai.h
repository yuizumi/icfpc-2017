// -*- C++ -*-

#ifndef AI_H_
#define AI_H_

// NOTE(yuizumi): まだいろいろ変わる可能性あり

#include <string>
#include <vector>

#include "json.h"

// 正規化
using SiteId = int;
struct River { SiteId source, target; };  // 向きは関係ない

class Map {
public:
    static Map Parse(const Json& map_json);

    int num_sites() const { return site_ids_.size(); }

    const std::vector<River>& rivers() const { return rivers_; }  // aka. edges
    const std::vector<SiteId>& mines() const { return mines_; }

    // NOT FOR AI ROUTINES.
    using JsonSiteId = int64_t;
    JsonSiteId ToJsonId(SiteId i) const { return site_ids_[i]; }

private:
    explicit Map(std::vector<JsonSiteId> site_ids);
    SiteId ToSiteId(JsonSiteId json_id) const;

    std::vector<JsonSiteId> site_ids_;
    std::vector<River> rivers_;
    std::vector<SiteId> mines_;
};

struct Move {
    enum class Action { kPass, kClaim };
    Action action;
    River river;
};

class AI {
public:
    virtual std::string name() const { return typeid(*this).name(); }

    // ターンが来るごとに呼び出される
    // map の指すオブジェクトは Run() の実行中は存在が保証される
    virtual void Init(int punter, int num_punters, const Map* map) = 0;

    virtual void LoadState(Json&& json) = 0;
    virtual Json SaveState() = 0;

    virtual void Setup() = 0;

    // moves[i] == {"punter": i} の直前のターンにおける手
    virtual Move Gameplay(const std::vector<Move>& moves) = 0;
};

#endif  // AI_H_
