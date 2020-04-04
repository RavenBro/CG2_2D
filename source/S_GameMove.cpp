#include "EcsSystems.h"
#include "Game.h"

void MoveMovables()
{
    ecs::Apply<CompPos, CompMoveDir>([&](ecs::IEntity *movable)
    {
        CompMoveDir *dir = movable->GetComp<CompMoveDir>();
        CompPos *pos = movable->GetComp<CompPos>();
        if (dir->CanMove == false) {
            return;
        }
        if (movable->GetComp<TagCollidable>())
        {
            bool isbumpable = movable->GetComp<TagBumpable>() != NULL;
            bool canMove = true;
            ecs::Apply<TagCollidable>([&](ecs::IEntity *collidable)
            {
                CompPos *colPos = collidable->GetComp<CompPos>();
                CompPosPattern *colPattern = collidable->GetComp<CompPosPattern>();
                if (colPos)
                {
                    if (pos->v + dir->v == colPos->v)
                        canMove = false;
                }
                else if (colPattern)
                {
                    if (colPattern->positions.IsPosExists(pos->v.x + dir->v.x, pos->v.y + dir->v.y))
                        canMove = false;

                    /*for (int i = 0; i < colPattern->positions.size.x; i++)
                    {
                        for (int j = 0; j < colPattern->positions.size.y; j++)
                        {
                            printf("%d ", colPattern->positions.pattern[i][j]);
                        }
                        printf("\n");
                    }*/
                }
            });
            if (canMove)
                pos->v += dir->v;
            else {
                if (isbumpable) {
                    try
                    {
                        movable->OnDestroy();
                    }
                    catch(const std::exception& e)
                    {}
                    ecs::gEntityManager.DestroyEntity(movable->GetDesc());
                }
            }
        }
        else
        {
            pos->v += dir->v;
        }
        game::floating_nums::OnUpdate();
    });
    ecs::Apply<TagBoss>([&](ecs::IEntity *movable)
    {
        if (movable->GetComp<CompMoveDir>()->v != sf::Vector2i(0,0)){
            ((Boss*)movable)->HaveMoved = true;
        }
    });
}

void MoveAttackArea()
{
    ecs::Apply<CompPos, CompCurrentAttackArea>([&](ecs::IEntity *ent)
    {
        CompCurrentAttackArea *areaComp = ent->GetComp<CompCurrentAttackArea>();
        ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(areaComp->ent);
        area->GetComp<CompPosPattern>()->positions.pos = ent->GetComp<CompPos>()->v + areaComp->shift;
    });
}

void GameMoveSys::OnUpdate()
{
    MoveMovables();
    MoveAttackArea();
}
