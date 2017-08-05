#include "driver.h"

#include <cassert>
#include <iostream>
#include <string>

#include "json.h"

using namespace std;

namespace {

Json Read() {
    int length;
    char colon;
    cin >> length >> colon;
    assert(iss && colon == ':');
    std::unique_ptr<char[]> body(new char[length + 1]);
    cin.read(body, length);
    return Json::parse(body.get());
}

void Write(const Json& json) {
    const string json_str = json.dump();
    cout << json_str.length() << ":" << json_str;
    cout.flush();
}

Map ParseMap(const Json& json) {
    Map map;
    for (const auto& site : json["sites"]) {
        map.sites().push_back(site["id"]);
    }
    for (const auto& river : json["rivers"]) {
        map.rivers().push_back({river["source"], river["target"]});
    }
    map.mines().insert(map.mines().end(),
                       json["mines"].begin(), json["mines"].end());
    return map;
}

std::vector<Move> ParseMove(const Json& json) {
    std::vector<Move> moves(json["moves"].size());

    for (const auto& e : json["moves"]) {
        const auto claim = e.find("claim");
        if (claim != e.end()) {
            const SiteId source = (*claim)["source"];
            const SiteId target = (*claim)["target"];
            moves[(*claim)["punter"]] = {Move::Action::kClaim, {source, target}};
        } else {
            moves[e["pass"]["punter"]] = {Move::Action::kPass, {}};
        }
    }

    return moves;
}

void DoSetup(AI* ai, Json&& json) {
    const int id = json["punter"];
    ai->Init(id, json["punters"], ParseMap(json["map"]));
    ai->Setup();
    json["custom"] = ai->SaveState();
    Write({{"ready", id}, {"state", json}});
}

void DoGameplay(AI* ai, Json&& json) {
    Json state = json["state"];
    const int id = state["punter"];
    ai->Init(id, state["punters"], ParseMap(state["map"]));
    ai->LoadState(std::move(state["custom"]));
    Move next_move = ai->Gameplay(ParseMove(json["move"]));
    state["custom"] = ai->SaveState();

    switch (next_move.action) {
    case Move::Action::kPass:
        Write({{"pass", {{"punter", id}}}, {"state", state}});
        break;
    case Move::Action::kClaim:
        const SiteId source = next_move.river.source;
        const SiteId target = next_move.river.target;
        const Json claim = {
            {"punter", id}, {"source", source}, {"target", target}};
        Write({{"claim", claim}, {"state", state}});
        break;
    }
}

}  // namespace

void Run(AI* ai) {
    const string name = ai->name();
    Write({{"me", name}});
    Json hello = Read();
    assert(hello["you"] == name);

    Json json = Read();

    if (json.count("stop")) {
        return;
    }
    if (json.count("move")) {
        DoGameplay(ai, std::move(json));
    } else {
        DoSetup(ai, std::move(json));
    }
}
