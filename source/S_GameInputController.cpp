#include "stl.h"
#include "EcsSystems.h"
#include "Utils.h"
#include "Game.h"

static stl::queue<game::EventSfml> inputPool;

void GameInputControllerSys::OnEvent(const game::EventSfml &evt)
{
    inputPool.push(evt);
}   

static bool Move(CompPos *curPos, sf::Vector2i dxy)
{
    bool canMove = true;
    ecs::Apply<CompPos, TagCollidable>([&](ecs::IEntity *ent) {
        if (curPos->v + dxy == ent->GetComp<CompPos>()->v)
            canMove = false;
    });
    if (!canMove)
        return false;

    ecs::Apply<CompPosPattern, TagCollidable>([&](ecs::IEntity *ent) {
        if (ent->GetComp<CompPosPattern>()->
            positions.IsPosExists(curPos->v.x + dxy.x, curPos->v.y + dxy.y))
            canMove = false;
    });
    if (!canMove)
        return false;

    curPos->v += dxy;

    ecs::Apply<CompPos, TagCollectable>([&](ecs::IEntity *ent) {
        if (ent->GetComp<CompPos>()->v == curPos->v)
        {
            // TODO: Collect item.
            ecs::gEntityManager.DestroyEntity(ent->GetDesc());
        }
    });
}

static bool ChangeAttackField(CompPos *myPos, CompDir *dir, CompOwnedItems *ammo,
    CompCurrentAttackArea *area)
{
    if (ammo->curWeaponId < 0 || ammo->curWeaponId >= int(ammo->weapons.size()))
        return false;

    sf::Vector2i normDir(0, 0);
    if (abs(dir->v.x) > abs(dir->v.y))
        normDir.x = dir->v.x > 0 ? 1 : -1;
    else
        normDir.y = dir->v.y > 0 ? 1 : -1;

    ecs::IEntity *areaEnt = ecs::gEntityManager.GetEntityByDesc(area->ent);
    if (!areaEnt)
        return false;

    CompPosPattern *pp = areaEnt->GetComp<CompPosPattern>();

    const game::Weapon *weap = ammo->weapons[ammo->curWeaponId];

    int w = weap->areaSize.x;
    int h = weap->areaSize.y;

    if (normDir.y == 1)
    {
        pp->positions.Init(w, h, 0);
        area->shift = sf::Vector2i(- w / 2, 1 + weap->shift);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[i][j] = weap->pattern[i * h + j];
    }
    else if (normDir.y == -1)
    {
        pp->positions.Init(w, h, 0);
        area->shift = sf::Vector2i(- w / 2, - h - weap->shift);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[w - 1 - i][h - 1 - j] = weap->pattern[i * h + j];
    }
    else if (normDir.x == 1)
    {
        pp->positions.Init(h, w, 0);
        area->shift = sf::Vector2i(1 + weap->shift, - w / 2);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[j][w - 1 - i] = weap->pattern[i * h + j];
    }
    else
    {
        pp->positions.Init(h, w, 0);
        area->shift = sf::Vector2i(- h - weap->shift, - w / 2);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[h - 1 - j][i] = weap->pattern[i * h + j];
    }
    
    CompDamage *damage = areaEnt->GetComp<CompDamage>();
    damage->physDamage = weap->physDamage;
    damage->magicDamage = weap->magicDamage;

    return true;
}

static void ShowAttackField(CompVisibility *vis)
{
    vis->visible = true;
    vis->hideAfterOneTick = true;
}

static void Attack(CompActivity *act, CompOwnedItems *items, ecs::IEntity *player)
{
    float stamCons = items->weapons[items->curWeaponId]->staminaConsumption;
    float manaCons = items->weapons[items->curWeaponId]->manaConsumption;
    CompStamina *s = player->GetComp<CompStamina>();
    CompMana *m = player->GetComp<CompMana>();

    if (s->value - stamCons >= 0.0f && m->value - manaCons >= 0.0f)
    {
        s->value -= stamCons;
        m->value -= manaCons;
        act->active = true;
        act->disableAfterOneTick = true; // So the damage will be only in one step.
    }
}

void GameInputControllerSys::OnUpdate()
{
    auto players = ecs::gEntityManager.GetEntitiesByType(ecs::GetEntityTid<Player>());
    if (!players)
        return;
    IEntity *player = players->at(0).get();
    if (!player)
        return;

    // Yes, input is hardcoded.
    // You may change such realization, if you want.

    CompOwnedItems *items = player->GetComp<CompOwnedItems>();

    bool needAttackChange = false;
    bool needAttack = false;
    bool needShowAttack = false;

    sf::Vector2i moveDir(0, 0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moveDir += sf::Vector2i(0, 1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moveDir += sf::Vector2i(-1, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moveDir += sf::Vector2i(0, -1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moveDir += sf::Vector2i(1, 0);

    player->GetComp<CompMoveDir>()->v = moveDir;
    if (moveDir != sf::Vector2i(0, 0))
        needAttackChange = true;


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        IEntity *attackArea = ecs::gEntityManager.GetEntityByDesc(
            player->GetComp<CompCurrentAttackArea>()->ent);
        if (attackArea)
            ShowAttackField(attackArea->GetComp<CompVisibility>());
    }

    if (needAttackChange)
    {
        ChangeAttackField(player->GetComp<CompPos>(), player->GetComp<CompDir>(),
            items, player->GetComp<CompCurrentAttackArea>());
    }

    while (inputPool.size())
    {
        
        needAttackChange = false;
        needAttack = false;
        needShowAttack = false;

        game::EventSfml &evt = inputPool.front();
        
        switch (evt.type)
        {
        case game::EventSfml::KeyPressed:
            switch (evt.key.code)
            {
            case sf::Keyboard::Up:
                player->GetComp<CompDir>()->v = sf::Vector2i(0, 1);
                needAttackChange = true;
                needShowAttack = true;
                break;
            case sf::Keyboard::Down:
                player->GetComp<CompDir>()->v = sf::Vector2i(0, -1);
                needAttackChange = true;
                needShowAttack = true;
                break;
            case sf::Keyboard::Left:
                player->GetComp<CompDir>()->v = sf::Vector2i(-1, 0);
                needAttackChange = true;
                needShowAttack = true;
                break;
            case sf::Keyboard::Right:
                player->GetComp<CompDir>()->v = sf::Vector2i(1, 0);
                needAttackChange = true;
                needShowAttack = true;
                break;
            }
            if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
            {
                unsigned weapId = evt.key.code - sf::Keyboard::Num1;
                if (weapId < items->weapons.size())
                {
                    items->curWeaponId = weapId;
                    needAttackChange = true;
                }
            }
            break;
        case game::EventSfml::KeyReleased:
            
            switch (evt.key.code)
            {
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
                needAttack = true;
                break;
            case sf::Keyboard::Q:
                if(player->GetComp<CompOwnedItems>()->specialId > 0){
                    player->GetComp<CompOwnedItems>()->specialId--;
                }                 
                break;
            case sf::Keyboard::E:
                if(player->GetComp<CompOwnedItems>()->specialId < -1 + (int)player->GetComp<CompOwnedItems>()->weapons.size()){
                    player->GetComp<CompOwnedItems>()->specialId++;
                }                 
                break;
            case sf::Keyboard::G:
                ((Player*)player)->godmod = true;
            case sf::Keyboard::Space:
                game::gRenderer.GuiChangeSize("FirstScreen",{1,1});
                if(player->GetComp<CompOwnedItems>()->specialId != -1){
                    auto items = player->GetComp<CompOwnedItems>();
                    auto temp = items->weapons[items->specialId];
                    for (int i = items->specialId; i >= 0; i--) {
                        if(i != 0){
                            items->weapons[i] = items->weapons[i-1];
                        } else{
                            items->weapons[i] = temp;
                        }
                    }
                }                 
                break;
            break;
            }
        }

        game::Weapon *weapon = (game::Weapon *) player->GetComp<CompOwnedItems>()->weapons[player->GetComp<CompOwnedItems>()->curWeaponId];
            
        //Расходники
        if (weapon->is_usable && needAttack) {
            if (weapon->actionId == 1) {
                sf::Vector2i pos = player->GetComp<CompPos>()->v;
                auto desc = ecs::gEntityManager.CreateEntity<AttackArea>();
                auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
                game::PosPattern pattern;
                pattern.Init(9, 9, true);
                pattern.pos = pos - sf::Vector2i(pattern.size.x / 2, pattern.size.y / 2);
                pattern.tileId = game::TilesLib["shots"]["FreezeArea"];
                ent->GetComp<CompPosPattern>()->positions = pattern;
                ent->GetComp<CompActivity>()->active = true;
                ent->GetComp<CompDamage>()->physDamage = 0;
                ent->GetComp<CompDamage>()->magicDamage = 0;
                ent->GetComp<CompVisibility>()->visible = true;
                ent->GetComp<CompTimeToLive>()->ticks = 2;
                ent->GetComp<CompTimeToLive>()->isActive = true;
                ent->GetComp<CompDrawTileInfo>()->layer = 2;
                ent->GetComp<CompDrawTileInfo>()->tileId = pattern.tileId; 
                ent->GetComp<CompDamage>()->effector = 2;
                ent->GetComp<CompOwner>()->owner = player->GetDesc();
            } else if (weapon->actionId == 2) {
                player->GetComp<CompHealth>()->value = stl::min((int)player->GetComp<CompHealth>()->value + 50, 100);
            } else if (weapon->actionId == 3) {
                player->GetComp<CompHealth>()->value = 100;
                player->GetComp<CompStamina>()->value = 100;
            }
            //auto temp = player->GetComp<CompOwnedItems>()->weapons[player->GetComp<CompOwnedItems>()->weapons.size() - 1];
            //player->GetComp<CompOwnedItems>()->weapons[player->GetComp<CompOwnedItems>()->weapons.size() - 1] = player->GetComp<CompOwnedItems>()->weapons[0];
            //player->GetComp<CompOwnedItems>()->weapons[0] = temp;
            //player->GetComp<CompOwnedItems>()->weapons.
            constexpr int bufSize = 16;
            char buf[bufSize] = {};
            std::snprintf(buf, bufSize, "itm%d", player->GetComp<CompOwnedItems>()->weapons.size() - 1);
            game::gRenderer.GuiChangeTile(buf, 0);
            player->GetComp<CompOwnedItems>()->weapons.erase(player->GetComp<CompOwnedItems>()->weapons.begin() + player->GetComp<CompOwnedItems>()->curWeaponId);
            //player->GetComp<CompOwnedItems>()->weapons.pop_back();
            needAttack = false;
            needShowAttack = false;
            
        }

        if (needAttackChange)
        {
            ChangeAttackField(player->GetComp<CompPos>(), player->GetComp<CompDir>(),
                items, player->GetComp<CompCurrentAttackArea>());
        }
        if (needAttack || needShowAttack)
        {            
            //СТРЕЛКОВОЕ ОРУЖИЕ
            if (weapon->shootable && needAttack) {
                //game::floating_nums::nums.push_back(game::floating_nums());
                //game::floating_nums::set_floating_nums(sf::Vector2i (24, 24), "HELLO", 30, sf::Color(0xFFFFFFFF), 0.0,4.0 );
                //ecs::Apply<TagBoss>([&](ecs::IEntity *creature){((Boss*)creature)->Activate();});

                float stamCons = weapon->staminaConsumption;
                float manaCons = weapon->manaConsumption;
                CompStamina *s = player->GetComp<CompStamina>();
                CompMana *m = player->GetComp<CompMana>();

                if (s->value - stamCons >= 0.0f && m->value - manaCons >= 0.0f)
                {
                    auto desc = ecs::gEntityManager.CreateEntity<Shot>();
                    auto shot_ent = ecs::gEntityManager.GetEntityByDesc(desc);
                    game::Shot_st shot = game::gGameShots[weapon->shot_id];
                    shot_ent->GetComp<CompDamage>()->radius = shot.radius;
                    shot_ent->GetComp<CompMoveDir>()->v = player->GetComp<CompDir>()->v;
                    shot_ent->GetComp<CompOwner>()->owner = player->GetDesc();
                    shot_ent->GetComp<CompPos>()->v = player->GetComp<CompDir>()->v + player->GetComp<CompPos>()->v;
                    sf::Vector2i dir = player->GetComp<CompDir>()->v;
                    stl::string name_for_library = shot.name;
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
                    shot_ent->GetComp<CompDamage>()->effector = weapon->effector;
                    shot_ent->GetComp<CompDamage>()->magicDamage = shot.magicDamage;
                    shot_ent->GetComp<CompAnimation>()->fuckid = game::TilesLib["shots"][shot.name + "Mark"];
                    int t = game::TilesLib["shots"][name_for_library];
                }
            }
            //ОБЫЧНОЕ ОРУЖИЕ
            IEntity *attackArea = ecs::gEntityManager.GetEntityByDesc(
                player->GetComp<CompCurrentAttackArea>()->ent);
            if (attackArea)
            {
                ((AttackArea*)ecs::gEntityManager.GetEntityByDesc(player->GetComp<CompCurrentAttackArea>()->ent))->GetComp<CompDamage>()->effector = weapon->effector;
                if (needShowAttack)
                    ShowAttackField(attackArea->GetComp<CompVisibility>());
                if (needAttack)
                    Attack(attackArea->GetComp<CompActivity>(), items, player);
            }
        }
        inputPool.pop();
        //game::Calculate_Path(sf::Vector2i(3,3),player->GetComp<CompPos>()->v);
    }
    ecs::Apply<CompPos, TagCollectable>([&](ecs::IEntity *ent) {
        if (ent->GetComp<CompPos>()->v == player->GetComp<CompPos>()->v)
        {
            if (ent->GetComp<CompOwnedItems>()->weapons.size() > 0) {
                player->GetComp<CompOwnedItems>()->weapons.push_back(ent->GetComp<CompOwnedItems>()->weapons[0]);
            } else {
                player->GetComp<CompOwnedItems>()->potions.push_back(ent->GetComp<CompOwnedItems>()->potions[0]);
            }
            int textSize = 12;
            game::floating_nums::set_floating_nums(sf::Vector2i(player->GetComp<CompPos>()->v.x + 1 ,player->GetComp<CompPos>()->v.y), "New Item!", textSize, sf::Color(0xFFffffFF), 0.05, 20.0 );//damageText, textSize, sf::Color(0x111111ff), 1.0, 10.0);
                
            ecs::gEntityManager.DestroyEntity(ent->GetDesc());
        }
    });
}