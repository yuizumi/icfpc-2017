#include "driver.h"

#include <assert.h>
#include <unistd.h>
#include <string>

#include "json.h"

namespace {

Json Read() {
    int len = 0;
    while (true) {
        char buf[1];
        while (read(0, buf, 1) <= 0) ;
        char ch = buf[0];
        if (ch == ':') break;
        assert(ch >= '0' && ch <= '9');
        len = len * 10 + (ch - '0');
    }
    std::unique_ptr<char[]> body(new char[len + 1]);
    body[len] = '\0';
    int pos = 0;
    while (pos < len) {
        int read_len = read(0, body.get() + pos, len - pos);
        if (read_len > 0) pos += read_len;
    }
    return Json::parse(body.get());
}

void WriteString(const std::string& str) {
    int len = str.length();
    int pos = 0;
    while (pos < len) {
	int write_len = write(1, str.c_str() + pos, len - pos);
	if (write_len > 0) pos += write_len;
    }
}

void Write(const Json& json) {
    const std::string json_str = json.dump();
    WriteString(std::to_string(json_str.length()) + ":");
    WriteString(json_str);
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
    const Map map = ParseMap(json["map"]);
    ai->Init(id, json["punters"], &map);
    ai->Setup();
    json["custom"] = ai->SaveState();
    Write({{"ready", id}, {"state", json}});
}

void DoGameplay(AI* ai, Json&& json) {
    Json state = json["state"];
    const int id = state["punter"];
    const Map map = ParseMap(state["map"]);
    ai->Init(id, state["punters"], &map);
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
    const std::string name = ai->name();
    Write({{"me", name}});
    Json handshake = Read();
    assert(handshake["you"] == name);

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
