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

ECS ecs;

RenderSystem renderSystem;
TimeSystem timeSystem(targetFPS);
MovementSystem movementSystem;
CollisionSystem collisionSystem(&ecs);

void ScreenCenter(Entity& entity, int offset[])
{
    PositionComponent* entityPosition = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
    SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));

    entityPosition->positionXY[0] = (screenWidth - sprite->getWidth()) / 2 + offset[0];
    entityPosition->positionXY[1] = (screenHeight - sprite->getHeight()) / 2 + offset[1];
}

//Arcade
Entity arcade("Arcade", 0, 0);
Entity arcadeBoundaryLeft("Arcade Boundary-Left", 0, 0);
Entity arcadeBoundaryRight("Arcade Boundary-Right", 0, 0);
Entity arcadeBoundaryTop("Arcade Boundary-Top", 0, 0);
Entity arcadeBoundaryDown("Arcade Boundary-Down", 0, 0);

void CreateArcade()
{
    ecs.addEntities({&arcade, &arcadeBoundaryLeft, &arcadeBoundaryRight, &arcadeBoundaryTop, &arcadeBoundaryDown});

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

    std::vector<std::string> boundaryVertical = {
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
            "|",
    };

    std::vector<std::string> boundaryHorizontal = {
            "____________________",
    };

    arcade.addComponent(new SpriteComponent(arcadeMachine));

    arcadeBoundaryLeft.addComponent(new SpriteComponent(boundaryVertical));
    arcadeBoundaryRight.addComponent(new SpriteComponent(boundaryVertical));
    arcadeBoundaryTop.addComponent(new SpriteComponent(boundaryHorizontal));
    arcadeBoundaryDown.addComponent(new SpriteComponent(boundaryHorizontal));

    renderSystem.addEntities({&arcade, &arcadeBoundaryLeft, &arcadeBoundaryRight, &arcadeBoundaryTop, &arcadeBoundaryDown});
    collisionSystem.addEntities({&arcadeBoundaryLeft, &arcadeBoundaryRight, &arcadeBoundaryTop, &arcadeBoundaryDown});

    ScreenCenter(arcade, new int[2]{0, 0});

    ScreenCenter(arcadeBoundaryLeft, new int[2]{-10, -4});
    ScreenCenter(arcadeBoundaryRight, new int[2]{11, -4});
    ScreenCenter(arcadeBoundaryTop, new int[2]{0, -11});
    ScreenCenter(arcadeBoundaryDown, new int[2]{0, 3});
}

void spawnFruit(Entity& fruit, int areaWidthLeft, int areaWidthRight, int areaHeightTop, int areaHeightBottom, CollisionSystem& collisionSystem)
{
    int position[2];

    std::vector<bool> wouldCollide = collisionSystem.wouldCollide(fruit, position);

    do
    {
        // Generate random position within the specified area
        position[0] = areaWidthLeft + rand() % (areaWidthRight - areaWidthLeft);
        position[1] = areaHeightTop + rand() % (areaHeightBottom - areaHeightTop);

        wouldCollide = collisionSystem.wouldCollide(fruit, position);
    }
    while(std::find(wouldCollide.begin(), wouldCollide.end(), true) != wouldCollide.end());

    // Set the fruit's position
    PositionComponent* fruitPosition = static_cast<PositionComponent*>(fruit.getComponent(typeid(PositionComponent).name()));
    fruitPosition->positionXY[0] = position[0];
    fruitPosition->positionXY[1] = position[1];
}

int main()
{
    CreateArcade();

    //Player
    Entity player("Player", 0, 0);
    std::vector<std::string> playerSprite = {"0"};
    player.addComponent(new SpriteComponent(playerSprite));

    //Entity playerNodes(20, 0);

    Entity fruit("Fruit", 0,0);
    std::vector<std::string> fruitSprite = {"*"};
    fruit.addComponent(new SpriteComponent(fruitSprite));

    ecs.addEntities({&player, &fruit});

    renderSystem.addEntities({&player, &fruit});
    collisionSystem.addEntities({&player, &fruit});
    ScreenCenter(player, new int[2]{0, 0});

    int movementDirection[2] = {0, 0};
    char input = 'n';
    char lastInput = 'n';

    bool gameOver = false;

    //ecs.listEntities();

    spawnFruit(fruit, 47, screenWidth - 46, 4, screenHeight - 13, collisionSystem);

    renderSystem.hideCursor();

    while (gameOver == false)
    {
        timeSystem.startFrame();

        inputSystem.Update();

        //GAME HERE

        /*movementSystem.MoveInput(player, 'w', new int[2]{0, -1}, collisionSystem);
        movementSystem.MoveInput(player, 's', new int[2]{0, 1}, collisionSystem);
        movementSystem.MoveInput(player, 'a', new int[2]{-1, 0}, collisionSystem);
        movementSystem.MoveInput(player, 'd', new int[2]{1, 0}, collisionSystem);*/

        if (inputSystem.getKeyState('w') == InputSystem::KEY_HELD)
        {
            lastInput = input;
            input = 'w';
        }
        else if (inputSystem.getKeyState('s') == InputSystem::KEY_HELD)
        {
            lastInput = input;
            input = 's';
        }
        else if (inputSystem.getKeyState('a') == InputSystem::KEY_HELD)
        {
            lastInput = input;
            input = 'a';
        }
        else if (inputSystem.getKeyState('d') == InputSystem::KEY_HELD)
        {
            lastInput = input;
            input = 'd';
        }

// Reset the movement direction
        movementDirection[0] = 0;
        movementDirection[1] = 0;

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

        std::vector<int> triggerObjectsID = {arcadeBoundaryLeft.getId(), arcadeBoundaryRight.getId(), arcadeBoundaryTop.getId(), arcadeBoundaryDown.getId(), fruit.getId()};

        std::vector<bool> triggers = movementSystem.Move(player, movementDirection, collisionSystem, triggerObjectsID);

        for (const auto &trigger : triggers)
        {
            std::cout << trigger << ' ';
        }

        if(triggers[fruit.getId()] == true)
        {
            spawnFruit(fruit, 47, screenWidth - 48, 4, screenHeight - 13, collisionSystem);
        }

        if(triggers[arcadeBoundaryLeft.getId()] || triggers[arcadeBoundaryRight.getId()] ||
           triggers[arcadeBoundaryTop.getId()] || triggers[arcadeBoundaryDown.getId()])
        {
            gameOver = true;
        }

        renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    renderSystem.showCursor();

    return 0;
}
