#include "World.h"
#include "Data.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <queue>
#include <json.hpp>
using json = nlohmann::json;

static int clamp(int v,int lo,int hi){ return std::max(lo,std::min(hi,v)); }
static int hitChance(const Stats& a,const Stats& d){ return clamp(60 + (a.dex - d.dex)*2, 10, 95); }
static int roll(){ static std::mt19937 rng{std::random_device{}()}; return std::uniform_int_distribution<int>(0,99)(rng); }
static int damage(const Stats& a){ static std::mt19937 rng{std::random_device{}()}; return std::uniform_int_distribution<int>(std::max(1,a.str-2), a.str+2)(rng); }

bool World::LoadData(const std::string& folder) {
    dataFolder_ = folder;
    std::string err;

    MapData m;
    if (!LoadMapJson(folder + "/map.json", m, &err)) { /* 你也可以把 err 打到 UI */ return false; }
    w_ = m.w; h_ = m.h;
    blocks_.assign(h_, std::vector<int>(w_, 0));
    for (auto [x, y] : m.blocks) if (y >= 0 && y < h_ && x >= 0 && x < w_) blocks_[y][x] = 1;
    tags_ = m.tags;

    std::vector<Entity> loaded;
    if (!LoadEntitiesJson(folder + "/entities.json", loaded, &err)) { return false; }
    entities_ = std::move(loaded);
    for (auto& e : entities_) if (e.isPlayer) playerId_ = e.id;

    DialogMap dlg;
    LoadDialogsJson(folder + "/dialogs.json", dlg, &err); // 读不到也不致命
    // 先偷懒：对话直接用固定文本，你以后可以把 dlg 存起来按 id 查

    // 新增：启动时校验出生点
    EnsureSpawnPassable();
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

std::string World::TagName(Vec2 p) const {
    for (const auto& t : tags_) {
        if (t.x == p.x && t.y == p.y) {
            if (t.type == "forest") return "树林";
            if (t.type == "river") return "河边";
            if (t.type == "smith") return "铁匠铺";
            if (t.type == "shop") return "商铺";
            if (t.type == "gate") return "城门";
            if (t.type == "house") return "李府";
            return t.type;
        }
    }
    return "街道";
}

std::string World::Save(const std::string& path) const {
    json j;
    j["w"] = w_; j["h"] = h_;
    j["day"] = day_; j["hour"] = hour_;
    j["entities"] = json::array();
    for (const auto& e : entities_) {
        j["entities"].push_back({
            {"id", e.id},
            {"name", e.name},
            {"pos", {e.pos.x, e.pos.y}},
            {"hp", e.stats.hp},
            {"str", e.stats.str},
            {"dex", e.stats.dex},
            {"morale", e.stats.morale},
            {"player", e.isPlayer}
            });
    }
    std::ofstream f(path);
    if (!f) return "存档失败";
    f << j.dump(2);
    return "已存档。";
}


std::string World::Load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "读档失败";
    json j;
    try { f >> j; }
    catch (...) { return "读档损坏"; }

    w_ = j.value("w", w_);
    h_ = j.value("h", h_);
    day_ = j.value("day", day_);
    hour_ = j.value("hour", hour_);

    entities_.clear();
    for (const auto& je : j["entities"]) {
        Entity e;
        e.id = je.value("id", 0);
        e.name = je.value("name", "unknown");
        auto pos = je.value("pos", std::vector<int>{0, 0});
        if (pos.size() >= 2) { e.pos.x = pos[0]; e.pos.y = pos[1]; }
        e.stats.hp = je.value("hp", 20);
        e.stats.str = je.value("str", 5);
        e.stats.dex = je.value("dex", 5);
        e.stats.morale = je.value("morale", 50);
        e.isPlayer = je.value("player", false);
        entities_.push_back(std::move(e));
    }
    for (auto& e : entities_) if (e.isPlayer) playerId_ = e.id;

    // 读档后重建障碍
    blocks_.assign(h_, std::vector<int>(w_, 0));
    if (!dataFolder_.empty()) {
        std::string err2;
        MapData m2;
        if (LoadMapJson(dataFolder_ + "/map.json", m2, &err2)) {
            blocks_.assign(h_, std::vector<int>(w_, 0));
            for (auto [x, y] : m2.blocks) if (y >= 0 && y < h_ && x >= 0 && x < w_) blocks_[y][x] = 1;
        }
    }
    return "已读档。";
}

bool World::EnsureSpawnPassable() {
    // 找到玩家实体
    Entity* p = Find(playerId_);
    if (!p) return false;
    if (Walkable(p->pos)) return true;

    // 螺旋/环形搜最近可通行点
    const int R = 8; // 搜索半径
    for (int r = 1; r <= R; ++r) {
        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                if (std::abs(dx) != r && std::abs(dy) != r) continue; // 只检查“环”边界
                Vec2 np{ p->pos.x + dx, p->pos.y + dy };
                if (InBounds(np) && Walkable(np)) { p->pos = np; return true; }
            }
        }
    }
    // 实在找不到，就强行放到(0,0)如果能走的话
    if (InBounds({ 0,0 }) && Walkable({ 0,0 })) { p->pos = { 0,0 }; return true; }
    return false;
}

std::vector<std::vector<int>> World::FloodStepsFrom(Vec2 start) const {
    const int INF = 1e9;
    std::vector<std::vector<int>> dist(h_, std::vector<int>(w_, INF));
    if (!InBounds(start) || !Walkable(start)) return dist;

    std::queue<Vec2> q;
    dist[start.y][start.x] = 0;
    q.push(start);

    const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
    while (!q.empty()) {
        auto c = q.front(); q.pop();
        for (auto& d : dirs) {
            Vec2 n{ c.x + d[0], c.y + d[1] };
            if (InBounds(n) && Walkable(n) && dist[n.y][n.x] == INF) {
                dist[n.y][n.x] = dist[c.y][c.x] + 1;
                q.push(n);
            }
        }
    }
    return dist;
}

std::optional<std::pair<EntityId, int>> World::NearestReachableNpc(EntityId from, int maxSteps) const {
    const Entity* me = Find(from); if (!me) return std::nullopt;
    auto dist = FloodStepsFrom(me->pos);
    const int INF = 1e9;

    int bestSteps = INF;
    std::optional<std::pair<EntityId, int>> ans;

    for (const auto& e : entities_) {
        if (e.id == from) continue;
        if (!InBounds(e.pos)) continue;
        int steps = dist[e.pos.y][e.pos.x];
        if (steps < bestSteps && steps <= maxSteps) {
            bestSteps = steps;
            ans = std::make_pair(e.id, steps);
        }
    }
    return ans;
}
