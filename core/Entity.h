#pragma once
#include <string>
struct Vec2 { int x=0, y=0; };
struct Stats { int str=5, dex=5, morale=50, hp=20; };
using EntityId = int;

struct Entity {
    EntityId id=0;
    std::string name="unknown";
    Vec2 pos{0,0};
    Stats stats{};
    bool isPlayer=false;
};
