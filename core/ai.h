// -*- C++ -*-

#ifndef AI_H_
#define AI_H_

// NOTE(yuizumi): まだいろいろ変わる可能性あり

#include <string>
#include <vector>

#include "json.h"

using SiteId = int;  // int64_t?
struct River { SiteId source, target; };  // 向きは関係ない

class Map {
public:
    const std::vector<SiteId>& sites() const { return sites_; }   // aka. nodes
    std::vector<SiteId>& sites() { return sites_; }
    const std::vector<River>& rivers() const { return rivers_; }  // aka. edges
    std::vector<River>& rivers() { return rivers_; }
    const std::vector<SiteId>& mines() const { return mines_; }
    std::vector<SiteId>& mines() { return mines_; }

private:
    std::vector<SiteId> sites_;
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
