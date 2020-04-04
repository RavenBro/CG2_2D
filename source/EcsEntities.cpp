#include "EcsEntities.h"

#include <cstdlib>
#include "Renderer.h"
#include "Game.h"
constexpr unsigned enemyUpdatePeriod = 2;


void Player::OnCreated()
{
    GetComp<CompParty>()->party = CompParty::ALLIANCE;

    int attackAreaTileId = game::gRenderer.AddTile(sf::Color(255, 0, 0, 128));
    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *ent = ecs::gEntityManager.GetEntityByDesc(eid);
    ent->GetComp<CompDrawTileInfo>()->tileId = attackAreaTileId;
    ent->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_EFFECTS;
    ent->GetComp<CompVisibility>()->visible = false;
    ent->GetComp<CompOwner>()->owner = this->GetDesc();
    GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["Mobes"]["Hero1"];
    GetComp<CompCurrentAttackArea>()->ent = eid;
    GetComp<CompAnimator>()->active = true;
    GetComp<CompAnimator>()->IDs[1] = game::TilesLib["Mobes"]["Hero1"];
    GetComp<CompAnimator>()->IDs[0] = game::TilesLib["Mobes"]["Hero2"];
}

void Player::OnDestroy()
{
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void Player::OnUpdate()
{
    CompHealth *h = GetComp<CompHealth>();
    CompStamina *s = GetComp<CompStamina>();
    CompMana *m = GetComp<CompMana>();
    if(GetComp<CompAnimator>()->active) {
        if (h->value < 100.0f)
            h->value = std::min(100.0f, h->value + 0.5f);
        if (s->value < 100.0f)
            s->value = std::min(100.0f, s->value + 0.5f);
        if (m->value < 100.0f)
            m->value = std::min(100.0f, m->value + 0.5f);
    }
    if(godmod){
        h->value = 100;
        s->value = 100;
        m->value = 100;
    }
}

void Enemy::OnCreated()
{
    GetComp<CompParty>()->party = CompParty::ENEMIES;

    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);

    area->GetComp<CompVisibility>()->visible = false;
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = false;
    area->GetComp<CompActivity>()->disableAfterOneTick = true;
    
    // 3x3 attack area.
    area->GetComp<CompPosPattern>()->positions.Init(3, 3, 1);

    GetComp<CompCurrentAttackArea>()->ent = eid;
    GetComp<CompCurrentAttackArea>()->shift = sf::Vector2i(-1, -1);
}

void Enemy::OnDestroy()
{
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void Enemy::OnUpdate()
{
    ++curTick;
    
    IEntity *area = ecs::gEntityManager.GetEntityByDesc(GetComp<CompCurrentAttackArea>()->ent);
    CompDamage *myDamage = GetComp<CompDamage>();
    area->GetComp<CompDamage>()->physDamage = myDamage->physDamage;
    area->GetComp<CompDamage>()->magicDamage = myDamage->magicDamage;
    if (!(curTick % enemyUpdatePeriod))
    {
        area->GetComp<CompActivity>()->active = true;
    }
    else
    {
        GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
        return;
    }

    unsigned vr = GetComp<CompVisionRadius>()->value;

    sf::Vector2i dir(0, 0);
    sf::Vector2i pos = GetComp<CompPos>()->v;

    // Now enemies are just hunt for the player.
    // Now there is very stupid algorithm:
    // just go in the player direction.
    ecs::Apply<TagPlayer>([&](IEntity *player)
    {
        CompPos *playerPos = player->GetComp<CompPos>();
        
        int dx = abs(playerPos->v.x - pos.x);
        int dy = abs(playerPos->v.y - pos.y);
        if (std::max(dx, dy) > GetComp<CompVisionRadius>()->value)
            return;

        if (playerPos->v.x < pos.x)
            dir.x = -1;
        else if (playerPos->v.x > pos.x)
            dir.x = 1;
        if (playerPos->v.y < pos.y)
            dir.y = -1;
        else if (playerPos->v.y > pos.y)
            dir.y = 1;
    });

    GetComp<CompMoveDir>()->v = dir;
}
void Shot::OnDestroy() {
    sf::Vector2i pos = this->GetComp<CompPos>()->v;
    pos += this->GetComp<CompMoveDir>()->v;
    auto desc = ecs::gEntityManager.CreateEntity<AttackArea>();
    auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
    game::PosPattern pattern;
    pattern.Init(this->GetComp<CompDamage>()->radius, this->GetComp<CompDamage>()->radius, true);
    pattern.pos = pos - sf::Vector2i(pattern.size.x / 2, pattern.size.y / 2);
    pattern.tileId = this->GetComp<CompAnimation>()->fuckid;
    ent->GetComp<CompOwner>()->owner = GetComp<CompOwner>()->owner;
    ent->GetComp<CompPosPattern>()->positions = pattern;
    ent->GetComp<CompActivity>()->active = true;
    ent->GetComp<CompDamage>()->physDamage = this->GetComp<CompDamage>()->physDamage;
    ent->GetComp<CompDamage>()->magicDamage = this->GetComp<CompDamage>()->magicDamage;
    ent->GetComp<CompVisibility>()->visible = true;
    ent->GetComp<CompTimeToLive>()->ticks = 2;
    ent->GetComp<CompTimeToLive>()->isActive = true;
    ent->GetComp<CompDrawTileInfo>()->layer = 2;
    ent->GetComp<CompDrawTileInfo>()->tileId = pattern.tileId; 
    ent->GetComp<CompDamage>()->effector = GetComp<CompDamage>()->effector;
    auto players = ecs::gEntityManager.GetEntitiesByType(ecs::GetEntityTid<Player>());
    if (!players)
        return;
    IEntity *player = players->at(0).get();
    ent->GetComp<CompOwner>()->owner = GetComp<CompOwner>()->owner;


}
void Shot::OnCreated() {
    GetComp<CompDrawTileInfo>()->layer = 1;
}

void Collectable::OnUpdate() {
    /*auto route = GetComp<CompRoute>();
    auto players = ecs::gEntityManager.GetEntitiesByType(ecs::GetEntityTid<Player>());
    IEntity *player = players->at(0).get();
    auto vecroute = game::Calculate_Path(GetComp<CompPos>()->v, player->GetComp<CompPos>()->v);
    for(int i = 0; i < vecroute.size(); i++){
        
        if (i < route->entitieRoute.size()) {
            route->entitieRoute[i]->GetComp<CompPos>()->v = vecroute[i];
            route->entitieRoute[i]->GetComp<CompDrawTileInfo>()->tileId = route->tileId;
        } else {
            auto desc = ecs::gEntityManager.CreateEntity<SpecialArea>();
            auto area_ent = ecs::gEntityManager.GetEntityByDesc(desc);
            route->entitieRoute.push_back(area_ent);
            area_ent->GetComp<CompPos>()->v = vecroute[i];
            area_ent->GetComp<CompDrawTileInfo>()->layer = 1;
            area_ent->GetComp<CompDrawTileInfo>()->tileId = route->tileId;
        }
    }
    for (int i = vecroute.size(); i < route->entitieRoute.size(); i++)
    {
        route->entitieRoute[i]->GetComp<CompDrawTileInfo>()->tileId = 0;
        route->entitieRoute[i]->GetComp<CompPos>()->v = sf::Vector2i(0,0);
    }*/
    
}

void Boss::OnCreated() {
    auto route = GetComp<CompRoute>();
    route->tileId = game::TilesLib["OtherObjects"]["BloodRoute"];
    GetComp<CompPos>()->v = sf::Vector2i(100,100);
    GetComp<CompArmor>()->physResist = 1.0f;
    GetComp<CompArmor>()->magicResist = 1.0f;
    GetComp<CompHealth>()->value = 1000;
    GetComp<CompDrawTileInfo>()->layer = 1;
    GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["Mobes"]["Boss1"];

    GetComp<CompParty>()->party = CompParty::ENEMIES;

    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);

    area->GetComp<CompVisibility>()->visible = true;
    area->GetComp<CompDrawTileInfo>()->layer = 1;
    area->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["shots"]["BossAttack"];
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = false;
    area->GetComp<CompActivity>()->disableAfterOneTick = true;
    
    // 3x3 attack area.
    area->GetComp<CompPosPattern>()->positions.Init(3, 3, 1);
    //area->GetComp<CompPosPattern>()->positions.pattern[1][1] = 0;
    area->GetComp<CompDamage>()->physDamage = 1000;

    GetComp<CompCurrentAttackArea>()->ent = eid;
    GetComp<CompCurrentAttackArea>()->shift = sf::Vector2i(-1, -1);
    GetComp<CompDamage>()->magicDamage = 0;
    GetComp<CompDamage>()->physDamage = 1000;
    //GetComp<CompCurrentAttackArea> =
    GetComp<CompParty>()->party = GetComp<CompParty>()->ENEMIES;
    //Cоздание стрелков`
    std::vector<sf::Vector2i> tops;
    std::vector<sf::Vector2i> lefts;
    std::vector<sf::Vector2i> rights;
    std::vector<sf::Vector2i> downs;
    for (int i = 2; i < 18; i++){
        tops.push_back(sf::Vector2i(i,38));
    }
    for (int i = 23; i < 38; i++){
        rights.push_back(sf::Vector2i(1,i));
    }
    for (int i = 23; i < 36; i++){
        lefts.push_back(sf::Vector2i(15,i));
    }
    downs.push_back(sf::Vector2i(2,22));
    downs.push_back(sf::Vector2i(3,22));
    downs.push_back(sf::Vector2i(4,22));
    downs.push_back(sf::Vector2i(5,27));
    downs.push_back(sf::Vector2i(6,27));
    downs.push_back(sf::Vector2i(7,27));
    downs.push_back(sf::Vector2i(8,27));
    downs.push_back(sf::Vector2i(9,27));
    downs.push_back(sf::Vector2i(10,27));
    downs.push_back(sf::Vector2i(11,27));
    downs.push_back(sf::Vector2i(12,22));
    downs.push_back(sf::Vector2i(13,22));
    downs.push_back(sf::Vector2i(14,22));

    for (int i = 0; i < tops.size(); i++)
    {
        auto desc = ecs::gEntityManager.CreateEntity<SchoolShooter>();
        auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
        ent->GetComp<CompDir>()->v = {0,-1};
        ent->GetComp<CompPos>()->v = tops[i];
    }
    for (int i = 0; i < downs.size(); i++)
    {
        auto desc = ecs::gEntityManager.CreateEntity<SchoolShooter>();
        auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
        ent->GetComp<CompDir>()->v = {0,1};
        ent->GetComp<CompPos>()->v = downs[i];
    }
    for (int i = 0; i < lefts.size(); i++)
    {
        auto desc = ecs::gEntityManager.CreateEntity<SchoolShooter>();
        auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
        ent->GetComp<CompDir>()->v = {-1,0};
        ent->GetComp<CompPos>()->v = lefts[i];
    }
    for (int i = 0; i < rights.size(); i++)
    {
        auto desc = ecs::gEntityManager.CreateEntity<SchoolShooter>();
        auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
        ent->GetComp<CompDir>()->v = {1,0};
        ent->GetComp<CompPos>()->v = rights[i];
    }
    
    //GetComp<CompAnimator>()->active = true;
    GetComp<CompAnimator>()->IDs[1] = game::TilesLib["Mobes"]["Boss1"];
    GetComp<CompAnimator>()->IDs[0] = game::TilesLib["Mobes"]["Boss2"];
    
    // auto desc = ecs::gEntityManager.CreateEntity<SchoolShooter>();
    // auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
    // ent->GetComp<CompDir>()->v = {-1,0};
    // ent->GetComp<CompPos>()->v = {15,3};
    // ((SchoolShooter*)ent)->Activate();
}

void Boss::OnUpdate() {
    if (!alive){return;}

    if(GetComp<CompPos>()->v == sf::Vector2i(16,37)){
        isFinal = true;
        GetComp<CompArmor>()->magicResist = 0.0;
        GetComp<CompArmor>()->physResist = 0.0;
    }
    //MOVEMENT
    auto route = GetComp<CompRoute>();
    auto players = ecs::gEntityManager.GetEntitiesByType(ecs::GetEntityTid<Player>());
    IEntity *player = players->at(0).get();
    auto vecroute = game::Calculate_Path(GetComp<CompPos>()->v, player->GetComp<CompPos>()->v);
    for(int i = 0; i < vecroute.size(); i++){
        if (i < route->entitieRoute.size()) {
            route->entitieRoute[i]->GetComp<CompPos>()->v = vecroute[i];
            route->entitieRoute[i]->GetComp<CompDrawTileInfo>()->tileId = route->tileId;
        } else {
            auto desc = ecs::gEntityManager.CreateEntity<SpecialArea>();
            auto area_ent = ecs::gEntityManager.GetEntityByDesc(desc);
            ((SpecialArea *)area_ent)->SpecialCreate(0);
            route->entitieRoute.push_back(area_ent);
            area_ent->GetComp<CompPos>()->v = vecroute[i];
            area_ent->GetComp<CompDrawTileInfo>()->layer = 0;
            area_ent->GetComp<CompDrawTileInfo>()->tileId = route->tileId;
        }
    }
    for (int i = vecroute.size(); i < route->entitieRoute.size(); i++)
    {
        route->entitieRoute[i]->GetComp<CompDrawTileInfo>()->tileId = 0;
        route->entitieRoute[i]->GetComp<CompPos>()->v = sf::Vector2i(0,0);
    }
    route->entitieRoute[0]->GetComp<CompDrawTileInfo>()->tileId = 0;
    //printf("%d\n",vecroute.size());
    if (!(curTick % (4*enemyUpdatePeriod)) || isFinal)
    {   
        if (vecroute.size() > 2) {
            sf::Vector2i nextPos = route->entitieRoute[1]->GetComp<CompPos>()->v;
            GetComp<CompMoveDir>()->v = nextPos - route->entitieRoute[0]->GetComp<CompPos>()->v;
            Move = GetComp<CompMoveDir>()->v;
            //MarksCreating
            if(!isFinal){
                auto desc = ecs::gEntityManager.CreateEntity<SpecialArea>();
                auto area_ent = ecs::gEntityManager.GetEntityByDesc(desc);
                ((SpecialArea*)area_ent)->SpecialCreate(1);     
            }
        }
        HaveMoved = false;
    } else if(HaveMoved) {
        Move = sf::Vector2i(0,0);
        GetComp<CompMoveDir>()->v = sf::Vector2i(0,0);
    }
    GetComp<CompMoveDir>()->v = Move;
      

    //CLASSICAL ATTACKS
    ++curTick;
    
    IEntity *area = ecs::gEntityManager.GetEntityByDesc(GetComp<CompCurrentAttackArea>()->ent);
    CompDamage *myDamage = GetComp<CompDamage>();
    area->GetComp<CompDamage>()->physDamage = myDamage->physDamage;
    area->GetComp<CompDamage>()->magicDamage = myDamage->magicDamage;
    if (isFinal)
    {
        finalCounter++;
        area->GetComp<CompDamage>()->physDamage = 20;
        area->GetComp<CompDamage>()->magicDamage = 0;
        if(finalCounter > 40) {
            finalCounter = 0;
            GetComp<CompEffect>()->type = 2;
            GetComp<CompEffect>()->active = true;
            GetComp<CompEffect>()->counter = GetComp<CompEffect>()->duration / 2;
            Activate();
        }
    }
    
    if (1)
    {
        area->GetComp<CompActivity>()->active = true;
    }
    else
    {
        GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
        return;
    }

}

void Boss::OnDestroy(){
    game::gRenderer.GuiChangeTexture("BossBar", 0);
    game::gRenderer.GuiChangeTexture("BosshBar", 0);
    game::gRenderer.GuiChangeTexture("BossFill", 0);
    GetComp<CompPos>()->v = {100,100};
    game::blocks[1]->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["OtherObjects"]["Closed"];
        game::blocks[1]->GetComp<CompPos>()->v = sf::Vector2i(100,100);
        game::blocks[2]->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["OtherObjects"]["Closed"];
        game::blocks[2]->GetComp<CompPos>()->v = sf::Vector2i(100,100);
}

void Boss::Activate() {
    int amount = 10;
    for(int i = 0; i < 20; i++){
        //shooters[]
        int x = rand()%shooters.size();
        shooters[x]->Activate();
    }
}

void SpecialArea::SpecialCreate(int sd) {
    GetComp<CompParty>()->party = CompParty::ENEMIES;
    GetComp<CompDrawTileInfo>()->layer = 0;
    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);

    area->GetComp<CompVisibility>()->visible = false;
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = true;
    area->GetComp<CompActivity>()->disableAfterOneTick = false;

    area->GetComp<CompPosPattern>()->positions.Init(1, 1, 0);

    GetComp<CompCurrentAttackArea>()->ent = eid;
    //seed = sd;
    //BossRoute
    if (sd == 1) {
        GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["Mobes"]["BossMark1"];
        seed = sd;
        ecs::Apply<TagBoss>([&](IEntity *ent) {
            GetComp<CompPos>()->v = ent->GetComp<CompPos>()->v;
        });
        ecs::gEntityManager.GetEntityByDesc(GetComp<CompCurrentAttackArea>()->ent)->GetComp<CompPosPattern>()->positions.Init(1,1,1);
        ecs::gEntityManager.GetEntityByDesc(GetComp<CompCurrentAttackArea>()->ent)->GetComp<CompDamage>()->physDamage = 1000;
    } else if (sd == 2) {
        //BossSpawn
        seed = sd;
    } else if (sd == 3) {
        seed = sd;
    }
}

void SpecialArea::OnUpdate() {
    ecs::IEntity *player;
    ecs::Apply<TagPlayer>([&](IEntity *ent) {
        player = ent;
    });
    if (seed == 2) {
        if (GetComp<CompPos>()->v == player->GetComp<CompPos>()->v) {
            ecs::Apply<TagBoss>([&](IEntity *ent) {
                ((Boss*)ent)->alive = true;
                ent->GetComp<CompAnimator>()->active = true;
                ent->GetComp<CompPos>()->v = sf::Vector2i(10,10);
            });
            game::blocks[0]->GetComp<CompDrawTileInfo>()->tileId = 0;
            game::blocks[0]->GetComp<CompPos>()->v = sf::Vector2i(0,0);
            GetComp<CompPos>()->v = sf::Vector2i(100,100);
            //seed = 0;
        }
    } else if (seed == 3) {
        if (GetComp<CompPos>()->v == player->GetComp<CompPos>()->v) {
            ecs::Apply<TagBoss>([&](IEntity *ent) {
                ent->GetComp<CompPos>()->v = sf::Vector2i(16,37);
                ecs::gEntityManager.GetEntityByDesc(ent->GetComp<CompCurrentAttackArea>()->ent)->GetComp<CompVisibility>()->visible = false;
            });
            game::blocks[1]->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["OtherObjects"]["Closed"];
            game::blocks[1]->GetComp<CompPos>()->v = sf::Vector2i(8,26);
            game::blocks[2]->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["OtherObjects"]["Closed"];
            game::blocks[2]->GetComp<CompPos>()->v = sf::Vector2i(16,35);
            GetComp<CompPos>()->v = sf::Vector2i(100,100);
        }
    }
}

void SchoolShooter::OnCreated() {
    GetComp<CompDrawTileInfo>()->layer = 2;
    GetComp<CompDrawTileInfo>()->tileId = 0;
    ecs::Apply<TagBoss>([&](IEntity *ent) {
        ((Boss*)ent)->shooters.push_back(this);
    });
}

void SchoolShooter::Activate() {
    active = true;
    GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["Mobes"]["Shooter"];
}

void SchoolShooter::OnUpdate() {
    timer++;
    if(active) {
        if (timer % maxTime == 0 && timer != 0) {
            active = false;
            timer = 0;
            GetComp<CompDrawTileInfo>()->tileId = 0;
            auto desc = ecs::gEntityManager.CreateEntity<Shot>();
            auto shot_ent = ecs::gEntityManager.GetEntityByDesc(desc);
            game::Shot_st shot;
            for(int i = 0; i < game::gGameShots.size(); i++) {
                if (game::gGameShots[i].name == "BossArrow") {
                    shot = game::gGameShots[i];
                }
            }
            shot_ent->GetComp<CompMoveDir>()->v = GetComp<CompDir>()->v;
            shot_ent->GetComp<CompPos>()->v = GetComp<CompPos>()->v;
            ecs::Apply<TagBoss>([&](IEntity *ent) {
                shot_ent->GetComp<CompOwner>()->owner = ent->GetDesc();
            });
            sf::Vector2i dir = GetComp<CompDir>()->v;
            stl::string name_for_library = "BossArrow";
            if (dir.x == 1) {
                name_for_library.append("R");
            } else if (dir.x == -1) {
                name_for_library.append("L");
            } else if (dir.y == 1) {
                name_for_library.append("U");
            } else if (dir.y == -1) {
                name_for_library.append("D");
            }
            shot_ent->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["shots"][name_for_library];
            shot_ent->GetComp<CompTimeToLive>()->ticks = shot.duration;
            shot_ent->GetComp<CompTimeToLive>()->isActive = true;
            shot_ent->GetComp<CompDamage>()->physDamage = shot.physDamage;
            shot_ent->GetComp<CompDamage>()->magicDamage = shot.magicDamage;
            shot_ent->GetComp<CompAnimation>()->fuckid = game::TilesLib["shots"][shot.name + "Mark"];
        }
    }
}