#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Entity.h"

struct MapData {
        int w = 0, h = 0;
        int version = 1;
        std::vector<std::pair<int, int>> blocks;
        // 可选：带标签地形
        struct Tag {
                int x, y;                     // 位置
                std::string id;               // 唯一标识
                std::string name;             // 显示名称
                std::string desc;             // 描述
                bool walkable = true;         // 是否可通行
        };
        std::vector<Tag> tags;
};

using DialogMap = std::unordered_map<int, std::vector<std::string>>; // npcId -> lines

bool LoadMapJson(const std::string& path, MapData& out, std::string* err = nullptr);
bool LoadEntitiesJson(const std::string& path, std::vector<Entity>& out, std::string* err = nullptr);
bool LoadDialogsJson(const std::string& path, DialogMap& out, std::string* err = nullptr);
