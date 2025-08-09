#include "Command.h"
#include "World.h"

static std::string dirText(const std::string& t){
    if(t=="north") return "北";
    if(t=="south") return "南";
    if(t=="west") return "西";
    if(t=="east") return "东";
    return t;
}

std::string Execute(World& world, int actor, const Command& cmd){
    if(cmd.verb=="go"){
        auto d = cmd.target;
        if(d=="north" && world.Move(actor,{0,-1})) return "你向北移动一格。";
        if(d=="south" && world.Move(actor,{0, 1})) return "你向南移动一格。";
        if(d=="west"  && world.Move(actor,{-1,0})) return "你向西移动一格。";
        if(d=="east"  && world.Move(actor,{ 1,0})) return "你向东移动一格。";
        return "被阻挡，没走动。";
    }
    if(cmd.verb=="talk"){
        auto npc = world.NearestNpc(actor);
        if(!npc) return "附近没人搭理你。";
        return world.Talk(actor, *npc);
    }
    if(cmd.verb=="attack"){
        auto npc = world.NearestNpc(actor);
        if(!npc) return "附近没敌人。";
        return world.Attack(actor, *npc);
    }
    return "你喃喃自语，但什么也没发生。";
}
