#include <algorithm>
#include <vector>

#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"
#include "ECS.h"

int main()
{
    // INITIALIZATION OF SYSTEMS
    TransformSystem transformSystem;
    RenderSystem renderSystem;

    Entity player;
    Entity enemy;

    // INITIALIZATION OF ECS
    ECS ecs(player);

    //CODE
    std::vector <std::vector<char>> sprite = {{'(', '-', ')'}, {'(', '0', ')'}, {'(', '_', ')'}};

    PositionComponent position(0,0);
    SpriteComponent spriteComp(sprite);
    player.addComponent(&position);
    player.addComponent(&spriteComp);

    transformSystem.UpdateMove(&player, 10, 2);

    std::vector <std::vector<char>> sprite2 = {{'(', '-', ')'}, {'(', '0', ')'}, {'(', '_', ')'}};

    PositionComponent position2(0,0);
    SpriteComponent spriteComp2(sprite2);
    enemy.addComponent(&position2);
    enemy.addComponent(&spriteComp2);

    transformSystem.UpdateMove(&player, 10, 0);
    transformSystem.UpdateMove(&enemy, 20, 0);

    renderSystem.entitiesToRender.push_back(player);
    renderSystem.entitiesToRender.push_back(enemy);

    renderSystem.UpdateRender();

    return 0;
}
