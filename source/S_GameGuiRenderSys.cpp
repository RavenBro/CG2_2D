#include "EcsSystems.h"


#include "Renderer.h"
#include "Game.h"
#include <limits.h>

sf::Vector2f camPos;

static const sf::Vector2i cellSize = sf::Vector2i(40, 40);
static const sf::Vector2i vpCells = sf::Vector2i(21, 15);
static const sf::Vector2i vpCellsPos = sf::Vector2i(1, 1);  
static const sf::Vector2i wndCells = sf::Vector2i(30, 17);
static const sf::Vector2i wSize = sf::Vector2i(cellSize.x * wndCells.x, cellSize.y * wndCells.y);
static const sf::Vector2i vpPos = sf::Vector2i(cellSize.x * vpCellsPos.x, cellSize.y * vpCellsPos.y);
static const sf::Vector2i vpSize = sf::Vector2i(cellSize.x * vpCells.x, cellSize.y * vpCells.y);
static const sf::Vector2i curWeaponPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 4);
static const sf::Vector2i ownedItemsPos = sf::Vector2i(vpPos.x + vpSize.x + cellSize.x * 1, vpPos.y + cellSize.y * 6);
static const sf::Vector2i healthPos = sf::Vector2i(190, 555);
static const sf::Vector2i staminaPos = sf::Vector2i(190, 585);
static const sf::Vector2i manaPos = sf::Vector2i(190, 615);
static const int itemsBarWidth = wndCells.x - vpCells.x - vpCellsPos.x - 2;
static const int itemsBarHeight = 5;
static const int MaxRoute = 300;
static int RouteLength = 0;
static const sf::Vector2i BossPos = sf::Vector2i(40, 40);
static const sf::Vector2i EmptyBarSize = sf::Vector2i(300, 30);
static const sf::Vector2i FullBarSize = sf::Vector2i(255, 18);
static const sf::Vector2i BossFullBarSize = sf::Vector2i(700, 40);
static const sf::Vector2i FullBarShift = sf::Vector2i(23, 5);
static const sf::Vector2i BossBarShift = sf::Vector2i(70, 5);
static const sf::Vector2i HealthBPos = sf::Vector2i(60, 555);
static const sf::Vector2i StaminaBPos = sf::Vector2i(60, 585);
static const sf::Vector2i ManaBPos = sf::Vector2i(60, 615);
static const sf::Vector2i BossBarSize = sf::Vector2i(840, 50);
static const sf::Vector2i InventoryPos = sf::Vector2i(380, 560);
static const sf::Vector2i InventoryTileSize = sf::Vector2i(80, 80);

static stl::map<stl::string, int> guiTextures;

namespace game
{
void floating_nums::Init(){
    for (int i = 0; i < 40; i++)
    {
        floating_nums::nums.push_back(floating_nums());
        snprintf(floating_nums::nums[i].name, 30, "floating%d", i);
        game::gRenderer.GuiAddText(floating_nums::nums[i].name, 2, sf::Vector2i(0,0), "", 1, sf::Color(0));
        floating_nums::freeIds.push_back(i);   
    }
}

void floating_nums::set_floating_nums(sf::Vector2i pos, const char *text, int size, sf::Color Color, float Speed, float duration)
{     
    if (floating_nums::freeIds.size() > 0) {
        floating_nums *whom = &floating_nums::nums[freeIds[freeIds.size()-1]];
        //float sizeToShift = 
        whom->position.x = pos.x;
        whom->position.y = pos.y;
        char buffer[30];
        snprintf(buffer, 30, "%s", whom->name);
        gRenderer.GuiChangePos(whom->name, pos);
        whom->size = size;
        gRenderer.GuiChangeSize(whom->name, sf::Vector2i(0,0));
        gRenderer.GuiChangeColor(whom->name, Color);
        gRenderer.GuiChangeText(whom->name, text);
        whom->speed = Speed;
        whom->time = duration; 
        freeIds.pop_back();
    }     
}

void floating_nums::OnUpdate()
{
    for (int i = floating_nums::nums.size() - 1; i >= 0; i--)
    {
        int flag = 0;
        for (int j = 0; j < floating_nums::freeIds.size(); j++)
        {
            if (floating_nums::freeIds[j] == i) {
                flag = 1;
            }
        }
        if (flag) {
            continue;
        }
        gRenderer.GuiChangeSize(floating_nums::nums[i].name, sf::Vector2i(floating_nums::nums[i].size,0));
        floating_nums::nums[i].position.y += floating_nums::nums[i].speed * game::update_period;
        sf::Vector2i new_pos = sf::Vector2i((int) floating_nums::nums[i].position.x * cellSize.x - (camPos.x - 0.5 * vpCells.x) * cellSize.x, 1.0 * vpCells.y * cellSize.y - ((int) (floating_nums::nums[i].position.y * cellSize.y - (camPos.y - 0.5 * vpCells.y) * cellSize.y)));
        game::gRenderer.GuiChangePos(floating_nums::nums[i].name,  new_pos);
        floating_nums::nums[i].time -= game::update_period;
        //printf("AAAAAA %d %d %d\n",i, new_pos.x, new_pos.y);
        if (floating_nums::nums[i].time <= 0) {
            floating_nums::freeIds.push_back(i);
            gRenderer.GuiChangeText(floating_nums::nums[i].name, "");
        }
    } 
}
stl::vector<floating_nums> floating_nums::nums = stl::vector<floating_nums>();
stl::vector<int> floating_nums::freeIds = stl::vector<int>();
}

void GameGuiRenderSys::OnEvent(const game::EventRenderWindowWasCreated &evt)
{
    evt.wnd->setSize(sf::Vector2u(wSize));
}

void GameGuiRenderSys::OnEvent(const game::EventRendererWasInited &evt)
{
    guiTextures.emplace("backBlock", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1969.png"));
    guiTextures.emplace("left", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1990.png"));
    guiTextures.emplace("right", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1988.png"));
    guiTextures.emplace("up", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1987.png"));
    guiTextures.emplace("down", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1989.png"));
    guiTextures.emplace("topLeft", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1978.png"));
    guiTextures.emplace("topRight", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1979.png"));
    guiTextures.emplace("bottomLeft", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1981.png"));
    guiTextures.emplace("bottomRight", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1980.png"));
    guiTextures.emplace("emptyBar", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_bar.png"));
    guiTextures.emplace("emptyBar_holed", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_bar_with_hole.png"));
    guiTextures.emplace("specialItem", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/empty_special.png"));
    guiTextures.emplace("FirstScreen", game::gRenderer.AddTexture("FirstScreen.png"));
    //guiTextures.emplace("emptyPlace", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1955.png"));
    
    for(int i = 0; i < MaxRoute; i++){
        char buf[20];
        snprintf(buf, 20, "Route%d", i);
        game::gRenderer.GuiAddTile(buf, 1, sf::Vector2i(0,0), sf::Vector2i(0,0), 7);
    }

    game::gRenderer.GuiAddTexturedRect("up", 1,
        { vpPos.x + cellSize.x * 1, vpPos.y },
        { vpSize.x - cellSize.x * 2, cellSize.y * 1 }, guiTextures["up"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(vpCells.x - 2, 1), guiTextures["up"]));
    game::gRenderer.GuiAddTexturedRect("down", 1,
        { vpPos.x + cellSize.x * 1, vpPos.y + vpSize.y - cellSize.y * 1 },
        { vpSize.x - cellSize.x * 2, cellSize.y * 1 }, guiTextures["down"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(vpCells.x - 2, 1), guiTextures["down"]));
    game::gRenderer.GuiAddTexturedRect("left", 1,
        { vpPos.x, vpPos.y + cellSize.y * 1 },
        { cellSize.x * 1, vpSize.y - cellSize.y * 2 }, guiTextures["left"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(1, vpCells.y - 2), guiTextures["left"]));
    game::gRenderer.GuiAddTexturedRect("right", 1,
        { vpPos.x + vpSize.x - cellSize.x * 1, vpPos.y + cellSize.y * 1 },
        { cellSize.x * 1, vpSize.y - cellSize.y * 2 }, guiTextures["right"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(1, vpCells.y - 2), guiTextures["right"]));


    game::gRenderer.GuiAddTexturedRect("topLeft", 1,
        { vpPos.x, vpPos.y },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["topLeft"]);
    game::gRenderer.GuiAddTexturedRect("topRight", 1,
        { vpPos.x + vpSize.x - cellSize.x * 1, vpPos.y },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["topRight"]);
    game::gRenderer.GuiAddTexturedRect("bottomLeft", 1,
        { vpPos.x, vpPos.y + vpSize.y - cellSize.y * 1 },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["bottomLeft"]);
    game::gRenderer.GuiAddTexturedRect("bottomRight", 1,
        { vpPos.x + vpSize.x - cellSize.x * 1, vpPos.y + vpSize.y - cellSize.y * 1 },
        { cellSize.x * 1, cellSize.y * 1 }, guiTextures["bottomRight"]);

    game::gRenderer.GuiAddTexturedRect("backBlock", -1,
        { 0, 0 },
        { wSize.x, wSize.y }, guiTextures["backBlock"],
        { 0, 0 }, game::gRenderer.ntoat(sf::Vector2f(wndCells.x, wndCells.y), guiTextures["backBlock"]));

    //game::gRenderer.GuiAddTile("SAMPLE", 3, sf::Vector2i(100,100),cellSize, game::TilesLib["other"]["EmptyPlace"]);


    game::gRenderer.GuiAddText("HandledText", 2, {940, 30 },
        "Handled Item", 25, sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("CurrentLabel", 2, {940, 150 },
        "Current Item", 25, sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("CurrentName", 2, sf::Vector2i(1010,180), "[name]", 20, sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("CurrentDesc1", 2, sf::Vector2i(960,210),
        "", 15, sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("CurrentDesc2", 2, sf::Vector2i(960,230),
        "", 15, sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("InventoryLabel", 2, {980, 250 },
        "Inventory", 20, sf::Color(0xFFCF00FF));

    game::gRenderer.GuiAddTile("HandledItem", 2, sf::Vector2i(990,70), sf::Vector2i(80,80), 7);
    game::gRenderer.GuiAddTile("weaponBar", 2, sf::Vector2i(900,190), {60,60}, 0);    

    
    game::gRenderer.GuiAddTexturedRect("HealthBar", 2, HealthBPos, EmptyBarSize, guiTextures["emptyBar"]);
    game::gRenderer.GuiAddTexturedRect("ManaBar", 2, ManaBPos, EmptyBarSize, guiTextures["emptyBar"]);
    game::gRenderer.GuiAddTexturedRect("StaminaBar", 2, StaminaBPos, EmptyBarSize, guiTextures["emptyBar"]);

    game::gRenderer.GuiAddColoredRect("healthFill", 2, HealthBPos + FullBarShift, FullBarSize, sf::Color(0xBF3030FF));
    game::gRenderer.GuiAddColoredRect("staminaFill", 2, StaminaBPos + FullBarShift, FullBarSize, sf::Color(0x9E9E9EFF));
    game::gRenderer.GuiAddColoredRect("manaFill", 2, ManaBPos + FullBarShift, FullBarSize, sf::Color(0x1964B0FF));
    //FFCF00
    game::gRenderer.GuiAddTexturedRect("HealthBar", 2, HealthBPos, EmptyBarSize, guiTextures["emptyBar_holed"]);
    game::gRenderer.GuiAddTexturedRect("ManaBar", 2, ManaBPos, EmptyBarSize, guiTextures["emptyBar_holed"]);
    game::gRenderer.GuiAddTexturedRect("StaminaBar", 2, StaminaBPos, EmptyBarSize, guiTextures["emptyBar_holed"]);
    
    game::gRenderer.GuiAddTexturedRect("BossBar", 2, BossPos, BossBarSize, guiTextures["emptyBar"]);
    game::gRenderer.GuiAddColoredRect("BossFill", 2, BossPos + BossBarShift, BossFullBarSize, sf::Color(0x606060FF));
    game::gRenderer.GuiAddTexturedRect("BosshBar", 2, BossPos, BossBarSize, guiTextures["emptyBar_holed"]);
    
    game::gRenderer.GuiChangeSize("BossBar", {1,1});
    game::gRenderer.GuiChangeSize("BossFill", {1,1});
    game::gRenderer.GuiChangeSize("BosshBar", {1,1});


    game::gRenderer.GuiAddText("healthBar", 2, healthPos, "100.0", 20, sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("staminaBar", 2, staminaPos, "100.0", 20,  sf::Color(0xFFCF00FF));
    game::gRenderer.GuiAddText("manaBar", 2, manaPos, "100.0", 20, sf::Color(0xFFCF00FF));

    game::gRenderer.GuiAddText("HintBar", 2, {900, 500}, "To navigate in inventory\nuse Q and E, to make\nelemnt hot-keyd use SPACE", 16, sf::Color(0xFFFFFFFF));

    for (int i = 0; i < itemsBarWidth; i++) {
        char buf[15];
        snprintf(buf, 15, "inventBack%d", i);
        game::gRenderer.GuiAddTexturedRect(buf, 2, InventoryPos + sf::Vector2i(i * InventoryTileSize.x, 0), InventoryTileSize, guiTextures["specialItem"]);
        char buffer[15];
        snprintf(buffer, 15, "inventHere%d", i);
        game::gRenderer.GuiAddTile(buffer, 2, InventoryPos + sf::Vector2i(i * InventoryTileSize.x + (int)(0.125 * InventoryTileSize.x), (int)(0.125 * InventoryTileSize.x)), sf::Vector2i((int)(6.0 / 8 * InventoryTileSize.x), (int)(6.0 / 8 * InventoryTileSize.y)), 0);
        //printf("%s\n",buffer);
    }


    for (int i = 0; i < itemsBarWidth * itemsBarHeight; ++i){
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "empty%d", i);
        game::gRenderer.GuiAddTile(buf, 2, sf::Vector2i(
            (i % itemsBarWidth) * cellSize.x + ownedItemsPos.x,
            (i / itemsBarWidth) * cellSize.y + ownedItemsPos.y),
            cellSize, 0);
    }
    
    game::gRenderer.GuiAddTile("SpecialItem", 2, sf::Vector2i(ownedItemsPos.x,ownedItemsPos.y),
            cellSize, 0);//guiTextures["specialItem"]);

    for (int i = 0; i < itemsBarWidth * itemsBarHeight; ++i)
    {
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "itm%d", i);
        game::gRenderer.GuiAddTile(buf, 2, sf::Vector2i(
            (i % itemsBarWidth) * cellSize.x + ownedItemsPos.x,
            (i / itemsBarWidth) * cellSize.y + ownedItemsPos.y),
            cellSize, 0);
    }

    //int tid = game::gRenderer.AddTile(sf::Color(0x000000FFU));
    //game::gRenderer.SetOuterFillTile(tid);

    game::gRenderer.GuiAddTexturedRect("FirstScreen", 3, {0,0}, {1200,680}, guiTextures["FirstScreen"]);
    

    game::gRenderer.SetVisionAreaSize(sf::Vector2f(vpSize.x / cellSize.x, vpSize.y / cellSize.y));
    game::gRenderer.SetCameraViewport(vpPos.x, vpPos.y, vpSize.x, vpSize.y);
    game::gRenderer.SetCameraCenterPos(sf::Vector2f(20.0f, 20.0f));

    game::floating_nums::Init();
}

void GameGuiRenderSys::OnUpdate()
{
    static bool flag = 0;
    if (!flag) {
        flag = true;
        for (int i = 0; i < itemsBarWidth * itemsBarHeight; ++i){
            constexpr int bufSize = 16;
            char buf[bufSize] = {};
            std::snprintf(buf, bufSize, "empty%d", i);
            if (i < itemsBarWidth) {
                game::gRenderer.GuiChangeTile(buf, game::TilesLib["UIObjects"]["specialItem"]);
            } else {
                game::gRenderer.GuiChangeTile(buf, game::TilesLib["OtherObjects"]["EmptyPlace"]);
            }
            
        }
        game::gRenderer.GuiChangeTile("SpecialItem", game::TilesLib["OtherObjects"]["SpecialChoice"]);
    }

    ecs::Apply<TagBoss>([&](IEntity *ent)
    {
        if (((Boss*)ent)->isFinal){
            game::gRenderer.GuiChangeSize("BossBar", BossBarSize);
            game::gRenderer.GuiChangeSize("BossFill", sf::Vector2i((int)(BossFullBarSize.x * (ent->GetComp<CompHealth>()->value) /1000),BossFullBarSize.y));
            game::gRenderer.GuiChangeSize("BosshBar", BossBarSize);
        }
    });

    IEntity *plr;
    ecs::Apply<CompHealth, CompStamina, CompMana, CompOwnedItems, TagPlayer>([&](IEntity *ent)
    {       
        plr = ent;
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompHealth>()->value);
        game::gRenderer.GuiChangeText("healthBar", buf);
        game::gRenderer.GuiChangeSize("healthFill", {(int)(FullBarSize.x / (100.0 / ent->GetComp<CompHealth>()->value)),FullBarSize.y});
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompStamina>()->value);
        game::gRenderer.GuiChangeSize("staminaFill", {(int)(FullBarSize.x / (100.0 / ent->GetComp<CompStamina>()->value)),FullBarSize.y});
        game::gRenderer.GuiChangeText("staminaBar", buf);
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompMana>()->value);
        game::gRenderer.GuiChangeSize("manaFill", {(int)(FullBarSize.x / (100.0 / ent->GetComp<CompMana>()->value)),FullBarSize.y});
        game::gRenderer.GuiChangeText("manaBar", buf);


        CompOwnedItems *items = ent->GetComp<CompOwnedItems>();
        if(items->specialId == -1 && items->weapons.size() != 0) {
            items->specialId = 0;
        }
        int i = 0;
        for (auto item : items->weapons)
        {
            std::snprintf(buf, bufSize, "itm%d", i);
            game::gRenderer.GuiChangeTile(buf, item->tileId);
            
            if (i < itemsBarWidth) {
                char buffer[15];
                snprintf(buffer, 15, "inventHere%d", i);
                //printf("%s\n",buffer);
                game::gRenderer.GuiChangeTile(buffer, item->tileId);
            }
            ++i;
        }
        if (items->curWeaponId >= 0) {
            game::gRenderer.GuiChangeTile("HandledItem", items->weapons[items->curWeaponId]->tileId);
        }

        if (items->specialId >= 0 && items->specialId < itemsBarWidth*itemsBarHeight) {
            game::gRenderer.GuiChangePos("SpecialItem", sf::Vector2i(ownedItemsPos.x + cellSize.x * (items->specialId % itemsBarWidth), ownedItemsPos.y + cellSize.y * (items->specialId / itemsBarWidth)));
            game::gRenderer.GuiChangeTile("weaponBar", items->weapons[items->specialId]->tileId);
            game::gRenderer.GuiChangeText("CurrentDesc1", items->weapons[items->specialId]->description[0]);
            game::gRenderer.GuiChangeText("CurrentDesc2", items->weapons[items->specialId]->description[1]);
            game::gRenderer.GuiChangeText("CurrentName", &items->weapons[items->specialId]->name[0]);
        }
    });


    game::gRenderer.DrawGui(0, INT_MAX);
    
    //printf("TEEEEST %lf\n",game::floating_nums::nums[0].position.y);
    //game::floating_nums::OnUpdate();
}

void GameBackgroundGuiRenderSys::OnUpdate()
{
    game::gRenderer.DrawGui(INT_MIN, -1);
}

