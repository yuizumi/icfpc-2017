// -*- C++ -*-

#ifndef AI_H_
#define AI_H_

// NOTE(yuizumi): まだいろいろ変わる可能性あり

#include <string>
#include <vector>

#include "../thirdparty/json.hpp"

using Json = nlohmann::json;

using SiteId = int;
struct River { SiteId source, target; };  // 向きは関係ない

class Map {
public:
    const std::vector<SiteId>& sites() const { return sites_; }   // aka. nodes
    const std::vector<River>& rivers() const { return rivers_; }  // aka. edges
    const std::vector<SiteId>& mines() const { return mines_; }

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
    // ターンが来るごとに呼び出される
    virtual void Init(int punter, int num_punters, const Map& map) = 0;

    virtual void LoadState(Json&& json) = 0;
    virtual Json SaveState() = 0;

    virtual void Setup() = 0;

    // moves[i] == {"punter": i} の直前のターンにおける手
    virtual Move Gameplay(const std::vector<Move>& moves) = 0;
};

#endif  // AI_H_
