#include "driver.h"

#include <assert.h>
#include <unistd.h>
#include <algorithm>
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

void ParseMoveTurn(const Json& json, const Map& map, int turn,
                   std::vector<Move>* moves) {
    int num = moves->size();
    for (int i = 0; i < num; i++) {
        const auto& e = json["moves"][i + turn * num];
        const auto claim = e.find("claim");
        if (claim != e.end()) {
            const SiteId source = map.ToSiteId((*claim)["source"]);
            const SiteId target = map.ToSiteId((*claim)["target"]);
            const int punter = (*claim)["punter"];
            (*moves)[punter] = {kClaim, {source, target}};
            continue;
        }
        const auto splurge = e.find("splurge");
        if (splurge != e.end()) {
            std::vector<SiteId> route;
            for (Map::JsonSiteId json_id : (*splurge)["route"])
                route.push_back(map.ToSiteId(json_id));
            const int punter = (*splurge)["punter"];
            (*moves)[punter] = {kSplurge, {}};
            (*moves)[punter].route = std::move(route);
            continue;
        }
        const auto option = e.find("option");
        if (option != e.end()) {
            const SiteId source = map.ToSiteId((*option)["source"]);
            const SiteId target = map.ToSiteId((*option)["target"]);
            const int punter = (*option)["punter"];
            (*moves)[punter] = {kOption, {source, target}};
            continue;
        }
        (*moves)[e["pass"]["punter"]] = {kPass, {}};
    }
}

void ParseMove(const Json& json, const Map& map, int punter_id,
               std::vector<Move>* moves, AI* ai) {
    int num_turns = json["moves"].size() / moves->size();
    int num_punters = moves->size();
    for (int turn = 0; turn < num_turns; turn++) {
        ParseMoveTurn(json, map, turn, moves);
        for (int i = 0; i < num_punters; i++) {
            const int punter = (punter_id + i) % num_punters;
            const Move& move = (*moves)[punter];
            switch (move.action) {
                case kPass:
                    break;
                case kClaim:
                case kOption:
                    ai->HandleClaim(punter, move.river);
                    break;
                case kSplurge:
                    const auto& route = move.route;
                    for (size_t i = 1; i < route.size(); i++) {
                        ai->HandleClaim(punter, {route[i - 1], route[i]});
                    }
                    break;
            }
        }
    }
}

void DoSetup(AI* ai, Json&& json) {
    const int id = json["punter"];
    const Map map = Map::Parse(json["map"]);
    ai->Init(id, json["punters"], &map, json["settings"]);
    ai->Setup();
    json["custom"] = ai->SaveState();
    Write({{"ready", id}, {"state", json}});
}

void DoGameplay(AI* ai, Json&& json) {
    Json state = json["state"];
    const int id = state["punter"];
    const int num_punters = state["punters"];
    const Map map = Map::Parse(state["map"]);
    const Json settings = state["settings"];
    ai->Init(id, num_punters, &map, settings);
    ai->LoadState(std::move(state["custom"]));
    std::vector<Move> moves(num_punters);
    ParseMove(json["move"], map, id, &moves, ai);
    Move next_move = ai->Gameplay(moves);
    state["custom"] = ai->SaveState();

    switch (next_move.action) {
        case kPass:
            Write({{"pass", {{"punter", id}}}, {"state", state}});
            break;
        case kClaim:
        case kOption: {
            const auto source = map.ToJsonId(next_move.river.source);
            const auto target = map.ToJsonId(next_move.river.target);
            std::string key;
            Json value;
            if (HasSetting(settings, "splurges")) {
                key = "splurge";
                value = {{"punter", id}, {"route", {source, target}}};
            } else {
                key = (next_move.action == kOption) ? "option" : "claim";
                value = {{"punter", id}, {"source", source}, {"target", target}};
            }
            Write({{key, value}, {"state", state}});
            break;
        }
        case kSplurge: {
            Json route = Json::array();
            for (SiteId site : next_move.route)
                route.push_back(map.ToJsonId(site));
            const Json splurge = {{"punter", id}, {"route", route}};
            Write({{"splurge", splurge}, {"state", state}});
            break;
        }
    }
}

}  // namespace

Map::Map(std::vector<JsonSiteId> site_ids) {
    std::sort(site_ids.begin(), site_ids.end());
    site_ids_ = std::move(site_ids);
}

Map Map::Parse(const Json& json) {
    std::vector<JsonSiteId> site_ids;
    for (const auto& site : json["sites"])
        site_ids.push_back(site["id"]);
    Map map(site_ids);
    for (const auto& river : json["rivers"]) {
        const SiteId source = map.ToSiteId(river["source"]);
        const SiteId target = map.ToSiteId(river["target"]);
        map.rivers_.push_back({source, target});
    }
    for (const auto& mine : json["mines"])
        map.mines_.push_back(map.ToSiteId(mine));
    return map;
}

SiteId Map::ToSiteId(JsonSiteId json_id) const {
    const auto found = 
        std::lower_bound(site_ids_.begin(), site_ids_.end(), json_id);
    return found - site_ids_.begin();
}

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
