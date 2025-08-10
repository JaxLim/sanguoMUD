#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include "Entity.h"
#include "Data.h"
#include "GameClock.h"

class World {
public:
    bool LoadData(const std::string& folder);
    std::string Save(const std::string& path) const;
    std::string Load(const std::string& path);

    EntityId playerId() const { return playerId_; }
    bool Move(EntityId id, Vec2 dxy);
    std::string Talk(EntityId a, EntityId b);
    std::string Attack(EntityId a, EntityId b);
    std::string Interact(EntityId a, EntityId b, const std::string& action);

    int width() const { return w_; }
    int height() const { return h_; }
    bool Walkable(Vec2 p) const;

    const std::vector<Entity>& entities() const { return entities_; }

    const Entity* Find(EntityId id) const;
    Entity* Find(EntityId id);

    std::string TagName(Vec2 p) const;                     // 地块名称
    std::string TagDesc(Vec2 p) const;                     // 地块描述

    bool EnsureSpawnPassable();                                // 出生点自动调整
    std::optional<std::pair<EntityId, int>> NearestReachableNpc(EntityId from, int maxSteps) const; // 可达最近NPC

    GameClock& clock() { return clock_; }
    const GameClock& clock() const { return clock_; }
private:
    int w_=0, h_=0;
    std::vector<std::vector<int>> blocks_; // 0 walkable, 1 wall
    std::vector<Entity> entities_;
    EntityId playerId_ = 1;
    GameClock clock_;
    std::vector<MapData::Tag> tags_;                       // 地块标签
    std::vector<std::vector<int>> FloodStepsFrom(Vec2 start) const; // BFS 距离场
    bool InBounds(Vec2 p) const { return p.x >= 0 && p.y >= 0 && p.x < w_ && p.y < h_; }
    std::string dataFolder_;
    std::string mapFile_;
    struct NpcInfo { std::unordered_map<std::string, std::string> interactions; };
    std::unordered_map<EntityId, NpcInfo> npcData_;
    std::string AttackCombat(EntityId a, EntityId b);
};
