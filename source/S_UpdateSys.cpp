#include "EcsSystems.h"
#include "stl.h"


//#include <iostream>
//#include <vector>
//using namespace std;


#include "ECS.h"
#include "EcsComponents.h"
#include "Game.h"


void UpdateSys::OnUpdate(){
    ecs::Apply<CompTimeToLive>([&](ecs::IEntity *liver)
    {
        CompTimeToLive *timer = liver->GetComp<CompTimeToLive>();
        if (timer->isActive == false) {
            return;
        }
        timer->ticks--;
        if (timer->ticks <= 0) {
            try
            {
                liver->OnDestroy();
            }
            catch(const std::exception& e)
            {}
            ecs::gEntityManager.DestroyEntity(liver->GetDesc());
        }
    });

    ecs::Apply<CompAnimator>([&](ecs::IEntity *liver)
    {
        auto anim = liver->GetComp<CompAnimator>();
        if (anim->active) {
            anim->counter++;
            if (anim->counter >= anim->ofency) {
                anim->counter = 0;
                anim->cur = 1 - anim->cur;
                liver->GetComp<CompDrawTileInfo>()->tileId = anim->IDs[anim->cur];
            }
        }
    });

    ecs::Apply<CompEffect>([&](ecs::IEntity *liver)
    {
        CompEffect *effect = liver->GetComp<CompEffect>();
        effect->tileId = 0;
        if (effect->type == 1) {
            effect->duration = 15;
        } else {
            effect->duration = 30;
        }
        if (effect->active == false) {
            return;
        }
        //Poison
        if (effect->type == 1) {
            liver->GetComp<CompMoveDir>()->CanMove = true;
            effect->tileId = game::TilesLib["shots"]["Poison2"];
            if ((effect->counter % 2) == 0 && effect->counter != 0) {
                sf::Vector2i pos = liver->GetComp<CompPos>()->v;
                auto desc = ecs::gEntityManager.CreateEntity<AttackArea>();
                auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
                game::PosPattern pattern;
                pattern.Init(1, 1, true);
                pattern.pos = pos;
                pattern.tileId = 0;
                ent->GetComp<CompPosPattern>()->positions = pattern;
                ent->GetComp<CompActivity>()->active = true;
                ent->GetComp<CompActivity>()->disableAfterOneTick = true;
                ent->GetComp<CompDamage>()->physDamage = 3;
                ent->GetComp<CompDamage>()->magicDamage = 0;
                ent->GetComp<CompTimeToLive>()->ticks = 1;
                ent->GetComp<CompTimeToLive>()->isActive = true;
                ent->GetComp<CompDrawTileInfo>()->layer = 2;
                
            }
            if (effect->counter <= 0) {
               effect->active = false;
               effect->tileId = 0;
            } else {
                effect->counter--;
            }
        }

        //Freezw
        if (effect->type == 2) {
            //printf("ASD");
           
            if (effect->counter > 0) {
                effect->tileId = game::TilesLib["shots"]["Freeze"];
                liver->GetComp<CompMoveDir>()->CanMove = false;
                effect->counter--;
            }
            if (effect->counter <= 0) {
               effect->active = false;
               liver->GetComp<CompMoveDir>()->CanMove = true;
               effect->tileId = 0;
            }
            
        }
        
    });
}