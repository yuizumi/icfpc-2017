// -*- C++ -*-

#ifndef AI_H_
#define AI_H_

// NOTE(yuizumi): まだいろいろ変わる可能性あり

#include <algorithm>
#include <string>
#include <vector>

#include "json.h"

// (0..N-1) に正規化されている
// サーバーから通知される ID とは異なるかもしれない
using SiteId = int;

struct River {
    SiteId source, target;
    River(SiteId source_in, SiteId target_in)
        : source(std::min(source_in, target_in)),
          target(std::max(source_in, target_in)) {}
    River() = default;
};

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
    enum class Action { kPass, kClaim, kSplurge, kOption };
    using Route = std::vector<SiteId>;

    Action action;
    River river;  // kClaim, kOption のときだけ
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
//   3. HandleClaim（必要な回数だけ）
//   4. Gameplay
//   5. SaveState
class AI {
public:
    virtual std::string name() const { return typeid(*this).name(); }

    // ターンが来るごとに呼び出される
    // map の指すオブジェクトは Run() の実行中は存在が保証される
    // settings の中身は追加問題文を参照のこと
    virtual void Init(int punter, int num_punters, const Map* map,
                      const Json& settings) {
        Init(punter, num_punters, map);  // 古いコードのためのもの
    }
    // DEPRECATED
    virtual void Init(int punter, int num_punters, const Map* map) {}

    // ターン開始時に呼ばれる
    virtual void LoadState(Json&& json) = 0;

    // ターン終了時に呼ばれる
    // 返したJsonは次にLoadStateで読み込まれる
    virtual Json SaveState() = 0;

    // ゲームが始まる前の準備処理
    virtual void Setup() = 0;

    // {"punter": punter} が river を claim した（ことを処理する）
    // splurge、timeout などにも対応している
    virtual void HandleClaim(int punter, const River& river) {}

    // {"punter": punter} が river を option した（ことを処理する）
    virtual void HandleOption(int punter, const River& river) {
        HandleClaim(punter, river);  // まあこれでだいたい動く？
    }

    // メインのロジックをここに書く
    // moves[i] == {"punter": i} の直前のターンにおける手
    virtual Move Gameplay(const std::vector<Move>& moves) = 0;
};

// いちいち書くのが面倒くさいので。
constexpr Move::Action kPass = Move::Action::kPass;
constexpr Move::Action kClaim = Move::Action::kClaim;
constexpr Move::Action kSplurge = Move::Action::kSplurge;

#endif  // AI_H_
