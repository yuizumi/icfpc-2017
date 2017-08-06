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

void ParseMove(const Json& json, const Map& map,
               std::vector<Move>* moves, AI* ai) {
    for (const auto& e : json["moves"]) {
        const auto claim = e.find("claim");
        if (claim != e.end()) {
            const SiteId source = map.ToSiteId((*claim)["source"]);
            const SiteId target = map.ToSiteId((*claim)["target"]);
            const int punter = (*claim)["punter"];
            ai->HandleClaim(punter, {source, target});
            (*moves)[punter] = {Move::Action::kClaim, {source, target}};
            continue;
        }
        const auto splurge = e.find("splurge");
        if (splurge != e.end()) {
            std::vector<SiteId> route;
            for (Map::JsonSiteId json_id : (*splurge)["route"])
                route.push_back(map.ToSiteId(json_id));
            const int punter = (*splurge)["punter"];
            for (size_t i = 1; i < route.size(); i++) {
                ai->HandleClaim(punter, {route[i - 1], route[i]});
            }
            (*moves)[punter] = {Move::Action::kSplurge, {}};
            (*moves)[punter].route = std::move(route);
            continue;
        }
        (*moves)[e["pass"]["punter"]] = {Move::Action::kPass, {}};
    }
}

void DoSetup(AI* ai, Json&& json) {
    const int id = json["punter"];
    const Map map = Map::Parse(json["map"]);
    ai->Init(id, json["punters"], &map);
    ai->Setup();
    json["custom"] = ai->SaveState();
    Write({{"ready", id}, {"state", json}});
}

void DoGameplay(AI* ai, Json&& json) {
    Json state = json["state"];
    const int id = state["punter"];
    const int num_punters = state["punters"];
    const Map map = Map::Parse(state["map"]);
    ai->Init(id, num_punters, &map);
    ai->LoadState(std::move(state["custom"]));
    std::vector<Move> moves(num_punters);
    ParseMove(json["move"], map, &moves, ai);
    Move next_move = ai->Gameplay(moves);
    state["custom"] = ai->SaveState();

    switch (next_move.action) {
        case Move::Action::kPass:
            Write({{"pass", {{"punter", id}}}, {"state", state}});
            break;
        case Move::Action::kClaim: {
            const auto source = map.ToJsonId(next_move.river.source);
            const auto target = map.ToJsonId(next_move.river.target);
            const Json claim = {
                {"punter", id}, {"source", source}, {"target", target}};
            Write({{"claim", claim}, {"state", state}});
            break;
        }
        case Move::Action::kSplurge: {
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
