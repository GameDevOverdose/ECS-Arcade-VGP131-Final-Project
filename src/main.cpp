#include <vector>

#include "Systems/RenderSystem.h"
#include "Systems/TimeSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/CollisionSystem.h"

#include "ECS.h"

// INITIALIZATION OF SYSTEMS
int screenWidth = 120;
int screenHeight = 30;

const int targetFPS = 60;

RenderSystem renderSystem;
TimeSystem timeSystem(targetFPS);
MovementSystem movementSystem;
CollisionSystem collisionSystem;

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
    std::vector<std::string> playerSprite = {"0"};
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
    ScreenCenter(player, new int[2]{0, 0});

    int movementDirection[2] = {0, 0};
    char input = 'n';

    while (true)
    {
        timeSystem.startFrame();

        inputSystem.Update();

        //GAME HERE

        /*movementSystem.MoveInput(player, 'w', new int[2]{0, -1}, collisionSystem);
        movementSystem.MoveInput(player, 's', new int[2]{0, 1}, collisionSystem);
        movementSystem.MoveInput(player, 'a', new int[2]{-1, 0}, collisionSystem);
        movementSystem.MoveInput(player, 'd', new int[2]{1, 0}, collisionSystem);*/

        movementDirection[0] = 0;
        movementDirection[1] = 0;

        if (inputSystem.getKeyState('w') == InputSystem::KEY_HELD)
        {
            input = 'w';
        }
        else if (inputSystem.getKeyState('s') == InputSystem::KEY_HELD)
        {
            input = 's';
        }
        else if (inputSystem.getKeyState('a') == InputSystem::KEY_HELD)
        {
            input = 'a';
        }
        else if (inputSystem.getKeyState('d') == InputSystem::KEY_HELD)
        {
            input = 'd';
        }

        // Reset the movement direction


        // Move the player in the direction of the last input direction
        switch(input)
        {
            case 'w':
                movementDirection[1] = -1;
                break;
            case 's':
                movementDirection[1] = 1;
                break;
            case 'a':
                movementDirection[0] = -1;
                break;
            case 'd':
                movementDirection[0] = 1;
                break;
        }

        movementSystem.Move(player, movementDirection, collisionSystem);

        renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    renderSystem.showCursor();

    return 0;
}
