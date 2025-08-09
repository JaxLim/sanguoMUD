#pragma once
#include <string>
#include <unordered_map>
struct Command {
    std::string verb;
    std::string target;
    std::unordered_map<std::string,std::string> args;
};
class World;
std::string Execute(World& world, int actor, const Command& cmd);
