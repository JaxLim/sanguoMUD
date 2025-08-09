#pragma once
#include <vector>
#include <string>
#include <optional>
#include "Entity.h"

class World {
public:
    bool LoadData(const std::string& folder);
    void TickHours(int h);
    std::string Save(const std::string& path) const;
    std::string Load(const std::string& path);

    EntityId playerId() const { return playerId_; }
    bool Move(EntityId id, Vec2 dxy);
    std::string Talk(EntityId a, EntityId b);
    std::string Attack(EntityId a, EntityId b);

    int width() const { return w_; }
    int height() const { return h_; }
    bool Walkable(Vec2 p) const;

    std::optional<EntityId> NearestNpc(EntityId from) const;
    const Entity* Find(EntityId id) const;
    Entity* Find(EntityId id);

private:
    int w_=0, h_=0;
    std::vector<std::vector<int>> blocks_; // 0 walkable, 1 wall
    std::vector<Entity> entities_;
    EntityId playerId_ = 1;
    int day_=1, hour_=6;
};
