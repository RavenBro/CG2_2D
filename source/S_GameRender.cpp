#include "stl.h"
#include "EcsSystems.h"
#include "Renderer.h"
#include "Game.h"

#define LAYERS_COUNT CompDrawTileInfo::LAYERS_COUNT

extern sf::Vector2f camPos;

void GameRenderSys::OnUpdate()
{
    camPos = sf::Vector2f(0, 0);
    stl::map<CompDrawTileInfo *, CompPos *> singleObjects[LAYERS_COUNT];
    stl::map<CompDrawTileInfo *, CompPosPattern *> patternObjects[LAYERS_COUNT];

    stl::vector<ecs::IEntity *> Effects;
    ecs::Apply<CompEffect>([&](ecs::IEntity *ent)
    {
        if (ent->GetComp<CompEffect>()->active) {
        auto d = ecs::gEntityManager.CreateEntity<Simple>();
        auto new_ent = ecs::gEntityManager.GetEntityByDesc(d);
        new_ent->GetComp<CompPos>()->v = ent->GetComp<CompPos>()->v;
        new_ent->GetComp<CompDrawTileInfo>()->layer = 2;
        new_ent->GetComp<CompDrawTileInfo>()->tileId = ent->GetComp<CompEffect>()->tileId;
        Effects.push_back(new_ent);}
    });

    ecs::Apply<CompPos, CompDrawTileInfo>([&](ecs::IEntity *ent)
    {
        CompVisibility *vis = ent->GetComp<CompVisibility>();
        if (vis)
        {
            if (!vis->visible)
                return;
            if (vis->hideAfterOneTick)
                vis->visible = false;
        }
        CompDrawTileInfo *info = ent->GetComp<CompDrawTileInfo>();
        if (info->layer < LAYERS_COUNT)
        {
            CompPos *pos = ent->GetComp<CompPos>();
            singleObjects[info->layer].emplace(info, pos);
            if (ent->GetComp<TagPlayer>())
                camPos = sf::Vector2f(pos->v) + sf::Vector2f(0.5f, 0.5f);
        }
    });


    ecs::Apply<CompPosPattern, CompDrawTileInfo>([&](ecs::IEntity *ent)
    {
        CompVisibility *vis = ent->GetComp<CompVisibility>();
        if (vis)
        {
            if (!vis->visible)
                return;
            if (vis->hideAfterOneTick)
                vis->visible = false;
        }
        CompDrawTileInfo *info = ent->GetComp<CompDrawTileInfo>();
        if (info->layer < LAYERS_COUNT)
        {
            CompPosPattern *pattern = ent->GetComp<CompPosPattern>();
            pattern->positions.tileId = info->tileId;
            patternObjects[info->layer].emplace(info, pattern);
        }
    });

    game::gRenderer.SetCameraCenterPos(camPos);
    game::gRenderer.BeginDrawWorld();
    for (int i = 0; i < LAYERS_COUNT; ++i)
    {
        for (auto &surf : patternObjects[i])
            game::gRenderer.DrawTiledSurface(surf.second->positions);
        for (auto &obj : singleObjects[i])
            game::gRenderer.DrawSingleTile(obj.first->tileId, sf::Vector2f(obj.second->v.x, obj.second->v.y));
    }
    while(Effects.size() > 0) {
        Effects[Effects.size()-1]->GetComp<CompDrawTileInfo>()->tileId = 0;
        ecs::gEntityManager.DestroyEntity(Effects[Effects.size()-1]->GetDesc());
        Effects.pop_back();
    }
}
