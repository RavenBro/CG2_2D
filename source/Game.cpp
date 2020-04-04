#include "Game.h"
#include "stl.h"
#include "GameStructures.h"
#include "GameDataLoad.h"
#include "ECS.h"
#include "Timer.h"
#include "EcsSystems.h"
#include <limits.h>

#include "Renderer.h"
#include "GameLevelData.h"

#define UPDATE_PERIOD 0.2f

namespace game
{

LevelData gLevelData;
std::map<std::string, std::map<std::string, int>> TilesLib;
std::vector<game::Weapon> gGameWeapons;
std::vector<std::vector<game::edge>> gGameGraph;
sf::Vector2i gGameGraphSize;
std::vector<game::Shot_st> gGameShots;
float update_period = UPDATE_PERIOD;
std::vector<ecs::IEntity *> blocks;

void InitiateTiles(){
    //Моя библиотека тайлов
    std::map<std::string, int> shots;
    shots.emplace("ArrowU", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_172.png")));
    shots.emplace("ArrowR", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_171.png")));
    shots.emplace("ArrowD", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_174.png")));
    shots.emplace("ArrowL", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_173.png")));
    shots.emplace("ArrowMark", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_16.png")));
    shots.emplace("BossArrowU", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_182.png")));
    shots.emplace("BossArrowR", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_181.png")));
    shots.emplace("BossArrowD", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_184.png")));
    shots.emplace("BossArrowL", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_183.png")));
    shots.emplace("BossArrowMark", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_25.png")));
    shots.emplace("BallU", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_42.png")));
    shots.emplace("BallR", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_41.png")));
    shots.emplace("BallD", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_44.png")));
    shots.emplace("BallL", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_43.png")));
    shots.emplace("BallMark", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_84.png")));
    shots.emplace("BoltU", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_162.png")));
    shots.emplace("BoltR", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_161.png")));
    shots.emplace("BoltD", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_164.png")));
    shots.emplace("BoltL", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_163.png")));
    shots.emplace("BoltMark", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_24x24/oryx_16bit_fantasy_fx2_27.png")));
    shots.emplace("BossAttack", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_19.png")));
    shots.emplace("Poison1", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_28.png")));
    shots.emplace("Poison2", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_41_2.png")));
    shots.emplace("Freeze", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_17.png")));
    shots.emplace("SmallHeal", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_03.png")));
    shots.emplace("BigHeal", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_15.png")));
    shots.emplace("FreezeArea", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_05.png")));
    TilesLib.emplace("shots", shots);
    
    std::map<std::string, int> weapons;
    weapons.emplace("Hammer", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_165.png")));
    weapons.emplace("Bow", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_192.png")));
    weapons.emplace("IceWand", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_54.png")));
    weapons.emplace("FireWand", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_153.png")));
    weapons.emplace("FreezeRing", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_241.png")));
    weapons.emplace("Dagger", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_208.png")));
    weapons.emplace("Hallebard", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_175.png")));
    TilesLib.emplace("weapons", weapons);
    
    std::map<std::string, int> Mobes;
    Mobes.emplace("Boss1", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/creatures_24x24/oryx_16bit_fantasy_creatures_294.png")));
    Mobes.emplace("Boss2", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/creatures_24x24/oryx_16bit_fantasy_creatures_312.png")));
    Mobes.emplace("Hero1", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/creatures_24x24/hero1.png")));
    Mobes.emplace("Hero2", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/creatures_24x24/hero.png")));
    Mobes.emplace("BossMark1", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_85.png")));
    Mobes.emplace("BossMark1", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/fx_32x32/oryx_16bit_fantasy_fx_85.png")));
    Mobes.emplace("Shooter", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_432.png")));
    Mobes.emplace("Grave", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_492.png")));
    TilesLib.emplace("Mobes", Mobes);
    
    std::map<std::string, int> OtherObjects;
    OtherObjects.emplace("EmptyPlace", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1955.png")));
    OtherObjects.emplace("Chest", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/chest.png")));
    OtherObjects.emplace("BloodRoute", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_153.png")));
    OtherObjects.emplace("SpecialChoice", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_choosed.png")));
    OtherObjects.emplace("Closed", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_258.png")));
    TilesLib.emplace("OtherObjects", OtherObjects);
    
    std::map<std::string, int> UIObjects;
    //UIObjects.emplace("Health_Full", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/creatures_24x24/Health_full.png")));
    UIObjects.emplace("EmptyBar", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_bar.png")));
    UIObjects.emplace("choosedItem", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_choosed.png")));
    UIObjects.emplace("specialItem", gRenderer.AddTile(gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_special.png")));
    TilesLib.emplace("UIObjects", UIObjects);
}


void InitiateWeapons() {
    

    

    //halebard 2
    sf::Vector2u Halebardarea = sf::Vector2u(5, 5);
    stl::vector<bool> HalebardPattern = {
        0, 0, 0, 0, 0,
         1, 1, 1, 0, 0,
         0, 1, 1, 1, 0,
         1, 1, 1, 0, 0,
         0, 0, 0, 0 ,0 
    };
    Weapon halebard;
    halebard.shift = -1;
    halebard.name = "Halebard";
    halebard.description.push_back(" Powerfull weapon of ");
    halebard.description.push_back("  medium distance");
    halebard.areaSize = Halebardarea;
    halebard.pattern = HalebardPattern;
    halebard.physDamage = 10.0f;
    halebard.magicDamage = 0.0f;
    halebard.staminaConsumption = 20.0f;
    halebard.manaConsumption = 0.0f;
    halebard.tileId = TilesLib["weapons"]["Hallebard"];
    halebard.effector = 0;
    gGameWeapons.push_back(halebard);

    //Hammer 1
    sf::Vector2u hammerarea = sf::Vector2u(5, 5);
    stl::vector<bool> hammerPattern = {
         0, 0, 0, 0, 0,
         0, 1, 1, 1, 0,
         1, 1, 1, 1, 0,
         0, 1, 1, 1, 0,
         0, 0, 0, 0 ,0 
    };
    Weapon hammer;
    hammer.name = "Hammer";
    hammer.description.push_back("  STAMP AND STOMP!");
    hammer.description.push_back("");
    hammer.areaSize = hammerarea;
    hammer.pattern = hammerPattern;
    hammer.physDamage = 30.0f;
    hammer.magicDamage = 0.0f;
    hammer.staminaConsumption = 60.0f;
    hammer.manaConsumption = 0.0f;
    hammer.tileId = TilesLib["weapons"]["Hammer"];
    hammer.effector = 2;
    gGameWeapons.push_back(hammer);

    //Poison 0
    sf::Vector2u Daggerarea = sf::Vector2u(3, 3);
    stl::vector<bool> DaggerPattern = {
        0, 0, 0,
        1, 1, 0,
        0, 0, 0
    };
    Weapon dagger;
    dagger.name = "Dagger";
    dagger.description.push_back("  Poisonous Blade");
    dagger.description.push_back("");
    dagger.areaSize = Daggerarea;
    dagger.pattern = DaggerPattern;
    dagger.physDamage = 2.0f;
    dagger.magicDamage = 0.0f;
    dagger.staminaConsumption = 5.0f;
    dagger.manaConsumption = 0.0f;
    dagger.tileId = TilesLib["weapons"]["Dagger"];
    dagger.effector = 1;
    gGameWeapons.push_back(dagger);

    //FreezeRing 3
    sf::Vector2u ringarea = sf::Vector2u(1, 1);
    stl::vector<bool> ringPattern = {0};
    Weapon ring;
    ring.name = "Ring";
    ring.description.push_back("  Freeze enemies around");
    ring.description.push_back("     (only once)   ");
    ring.areaSize = ringarea;
    ring.pattern = ringPattern;
    ring.tileId = TilesLib["weapons"]["FreezeRing"];
    ring.is_usable = true;
    ring.actionId = 1;
    gGameWeapons.push_back(ring);

    //SmallHeal 4
    Weapon sHeal;
    sHeal.name = "SmallHeal";
    sHeal.description.push_back(" Replenish 50 Hp");
    sHeal.description.push_back("   (only once)   ");
    sHeal.areaSize = ringarea;
    sHeal.pattern = ringPattern;
    sHeal.tileId = TilesLib["shots"]["SmallHeal"];
    sHeal.is_usable = true;
    sHeal.actionId = 2;
    gGameWeapons.push_back(sHeal);

    //BigHeal 5
    Weapon bHeal;
    bHeal.name = "BigHeal";
    bHeal.description.push_back(" Replenish Hep and Stamina");
    bHeal.description.push_back("       (only once)   ");
    bHeal.areaSize = ringarea;
    bHeal.pattern = ringPattern;
    bHeal.tileId = TilesLib["shots"]["BigHeal"];
    bHeal.is_usable = true;
    bHeal.actionId = 3;
    gGameWeapons.push_back(bHeal);
    
    //IceWand 6
    Weapon wand;
    wand.name = "IceWand";
    wand.description.push_back("  King's of the Night");
    wand.description.push_back("   legendary staff");
    wand.manaConsumption = 30.0f;
    wand.effector = 2;
    wand.tileId = TilesLib["weapons"]["IceWand"];
    wand.shootable = true;
    wand.shot_name = "Arrow";
    for(int i = 0; i < gGameShots.size(); i++) {
        if (gGameShots[i].name == wand.shot_name) {
            wand.shot_id = i;
        }
    }
    if (wand.shot_id == gGameShots.size()) {
        abort();
    }
    gGameWeapons.push_back(wand);

    //Bow 7
    Weapon bow;
    bow.name = "Bow";
    bow.description.push_back("  Doom Arrows brings");
    bow.description.push_back("    painfull death");
    bow.manaConsumption = 10.0f;
    bow.effector = 1;
    bow.tileId = TilesLib["weapons"]["Bow"];
    bow.shootable = true;
    bow.shot_name = "Bolt";
    for(int i = 0; i < gGameShots.size(); i++) {
        if (gGameShots[i].name == bow.shot_name) {
            bow.shot_id = i;
        }
    }
    if (bow.shot_id == gGameShots.size()) {
        abort();
    }
    gGameWeapons.push_back(bow);

    //FireStaff 8
    Weapon Fire;
    Fire.name = "FireWand";
    Fire.description.push_back("   KEEP AWAY FROM ");
    Fire.description.push_back("      CHILDREN!");
    Fire.manaConsumption = 50.0f;
    Fire.tileId = TilesLib["weapons"]["FireWand"];
    Fire.shootable = true;
    Fire.shot_name = "Ball";
    for(int i = 0; i < gGameShots.size(); i++) {
        if (gGameShots[i].name == Fire.shot_name) {
            Fire.shot_id = i;
        }
    }
    if (Fire.shot_id == gGameShots.size()) {
        abort();
    }
    gGameWeapons.push_back(Fire);
}

void InitiateShots() {
    //IceArrow
    Shot_st arrow;
    arrow.physDamage = 10.0f;
    arrow.name = "Arrow";
    arrow.duration = 8;
    arrow.tileIdList[0] = TilesLib["shots"]["ArrowU"];
    arrow.tileIdList[1] = TilesLib["shots"]["ArrowR"];
    arrow.tileIdList[2] = TilesLib["shots"]["ArrowD"];
    arrow.tileIdList[3] = TilesLib["shots"]["ArrowL"];
    arrow.tileIdList[4] = TilesLib["shots"]["ArrowMark"];
    arrow.radius = 1;
    arrow.effector = 2;
    gGameShots.push_back(arrow);
    //Bolt
    Shot_st Bolt;
    Bolt.physDamage = 5.0f;
    Bolt.name = "Bolt";
    Bolt.duration = 8;
    Bolt.tileIdList[0] = TilesLib["shots"]["BoltU"];
    Bolt.tileIdList[1] = TilesLib["shots"]["BoltR"];
    Bolt.tileIdList[2] = TilesLib["shots"]["BoltD"];
    Bolt.tileIdList[3] = TilesLib["shots"]["BoltL"];
    Bolt.tileIdList[4] = TilesLib["shots"]["BoltMark"];
    Bolt.radius = 1;
    Bolt.effector = 1;
    gGameShots.push_back(Bolt);
    //BossArrow
    Shot_st Barrow;
    Barrow.physDamage = 15.0f;
    Barrow.name = "BossArrow";
    Barrow.duration = 28;
    Barrow.radius = 1;
    Barrow.tileIdList[0] = TilesLib["shots"]["BossArrowU"];
    Barrow.tileIdList[1] = TilesLib["shots"]["BossArrowR"];
    Barrow.tileIdList[2] = TilesLib["shots"]["BossArrowD"];
    Barrow.tileIdList[3] = TilesLib["shots"]["BossArrowL"];
    Barrow.tileIdList[4] = TilesLib["shots"]["BossArrowMark"];
    gGameShots.push_back(Barrow);
    //FireBall
    Shot_st Ball;
    Ball.physDamage = 60.0f;
    Ball.name = "Ball";
    Ball.duration = 28;
    Ball.radius = 5;
    Ball.tileIdList[0] = TilesLib["shots"]["BallU"];
    Ball.tileIdList[1] = TilesLib["shots"]["BallR"];
    Ball.tileIdList[2] = TilesLib["shots"]["BallD"];
    Ball.tileIdList[3] = TilesLib["shots"]["BallL"];
    Ball.tileIdList[4] = TilesLib["shots"]["BallMark"];
    gGameShots.push_back(Ball);
}

void Game::Init(const stl::shared_ptr<sf::RenderWindow> &window)
{
    this->window = window;
    gRenderer.Init(window);
    SendEvent(EventRendererWasInited());
    InitiateTiles();
    InitiateShots();
    InitiateWeapons();

    // auto d = ecs::gEntityManager.CreateEntity<Boss>();
    // auto ent = ecs::gEntityManager.GetEntityByDesc(d);
    
    
    auto d = ecs::gEntityManager.CreateEntity<SimpleBlock>();
    auto ent = ecs::gEntityManager.GetEntityByDesc(d);
    ent->GetComp<CompDrawTileInfo>()->layer = 1;
    ent->GetComp<CompDrawTileInfo>()->tileId = TilesLib["OtherObjects"]["Closed"];
    ent->GetComp<CompPos>()->v = sf::Vector2i(15,6);
    blocks.push_back(ent);

    d = ecs::gEntityManager.CreateEntity<SimpleBlock>();
    ent = ecs::gEntityManager.GetEntityByDesc(d);
    ent->GetComp<CompDrawTileInfo>()->layer = 1;
    ent->GetComp<CompDrawTileInfo>()->tileId = 0;
    ent->GetComp<CompPos>()->v = sf::Vector2i(0,0);
    blocks.push_back(ent);

    d = ecs::gEntityManager.CreateEntity<SimpleBlock>();
    ent = ecs::gEntityManager.GetEntityByDesc(d);
    ent->GetComp<CompDrawTileInfo>()->layer = 1;
    ent->GetComp<CompDrawTileInfo>()->tileId = 0;
    ent->GetComp<CompPos>()->v = sf::Vector2i(0,0);
    blocks.push_back(ent);
    
    d = ecs::gEntityManager.CreateEntity<SpecialArea>();
    ent = ecs::gEntityManager.GetEntityByDesc(d);
    ((SpecialArea*)ent)->SpecialCreate(2);
    ent->GetComp<CompPos>()->v = sf::Vector2i(8,16);

    d = ecs::gEntityManager.CreateEntity<SpecialArea>();
    ent = ecs::gEntityManager.GetEntityByDesc(d);
    ((SpecialArea*)ent)->SpecialCreate(3);
    ent->GetComp<CompPos>()->v = sf::Vector2i(8,27);
    
    std::vector<sf::Vector2i> wp = {{16,6}, {35,36}, {17,21}, {5,2}, {20,36}, {20,9}, {8,20}, {35,15} };
    for (int i = 0; i < wp.size(); i++)
    {
        d = ecs::gEntityManager.CreateEntity<Collectable>();
        ent = ecs::gEntityManager.GetEntityByDesc(d);
        ent->GetComp<CompPos>()->v = wp[i];
        ent->GetComp<CompOwnedItems>()->weapons.push_back(&gGameWeapons[i + 1]);
        ent->GetComp<CompDrawTileInfo>()->tileId = TilesLib["OtherObjects"]["Chest"];
        ent->GetComp<CompDrawTileInfo>()->layer = 1;
    }

    d = ecs::gEntityManager.CreateEntity<Boss>();
    ent = ecs::gEntityManager.GetEntityByDesc(d);
    

}

void Game::Release()
{
    //window.reset();
    gRenderer.Release();
    gGameWeapons.clear();
    ecs::gEntityManager.DestroyAllEntities();
    
}


GameResults Game::StartLevel(const char *levelFilename)
{
    gLevelData = LoadLevel(levelFilename);
    
//Создание графа для Дийкстры
    ecs::Apply<TagIsWalls>([&](ecs::IEntity *ent) {
        auto maper = ent->GetComp<CompPosPattern>()->positions.pattern;
        auto size = ent->GetComp<CompPosPattern>()->positions.size;
        gGameGraphSize.x = size.x;
        gGameGraphSize.y = size.y;
        for (int i = 0; i < size.x; i++)
        {
            for (int j = 0; j < size.y; j++)
            {
                gGameGraph.push_back(std::vector<edge>());
                if(maper[i][j] == 0){
                    if(maper[i-1][j] == 0) {
                        int vecInd = (i - 1) * size.x + j;
                        gGameGraph[gGameGraph.size() - 1].push_back(edge {vecInd, 1});
                        gGameGraph[vecInd].push_back(edge {gGameGraph.size() - 1, 1});
                    }
                    if(maper[i][j-1] == 0) {
                        int vecInd = (i) * size.x + j - 1;
                        gGameGraph[gGameGraph.size() - 1].push_back(edge {vecInd, 1});
                        gGameGraph[vecInd].push_back(edge {gGameGraph.size() - 1, 1});
                    }
                }
            }
            
        }
        /*for (int i = 0; i < 1000; i++)
        {
            for (int j = 0; j < gGameGraph[i].size(); j++)
            {
                printf("%d   %d %d \n", i, gGameGraph[i][j].to,gGameGraph[i][j].length);
            }
            
            
        }*/
    });

//

    ecs::Apply<CompOwnedItems, CompCurrentAttackArea>([&](ecs::IEntity *ent) {
        CompOwnedItems *ammo = ent->GetComp<CompOwnedItems>();
        //ammo->weapons.push_back(&gGameWeapons[0]);
        /*for (int i = 0; i < gGameWeapons.size(); i++)
        {
            ammo->weapons.push_back(&gGameWeapons[i]);
        }*/
        ammo->weapons.push_back(&gGameWeapons[0]);
        ammo->curWeaponId = 0;
        CompCurrentAttackArea *area = ent->GetComp<CompCurrentAttackArea>();
    });

    gTime.Reset();
    volatile float lastTime = gTime.GetTime();
    while (window->isOpen())
    {
        gRenderer.BeginDraw(sf::Color(0x000000FFU));
        ecs::gEntityManager.UpdateEntities();
        gRenderer.EndDraw();
        while (lastTime + UPDATE_PERIOD > gTime.GetTime())
            ;
        lastTime = gTime.GetTime();
        // Sleep(200);
    }
    return GameResults();


}


//Дийкстра
int dijkstra(const std::vector<std::vector<edge> > &graph, std::vector<int> &predcessor, int source, int target) {
    std::vector<int> min_distance( graph.size(), INT_MAX );
    min_distance[ source ] = 0;
    std::set<std::pair<int,int> > active_vertices;
    active_vertices.insert( {0,source} );
    predcessor[source] = -1;
    while (!active_vertices.empty()) {
        int where = active_vertices.begin()->second;
        if (where == target) return min_distance[where];
        active_vertices.erase( active_vertices.begin() );
        for (auto ed : graph[where]) 
            if (min_distance[ed.to] > min_distance[where] + ed.length) {
                active_vertices.erase( { min_distance[ed.to], ed.to } );
                min_distance[ed.to] = min_distance[where] + ed.length;
                active_vertices.insert( { min_distance[ed.to], ed.to } );
                predcessor[ed.to] = where;
            }
    }
    return INT_MAX;
}

//Элементы навигации
std::vector<sf::Vector2i> Calculate_Path(sf::Vector2i from, sf::Vector2i dest) {
    int fromId = from.x * gGameGraphSize.x + from.y;
    int destId = dest.x * gGameGraphSize.x + dest.y;
    std::vector<int> predcessors;
    for (int i = 0; i < gGameGraph.size(); i++)
    {
        predcessors.push_back(0);
    }
    
    int leng = dijkstra(gGameGraph, predcessors, fromId, destId);
    int i = predcessors[destId];
    std::vector<sf::Vector2i> path;
    path.emplace(path.begin(),sf::Vector2i(destId/gGameGraphSize.x,destId%gGameGraphSize.x));
    int c = 0;
    while (i != -1) {
        c++;
        path.emplace(path.begin(),sf::Vector2i(i/gGameGraphSize.x,i%gGameGraphSize.x));
        i = predcessors[i];
        if (c > 5000) {
            path.clear();
            return path;
        }
    }
    return path;
}

}