#include "../ai.h"

class NaiveAI : public AI
{
  public:
    std::string name() const
    {
        return "naive";
    }

    void Setup(int punter, int num_punters, const Map &map){

    };

    /**
    * 各ターンの処理
    * moves[i] == {"punter": i} が指した最後に指した手
    * TODO: setupの引数と使用済みのriverくれ (できれば賭けたfutureも)
    */
    Move Play(const std::vector<Move> &moves)
    {
        Move m;
        m.type = MoveType::kClaim;
        m.source = 0;
        m.target = 1;
        return m;
    };

    /**
    * ゲーム終了時の処理
    * moves[i] == {"punter": i} が指した最後に指した手
    * scores[i] == {"punter": i} が指した最後に指した手
    */
    void Stop(const std::vector<Move> &moves, std::vector<int> scores){};
};

int main() {}
