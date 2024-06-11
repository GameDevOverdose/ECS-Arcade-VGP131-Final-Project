#include <algorithm>
#include <vector>
#include <thread>

#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/TimeSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/CollisionSystem.h"

#include "ECS.h"

// INITIALIZATION OF SYSTEMS
TransformSystem transformSystem;
RenderSystem renderSystem;

int screenWidth = 120;
int screenHeight = 30;

const int targetFPS = 60;

TimeSystem timeSystem(targetFPS);
InputSystem inputSystem;
CollisionSystem collisionSystem;

void MovementSystem(Entity& entity, char input, int* moveXY, CollisionSystem& collisionSystem)
{
    PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));

    if (inputSystem.getKeyState(input) == InputSystem::KEY_HELD)
    {
        // Predict the new position
        int predictedPosition[2] = {position->positionXY[0] + moveXY[0], position->positionXY[1] + moveXY[1]};

        // Check if the new position would result in a collision
        if (!collisionSystem.wouldCollide(entity, predictedPosition))
        {
            // If not, perform the move
            transformSystem.UpdateTransform(&entity, moveXY[0], moveXY[1]);
        }
    }
}

int main()
{
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

    transformSystem.UpdateTransform(&player, 10, 2);

    std::vector <std::vector<char>> sprite2 = {{'(', '-', ')'}, {'(', '0', ')'}, {'(', '_', ')'}};

    PositionComponent position2(0,0);
    SpriteComponent spriteComp2(sprite2);
    enemy.addComponent(&position2);
    enemy.addComponent(&spriteComp2);

    transformSystem.SetTransform(&player, 10, 0);
    transformSystem.SetTransform(&enemy, 20, 0);

    renderSystem.entitiesToRender.push_back(player);
    renderSystem.entitiesToRender.push_back(enemy);

    renderSystem.hideCursor();

    const std::chrono::milliseconds frameDuration(1000 / targetFPS);

    collisionSystem.addEntity(player);
    collisionSystem.addEntity(enemy);

    while (true)
    {
        timeSystem.startFrame();

        inputSystem.Update();

        //GAME HERE

        MovementSystem(player, 'w', new int[2]{0, -1}, collisionSystem);
        MovementSystem(player, 's', new int[2]{0, 1}, collisionSystem);
        MovementSystem(player, 'a', new int[2]{-1, 0}, collisionSystem);
        MovementSystem(player, 'd', new int[2]{1, 0}, collisionSystem);

        renderSystem.RenderScreen(screenWidth, screenHeight);

        timeSystem.endFrame();
    }

    renderSystem.showCursor();

    return 0;
}
