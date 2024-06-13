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

void ScreenCenter(Entity& entity, int offset[])
{
    PositionComponent* entityPosition = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
    SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));

    entityPosition->positionXY[0] = (screenWidth - sprite->getWidth()) / 2 + offset[0];
    entityPosition->positionXY[1] = (screenHeight - sprite->getHeight()) / 2 + offset[1];
}

int main()
{
    Entity player(50, 5);
    Entity enemy(20, 0);
    Entity arcade(1, 0);

    // INITIALIZATION OF ECS
    ECS ecs(player);

    //CODE
    std::vector<std::string> playerSprite = {"(-)", "(0)", "(_)"};
    player.addComponent(new SpriteComponent(playerSprite));

    std::vector<std::string> enemySprite = {"(-)", "(0)", "(_)"};
    enemy.addComponent(new SpriteComponent(enemySprite));

    std::vector<std::string> arcadeMachine = {
            "  ________________________",
            " /________________________\\",
            "|  ____________________  |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "| |____________________| |",
            "/  ____________________  \\",
            "| |                    | |",
            "| |                    | |",
            "| |                    | |",
            "\\ |____________________| /",
            "|                        |",
            "|     ______________     |",
            "|    |              |    |",
            "|    |              |    |",
            "|    |______________|    |",
            "\\________________________/"
    };

    arcade.addComponent(new SpriteComponent(arcadeMachine));

    renderSystem.entitiesToRender.push_back(arcade);
    renderSystem.entitiesToRender.push_back(player);
    renderSystem.entitiesToRender.push_back(enemy);

    renderSystem.hideCursor();

    collisionSystem.addEntity(player);
    collisionSystem.addEntity(enemy);

    ScreenCenter(arcade, new int[2]{0, 0});

    while (true)
    {
        timeSystem.startFrame();

        inputSystem.Update();

        //GAME HERE

        MovementSystem(player, 'w', new int[2]{0, -1}, collisionSystem);
        MovementSystem(player, 's', new int[2]{0, 1}, collisionSystem);
        MovementSystem(player, 'a', new int[2]{-1, 0}, collisionSystem);
        MovementSystem(player, 'd', new int[2]{1, 0}, collisionSystem);

        renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 47), 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    renderSystem.showCursor();

    return 0;
}
