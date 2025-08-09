#include "World.h"
#include "Data.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include "json.hpp"
using json = nlohmann::json;

static int clamp(int v,int lo,int hi){ return std::max(lo,std::min(hi,v)); }
static int hitChance(const Stats& a,const Stats& d){ return clamp(60 + (a.dex - d.dex)*2, 10, 95); }
static int roll(){ static std::mt19937 rng{std::random_device{}()}; return std::uniform_int_distribution<int>(0,99)(rng); }
static int damage(const Stats& a){ static std::mt19937 rng{std::random_device{}()}; return std::uniform_int_distribution<int>(std::max(1,a.str-2), a.str+2)(rng); }

bool World::LoadData(const std::string& folder) {
    std::string err;

    MapData m;
    if (!LoadMapJson(folder + "/map.json", m, &err)) { /* 你也可以把 err 打到 UI */ return false; }
    w_ = m.w; h_ = m.h;
    blocks_.assign(h_, std::vector<int>(w_, 0));
    for (auto [x, y] : m.blocks) if (y >= 0 && y < h_ && x >= 0 && x < w_) blocks_[y][x] = 1;
    // 需要的话把 m.tags 保存下来以便地形修正

    std::vector<Entity> loaded;
    if (!LoadEntitiesJson(folder + "/entities.json", loaded, &err)) { return false; }
    entities_ = std::move(loaded);
    for (auto& e : entities_) if (e.isPlayer) playerId_ = e.id;

    DialogMap dlg;
    LoadDialogsJson(folder + "/dialogs.json", dlg, &err); // 读不到也不致命
    // 先偷懒：对话直接用固定文本，你以后可以把 dlg 存起来按 id 查

    return true;
}


void World::TickHours(int h){
    hour_ += h;
    while(hour_>=24){ hour_-=24; day_++; }
    // 先打个证明活着
    // 真正的事件系统以后再加
}

bool World::Walkable(Vec2 p) const {
    if(p.x<0||p.y<0||p.x>=w_||p.y>=h_) return false;
    return blocks_[p.y][p.x]==0;
}

Entity* World::Find(EntityId id){
    for(auto& e: entities_) if(e.id==id) return &e;
    return nullptr;
}
const Entity* World::Find(EntityId id) const{
    for(auto& e: entities_) if(e.id==id) return &e;
    return nullptr;
}

bool World::Move(EntityId id, Vec2 d){
    auto* e = Find(id); if(!e) return false;
    Vec2 np{e->pos.x + d.x, e->pos.y + d.y};
    if(!Walkable(np)) return false;
    e->pos = np; return true;
}

std::optional<EntityId> World::NearestNpc(EntityId from) const{
    const Entity* me = Find(from); if(!me) return std::nullopt;
    int best=1e9; std::optional<EntityId> ans;
    for(auto& e: entities_){
        if(e.id==from) continue;
        int d = std::abs(e.pos.x - me->pos.x) + std::abs(e.pos.y - me->pos.y);
        if(d<best){ best=d; ans=e.id; }
    }
    return ans;
}

std::string World::Talk(EntityId a, EntityId b){
    const Entity* me = Find(a), *other = Find(b);
    if(!me||!other) return "没人回应你。";
    return other->name + "：……好吧，北面有巡逻。";
}

std::string World::Attack(EntityId a, EntityId b){
    auto* A = Find(a); auto* B = Find(b);
    if(!A||!B) return "空气被你揍了一拳。";
    int hc = hitChance(A->stats, B->stats);
    if(roll() < hc){
        int dmg = damage(A->stats);
        B->stats.hp -= dmg;
        if(B->stats.hp<=0) return A->name+"击倒了"+B->name+"！";
        return A->name+"命中，造成"+std::to_string(dmg)+"点伤害。敌方HP："+std::to_string(B->stats.hp);
    }else{
        return A->name+"挥空了。";
    }
}

std::string World::Save(const std::string& path) const{
    std::ofstream f(path, std::ios::binary); if(!f) return "存档失败";
    // 极简、糙但能用：以后换成安全格式
    f.write((char*)&w_, sizeof(w_)); f.write((char*)&h_, sizeof(h_));
    int n = (int)entities_.size(); f.write((char*)&n,sizeof(n));
    for(auto& e: entities_) f.write((char*)&e,sizeof(Entity));
    f.write((char*)&day_, sizeof(day_)); f.write((char*)&hour_, sizeof(hour_));
    return "已存档。";
}

std::string World::Load(const std::string& path){
    std::ifstream f(path, std::ios::binary); if(!f) return "读档失败";
    f.read((char*)&w_, sizeof(w_)); f.read((char*)&h_, sizeof(h_));
    int n=0; f.read((char*)&n,sizeof(n)); entities_.resize(n);
    for(int i=0;i<n;i++) f.read((char*)&entities_[i], sizeof(Entity));
    f.read((char*)&day_, sizeof(day_)); f.read((char*)&hour_, sizeof(hour_));
    for(auto& e: entities_) if(e.isPlayer) playerId_=e.id;
    blocks_.assign(h_, std::vector<int>(w_,0)); // 简化：读档不还原障碍
    return "已读档。";
}
