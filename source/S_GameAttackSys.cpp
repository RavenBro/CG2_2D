#include "EcsSystems.h"
#include "stl.h"


//#include <iostream>
//#include <vector>
//using namespace std;


#include "ECS.h"
#include "EcsComponents.h"
#include "Game.h"

void GameAttackSys::OnUpdate()
{
    ecs::Apply<CompPosPattern, CompActivity, CompDamage, CompOwner>([&](ecs::IEntity *area)
    {
        CompActivity *act = area->GetComp<CompActivity>();
        if (!act->active)
            return;
        if (act->disableAfterOneTick)
            act->active = false;
        CompPosPattern *positions = area->GetComp<CompPosPattern>();
        ecs::IEntity *owner = ecs::gEntityManager.GetEntityByDesc(area->GetComp<CompOwner>()->owner);

        ecs::Apply<CompPos, CompHealth, CompArmor>([&](ecs::IEntity *creature)
        {
            CompParty *creatParty = creature->GetComp<CompParty>();
            CompParty *ownerParty;
            CompParty party;
            if (owner) {
                ownerParty = owner->GetComp<CompParty>();
            } else {
                party.party = 0;
                ownerParty = &party;
            }
            if (creatParty && ownerParty && creatParty->party == ownerParty->party)
                return;

            CompPos *pos = creature->GetComp<CompPos>();
            if (positions->positions.IsPosExists(pos->v.x, pos->v.y))
            {
                CompDamage *damage = area->GetComp<CompDamage>();
                CompArmor *resist = creature->GetComp<CompArmor>();
                float physResist = 0.0f;
                float magicResist = 0.0f;
                if (resist)
                {
                    physResist = resist->physResist;
                    magicResist = resist->magicResist;
                }
                CompHealth *h = creature->GetComp<CompHealth>();
                h->value -= stl::max(damage->physDamage * (1.0f - physResist), 0.0f);
                h->value -= stl::max(damage->magicDamage * (1.0f - magicResist), 0.0f);
                
                if (area->GetComp<CompDamage>()->effector != 0 && creature->GetComp<CompEffect>()) {
                    creature->GetComp<CompEffect>()->active = true;
                    creature->GetComp<CompEffect>()->counter = creature->GetComp<CompEffect>()->duration;
                    creature->GetComp<CompEffect>()->type = area->GetComp<CompDamage>()->effector;
                    creature->GetComp<CompEffect>()->counter = creature->GetComp<CompEffect>()->duration;
                }

                int dam = (int) (stl::max(damage->physDamage * (1.0f - physResist), 0.0f) + stl::max(damage->magicDamage * (1.0f - magicResist), 0.0f));
                char damageText[10];
                sprintf(damageText, "-%d", dam);
                int textSize = 20;
                game::floating_nums::set_floating_nums(sf::Vector2i(pos->v.x + 1 ,pos->v.y), damageText, textSize, sf::Color(0xFF1111FF), 0.015, 100.0 );//damageText, textSize, sf::Color(0x111111ff), 1.0, 10.0);
                
                if (h->value <= 0.0f){
                    if(!creature->GetComp<TagPlayer>()){
                        ecs::gEntityManager.DestroyEntity(creature->GetDesc());
                    } else {
                        if (((Player*)creature)->godmod == false) {
                            creature->GetComp<CompMoveDir>()->CanMove = false;
                            creature->GetComp<CompHealth>()->value = 0;
                            creature->GetComp<CompAnimator>()->active = false;
                            creature->GetComp<CompDrawTileInfo>()->tileId = game::TilesLib["Mobes"]["Grave"];   
                        }
                    }    
                }
            }
        });
    });
}
