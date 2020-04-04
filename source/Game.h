#pragma once

#include "GameStructures.h"
#include "stl.h"
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "EcsSystems.h"
#include "ECS.h"

namespace game
{

extern std::map<std::string, std::map<std::string, int>> TilesLib;
extern std::vector<game::Weapon> gGameWeapons;
extern std::vector<game::Shot_st> gGameShots;
extern float update_period;
extern std::vector<std::vector<game::edge>> gGameGraph;
extern sf::Vector2i gGameGraphSize;
extern std::vector<ecs::IEntity *> blocks;

class GameLevel
{

};

class floating_nums {
    public:
        char name[30];
        float speed = 0;
        float time = 0;
        float size = 0;
        sf::Vector2f position = sf::Vector2f();
        static void set_floating_nums(sf::Vector2i pos, const char *text, int size, sf::Color Color, float Speed, float duration);
        static stl::vector<floating_nums> nums; 
        static void OnUpdate();
        static void Init();
        static stl::vector<int> freeIds;
};

struct GameResults
{
public:
    enum CompleteStatus
    {
        FAILED,
        COMPLETE
    }
    status;
};

extern std::vector<sf::Vector2i> Calculate_Path(sf::Vector2i from, sf::Vector2i dest);

class Game
{
public:

    void Init(const stl::shared_ptr<sf::RenderWindow> &window);
    void Release();

    GameResults StartLevel(const char *levelFilename);

protected:

    stl::map<stl::string, unsigned> texNames;
    stl::shared_ptr<sf::RenderWindow> window;
};

}
