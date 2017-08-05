* λ Punter FX (web interface): http://punter.inf.ed.ac.uk/puntfx/
* Game Status: http://punter.inf.ed.ac.uk/status.html

# 概要

ゲームサーバ 129.215.197.1でプレイするためのweb interface  
1 portにつき1つのゲームが動いており、全ゲームの結果はGame StatusのMap Nameのリンクから確認可能

# 遊び方

### 1. Game StatusからStatus: Waiting for puntersになっているportを選ぶ

この時`Offline`や`Game in progress`を選ぶとConnection droppedと表示され通信に失敗する

### 2. λ Punter FXのConnection settingsに設定を入力

Game port: 1で確認したport
Punter name: 任意の文字列, ユニークでなくてもよい

入力後Connect Buttonを押して通信を試みる  
失敗したらportを選び直す

### 3. ゲームプレイ

ルールに沿ってPunterの動きをサーバに送信する  
Web上からはPassしか行うことができない

### 4. スコア発表

ゲーム終了後, ScoringのJSON形式に従って結果が送信される (未検証)
