// TODO(yuizumi): もうちょっとよく考える
// NOTE(yuizumi): いろいろ変わる可能性あり
#ifndef AI_H_
#define AI_H_

#include <string>
#include <vector>

using SiteId = int;  // いらないかも
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

enum class MoveType { kClaim, kPass };

struct Move {
    MoveType type;  // kClaim in most cases
    int source;
    int target;
};

class AI {
public:
    virtual std::string name() const = 0;

    virtual void Setup(int punter, int num_punters, const Map& map) = 0;
    // moves[i] == {"punter": i} が指した最後に指した手
    virtual Move Play(const std::vector<Move>& moves) = 0;
    // moves[i] == {"punter": i} が指した最後に指した手
    // scores[i] == {"punter": i} が指した最後に指した手
    virtual void Stop(const std::vector<Move>& moves, std::vector<int> scores) = 0;
};

#endif  // AI_H_
