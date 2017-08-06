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
    SiteId ToSiteId(JsonSiteId json_id) const;

private:
    explicit Map(std::vector<JsonSiteId> site_ids);

    std::vector<JsonSiteId> site_ids_;
    std::vector<River> rivers_;
    std::vector<SiteId> mines_;
};

struct Move {
    enum class Action { kPass, kClaim, kSplurge };
    using Route = std::vector<SiteId>;

    Action action;
    River river;  // kClaim のときだけ
    Route route;  // kSplurge のときだけ

    Move(Action action, River river) : action(action), river(river) {}
    Move() = default;
};

// 呼ばれる順番
//
// ゲームが始まる前
//   1. Init
//   2. Setup
//   3. SaveState
//
// ゲームの途中（ターンごと）
//   1. Init
//   2. LoadState
//   3. Gameplay
//   4. SaveState
class AI {
public:
    virtual std::string name() const { return typeid(*this).name(); }

    // ターンが来るごとに呼び出される
    // map の指すオブジェクトは Run() の実行中は存在が保証される
    virtual void Init(int punter, int num_punters, const Map* map) = 0;

    // ターン開始時に呼ばれる
    virtual void LoadState(Json&& json) = 0;

    // ターン終了時に呼ばれる
    // 返したJsonは次にLoadStateで読み込まれる
    virtual Json SaveState() = 0;

    // ゲームが始まる前の準備処理
    virtual void Setup() = 0;

    // メインのロジックをここに書く
    // moves[i] == {"punter": i} の直前のターンにおける手
    virtual Move Gameplay(const std::vector<Move>& moves) = 0;
};

// いちいち書くのが面倒くさいので。
constexpr Move::Action kPass = Move::Action::kPass;
constexpr Move::Action kClaim = Move::Action::kClaim;
constexpr Move::Action kSplurge = Move::Action::kSplurge;

#endif  // AI_H_
