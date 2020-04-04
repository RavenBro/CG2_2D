#pragma once

#include "Event.h"
#include <SFML/Window.hpp>

namespace game
{

typedef sf::Event EventSfml;
struct EventRenderWindowWasCreated { stl::shared_ptr<sf::RenderWindow> wnd; };
struct EventRendererWasInited {};

}
