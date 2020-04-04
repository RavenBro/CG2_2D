#pragma once

/*
    In current realisation, entity is something that
    have components and can work and interact with
    other entities through 'Update()' (called on each frame update).

    If you want to initialize an entity, use OnCreated() instead of constructor.

    Here you can place all of the entity types you want to use.
*/

#include "ECS.h"
#include "EcsComponents.h"
#include <iostream>

class Player : public ecs::Entity<
    CompPos,
    CompAnimator,
    CompEffect,
    CompDir,
    CompMoveDir,
    CompHealth,
    CompDrawTileInfo,
    CompOwnedItems,
    CompCurrentAttackArea,
    CompStamina,
    CompMana,
    CompArmor,
    CompParty,
    // CompVisibility, // May be used for hide player (feature).
    //TagAlly,
    TagCollidable,
    TagPlayer>
{
public:
    ECS_ENTITY_DEFAULTS(Player)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
    bool godmod = false;
};

class AttackArea : public ecs::Entity<
    CompPosPattern,
    CompActivity,
    CompDamage,
    CompOwner,
    CompVisibility,
    CompTimeToLive,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(AttackArea)
};

class Walls : public ecs::Entity<
    CompPosPattern,
    CompDrawTileInfo,
    TagIsWalls,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Walls)
};

class Floor : public ecs::Entity<
    CompPosPattern,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(Floor)
};


//My own entities
//Firing shell
class Shot : public ecs::Entity<
    CompPos,
    CompMoveDir,
    TagCollidable,
    TagBumpable,
    CompDamage,
    CompTimeToLive,
    CompOwner,
    CompAnimation,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(Shot)
    void OnCreated() override;
    void OnDestroy() override;
};

class SchoolShooter : public ecs::Entity<
    CompPos,
    CompDir,
    TagShooter,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(SchoolShooter)
    void Activate();
    void OnCreated() override;
    void OnUpdate() override;
    bool active = false;
    int maxTime = 15;
    int timer;
};

class SpecialArea : public ecs::Entity<
    CompPos,
    CompDrawTileInfo,
    CompOwner,
    CompParty,
    TagSpecial,
    CompDamage,
    CompCurrentAttackArea>
{
public:
    ECS_ENTITY_DEFAULTS(SpecialArea)
    void SpecialCreate(int seed);
    void OnUpdate() override;
    int seed;
};

class Simple : public ecs::Entity<
    CompPos,
    CompDrawTileInfo>
{
    public:
       ECS_ENTITY_DEFAULTS(Simple) 
};

class SimpleBlock : public ecs::Entity<
    CompPos,
    CompDrawTileInfo,
    TagCollidable>
{
    public:
       ECS_ENTITY_DEFAULTS(SimpleBlock) 
};

class Collectable : public ecs::Entity<
    CompPos,
    CompOwnedItems,
    TagCollectable,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(Collectable)
    void OnUpdate() override;
};

// Ususal enemies have no stamina and mana.
class Enemy : public ecs::Entity<
    CompPos,
    CompEffect,
    CompMoveDir,
    CompHealth,
    CompDrawTileInfo,
    CompCurrentAttackArea,
    CompArmor,
    CompDamage,
    CompParty,
    CompVisionRadius,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Enemy)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
private:
    unsigned curTick = 0;
};

class Boss : public ecs::Entity<
    CompAnimator,
    CompParty,
    TagBoss,
    CompPos,
    CompDrawTileInfo,
    CompMoveDir,
    CompParty,
    CompHealth,
    CompArmor,
    CompCurrentAttackArea,
    CompEffect,
    CompDamage,
    CompRoute>
{
public:
    bool alive = false;
    bool isFinal = false;
    ECS_ENTITY_DEFAULTS(Boss)
    void OnCreated() override;
    void OnDestroy() override;
    //void OnDestroy() override;
    void OnUpdate() override;
    bool HaveMoved = false;
    int finalCounter = 0;
    std::vector<SchoolShooter *> shooters;
    void Activate();

private:
    sf::Vector2i Move;
    unsigned curTick = 0;
};

#ifndef ECS_REGISTER_ENTITY_TYPE
#define ECS_REGISTER_ENTITY_TYPE(typeName)
#endif

/*
    Register here all of the entity types
    you want to create by string names (and therefore from files).
*/
ECS_REGISTER_ENTITY_TYPE(Simple)
ECS_REGISTER_ENTITY_TYPE(SimpleBlock)
ECS_REGISTER_ENTITY_TYPE(SchoolShooter)
ECS_REGISTER_ENTITY_TYPE(SpecialArea)
ECS_REGISTER_ENTITY_TYPE(Player)
ECS_REGISTER_ENTITY_TYPE(Collectable)
ECS_REGISTER_ENTITY_TYPE(Shot)
ECS_REGISTER_ENTITY_TYPE(AttackArea)
ECS_REGISTER_ENTITY_TYPE(Walls)
ECS_REGISTER_ENTITY_TYPE(Floor)
ECS_REGISTER_ENTITY_TYPE(Enemy)
ECS_REGISTER_ENTITY_TYPE(Boss)