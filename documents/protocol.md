Protocol (v1.3)
===============

## 0. Handshake

|No. |方向 |JSON|
|----|----|----|
|0|P->S| {"me": name}|
|1|S->P| {"you": name}|

- name : String (punter name)

※ オフラインモードではインスタンス起動毎に最初に行われる
 (Setup, Gameplay各ターン, Scoringのそれぞれ)

## 1. Setup

|No. |方向 |JSON|
|----|----|----|
|0|S->P| {"punter" : p, "punters" : n, "map" : map, "settings" : settings} |
|1|P->S| {"ready" : p, "state" : state, "futures" : futures}|

- p : PunterId (punter id)
- n : Nat (total number of punters)
- map : Map (the map)
- **[オフラインのみ]** state: GameState (initial game state) <- 次のターンに引き継ぎたい情報
- **[Future拡張]** settings = {"futures" : true};  future拡張が有効かどうか
- **[Future拡張]** futures : [{"source" : SiteId, "target" : SiteId}]; futureの賭けたい内容
- Pid = Nat
- Map = {"sites" : [Site]; "rivers" : [River]; "mines" : [SiteId]; }
- Site = {"id" : SiteId}
- River = {"source" : SiteId; "target" : SiteId; }
- SiteId = Nat

## 2. Gameplay

|No. |方向 |JSON|
|----|----|----|
|0|S->P| {"move" : {"moves" : moves}, "state" : state} |
|1|P->S| move & {"state" : state'}|

- moves : [Move] (moves from previous turn) これまで
- move : Move (P’s chosen move)
- Move = {"claim" : {"punter" : PunterId; "source" : SiteId; "target" : SiteId}} | {"pass" : {"punter" : PunterId}}
- **[オフラインのみ]** state : GameState (game state before this move)
- **[オフラインのみ]** state' : GameState (game state after this move)

## 3. Scoring

|No. |方向 |JSON|
|----|----|----|
|0|S->P| {"stop" : {"moves" : moves; "scores" : scores, "state" : state}} |

- moves : [Move] (collection of moves)
- scores : [Score] (collection of scores)
- Score = {"punter" : PunterId; "score" : Nat}
- **[オフラインのみ]** state : GameState (game state after P’s final move)

## 4. Timeout (オンラインモードのみ)

タイムアウトするとメッセージが来る

|No. |方向 |JSON|
|----|----|----|
|0|S->P| {"timeout" : t}|

- t : Float (length of the timeout, 秒?)

※ オフラインモードでは単純にkillされる

10回タイムアウトが連続するとzombie扱いになる
