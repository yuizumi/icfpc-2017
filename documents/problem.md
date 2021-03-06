# 1 Introduction

暇になったら読む

## 1.1 Refinements

暇に

# 2 Games

(Wiki引用)

目標は2人「以上」で行うゲームにおいて、スコアを最大化し勝利することである。

無向グラフ G=(V,E,M) が与えられる。

- V: ノード（拠点）
- E: エッジ（川）
- M: λが埋蔵された場所

グラフ上のエッジをターン制で一本づつ取っていく。
一度取られたエッジは他のプレイヤーが取ることができない。

エッジの数と同じだけの手番（move）がプレイされた時点でゲーム終了（つまりmoveの数/メンバーの数のターンで終了となる）。  
moveとは以下のいずれかを指す

* claim: エッジを取る
* pass: エッジを取らず自分のターンをパスする

その後以下の規則に沿ってスコアが計算される。

# 3 スコア

λの集合を{M_0, ... , M_m-1}, ノードの集合を{S_0, .., S_s-1}とする。

プレイヤーPのスコアは **score(P, M_a, S_b) = 0 (取得したエッジを使ってM_aからS_bへのルートが存在しない) | d * d (M_aからS_bへの最短距離d)** の総和

## 3.1 Examples

暇に

## 3.2 制限時間

## 3.3 ゾンビプレイヤー

movesを10回タイムアウトした場合、そのプレイヤーはサーバから接続を切られその後の操作は無視される

# 4. The lambda punter protocol

JSON形式. Online modeとOffline modeが存在する  

* Online mode

プレイヤー同士はTCP/IPで通信する

* Offline mode

プレイヤー同士はUnix pipesで通信する

(WIP)
Offline mode supports only one punter running at once. Thus the game state must be serialised
between moves. The lambda punter server coordinates the punters, running each in turn and
keeping hold of the game state for each punter alongside each message.

## 4.1 メッセージ

全てのデータのやりとりはJSONで行われ、形式は *n:json*  

* n
  - *json*のサイズ (単位: byte)
  - 整数
  - 9桁(1GB)を超えない
* json
  - 後述する形式のJSON文字列

以下の説明では*n*を省略する

## 4.2 Online mode

ゲームサーバをS, プレイヤーをPとし, S->Pがサーバからプレイヤーの通信, P->Sがプレイヤーからサーバへの通信とする

* Handshake

**P->S {"me": *name*}**  
**S->P {"you": *name*}**

* Setup
* Gameplay
* Scoring

## 4.3 Offline mode

* Setup
* Gameplay
* Scoring

## 4.4 Timeouts

# 5 Game servers

# 6 Determining the winner

## 6.1 The judges’ prize

# 7 Submission

# A Sample play

# B Version history
