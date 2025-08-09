#include "Data.h"
#include <json.hpp>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

static bool read_json(const std::string& path, json& j, std::string* err) {
    std::ifstream f(path);
    if (!f) { if (err) *err = "无法打开文件: " + path; return false; }
    try { f >> j; }
    catch (const std::exception& e) { if (err) *err = std::string("JSON 解析失败: ") + e.what(); return false; }
    return true;
}

bool LoadMapJson(const std::string& path, MapData& out, std::string* err) {
    json j;
    if (!read_json(path, j, err)) return false;
    try {
        out.w = j.at("w").get<int>();
        out.h = j.at("h").get<int>();
        out.version = j.value("version", 1);
        out.blocks.clear();
        if (j.contains("blocks")) {
            for (const auto& b : j["blocks"]) {
                out.blocks.emplace_back(b.at(0).get<int>(), b.at(1).get<int>());
            }
        }
        out.tags.clear();
        if (j.contains("tags")) {
            for (const auto& t : j["tags"]) {
                MapData::Tag tg;
                tg.x = t.at("pos").at(0).get<int>();
                tg.y = t.at("pos").at(1).get<int>();
                tg.type = t.at("type").get<std::string>();
                out.tags.push_back(tg);
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        if (err) *err = std::string("map.json 字段缺失或类型不对: ") + e.what();
        return false;
    }
}

bool LoadEntitiesJson(const std::string& path, std::vector<Entity>& out, std::string* err) {
    json j;
    if (!read_json(path, j, err)) return false;
    try {
        out.clear();
        for (const auto& e : j) {
            Entity ent;
            ent.id = e.at("id").get<int>();
            ent.name = e.at("name").get<std::string>();
            ent.pos = { e.at("pos").at(0).get<int>(), e.at("pos").at(1).get<int>() };
            ent.stats.hp = e.value("hp", 20);
            ent.stats.str = e.value("str", 5);
            ent.stats.dex = e.value("dex", 5);
            ent.stats.morale = e.value("morale", 50);
            ent.isPlayer = e.value("player", false);
            out.push_back(ent);
        }
        return true;
    }
    catch (const std::exception& e) {
        if (err) *err = std::string("entities.json 字段缺失或类型不对: ") + e.what();
        return false;
    }
}

bool LoadDialogsJson(const std::string& path, DialogMap& out, std::string* err) {
    json j;
    if (!read_json(path, j, err)) return false;
    try {
        out.clear();
        if (j.is_object()) {
            // 支持两种写法：{"2":["a","b"]} 或 [{"id":2,"lines":[...]}]
            for (auto it = j.begin(); it != j.end(); ++it) {
                int id = std::stoi(it.key());
                std::vector<std::string> lines = it.value().get<std::vector<std::string>>();
                out[id] = std::move(lines);
            }
        }
        else if (j.is_array()) {
            for (const auto& item : j) {
                int id = item.at("id").get<int>();
                out[id] = item.at("lines").get<std::vector<std::string>>();
            }
        }
        return true;
    }
    catch (const std::exception& e) {
        if (err) *err = std::string("dialogs.json 解析失败: ") + e.what();
        return false;
    }
}

