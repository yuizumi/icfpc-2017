#include "../core/ai.h"
#include "../core/driver.h"

using namespace std;

constexpr auto kPass = Move::Action::kPass;

class NullAI : public AI {
    void Init(int punter, int num_punters, const Map& map) override {}

    void LoadState(Json&& json) override {}
    Json SaveState() override { return Json(); }

    void Setup() override {}

    Move Gameplay(const std::vector<Move>& moves) override { return {kPass, {}}; }
};

int main() {
    NullAI ai;
    Run(&ai);
    return 0;
}
