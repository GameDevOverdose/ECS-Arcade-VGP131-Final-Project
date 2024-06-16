#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#include "Engine/Systems/RenderSystem.h"
#include "Engine/Systems/TimeSystem.h"
#include "Engine/Systems/InputSystem.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/CollisionSystem.h"

#include "Engine/ECS.h"

// INITIALIZATION OF SYSTEMS
int screenWidth = 120;
int screenHeight = 30;

const int targetFPS = 60;

ECS ecs;

RenderSystem renderSystem;
TimeSystem timeSystem(targetFPS);
MovementSystem movementSystem;
CollisionSystem collisionSystem(&ecs);

void ScreenCenter(Entity& entity, const std::array<int, 2>& offset)
{
    PositionComponent* entityPosition = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
    SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));

    entityPosition->positionXY[0] = (screenWidth - sprite->getWidth()) / 2 + offset[0];
    entityPosition->positionXY[1] = (screenHeight - sprite->getHeight()) / 2 + offset[1];
}

// Arcade entities
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

    ScreenCenter(arcade, {0, 0});

    ScreenCenter(arcadeBoundaryLeft, {-10, -4});
    ScreenCenter(arcadeBoundaryRight, {11, -4});
    ScreenCenter(arcadeBoundaryTop, {0, -11});
    ScreenCenter(arcadeBoundaryDown, {0, 3});
}

void spawnFruit(Entity& fruit, int areaWidthLeft, int areaWidthRight, int areaHeightTop, int areaHeightBottom, CollisionSystem& collisionSystem)
{
    std::array<int, 2> position;

    std::vector<bool> wouldCollide = collisionSystem.wouldCollide(fruit, position.data());

    do
    {
        // Generate random position within the specified area
        position[0] = areaWidthLeft + rand() % (areaWidthRight - areaWidthLeft);
        position[1] = areaHeightTop + rand() % (areaHeightBottom - areaHeightTop);

        wouldCollide = collisionSystem.wouldCollide(fruit, position.data());
    }
    while (std::find(wouldCollide.begin(), wouldCollide.end(), true) != wouldCollide.end());

    // Set the fruit's position
    PositionComponent* fruitPosition = static_cast<PositionComponent*>(fruit.getComponent(typeid(PositionComponent).name()));
    fruitPosition->positionXY[0] = position[0];
    fruitPosition->positionXY[1] = position[1];
}

Entity* CreatePlayerNode(const std::array<int, 2>& nextNodesPosition)
{
    Entity* node = new Entity("Player Node", 0, 0);

    node->addComponent(new SpriteComponent(std::vector<std::string>{"0"}));

    PositionComponent* nodePosition = static_cast<PositionComponent*>(node->getComponent(typeid(PositionComponent).name()));

    nodePosition->positionXY[0] = nextNodesPosition[0];
    nodePosition->positionXY[1] = nextNodesPosition[1];

    ecs.addEntities(node);
    renderSystem.addEntities(node);
    collisionSystem.addEntities(node);

    return node;
}

void SnakeGameMain()
{
    // Player entity
    Entity player("Player", 0, 0);
    std::vector<std::string> playerSprite = {"0"};
    player.addComponent(new SpriteComponent(playerSprite));

    std::vector<Entity*> playerNodes;
    std::vector<std::array<int, 2>> nodeLastPositions;

    Entity fruit("Fruit", 0, 0);
    std::vector<std::string> fruitSprite = {"*"};
    fruit.addComponent(new SpriteComponent(fruitSprite));

    ecs.addEntities({&player, &fruit});

    renderSystem.addEntities({&player, &fruit});
    collisionSystem.addEntities({&player, &fruit});
    ScreenCenter(player, {0, 0});

    std::array<int, 2> movementDirection = {0, 0};
    char input = 'n';
    char lastInput = 'n';

    bool gameOver = false;

    // ecs.listEntities();

    spawnFruit(fruit, 47, screenWidth - 46, 4, screenHeight - 13, collisionSystem);

    renderSystem.hideCursor();

    while (!gameOver)
    {
        timeSystem.startFrame();

        inputSystem.Update();

        // GAME LOGIC

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
        movementDirection = {0, 0};

        // Move the player in the direction of the last input direction
        switch (input)
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

        for (size_t i = 0; i < playerNodes.size(); ++i)
        {
            triggerObjectsID.push_back(playerNodes[i]->getId());
        }

        PositionComponent* playerPositionComponent = static_cast<PositionComponent*>(player.getComponent(typeid(PositionComponent).name()));

        std::array<int, 2> lastPlayerPosition = {playerPositionComponent->positionXY[0], playerPositionComponent->positionXY[1]};
        std::vector<bool> triggers = movementSystem.Move(player, movementDirection.data(), collisionSystem, triggerObjectsID);
        std::array<int, 2> currentPlayerPosition = {playerPositionComponent->positionXY[0], playerPositionComponent->positionXY[1]};

        if (triggers[fruit.getId()])
        {
            spawnFruit(fruit, 47, screenWidth - 48, 4, screenHeight - 13, collisionSystem);

            if(playerNodes.empty())
            {
                playerNodes.push_back(CreatePlayerNode(lastPlayerPosition));
            }
            else
            {
                playerNodes.push_back(CreatePlayerNode(nodeLastPositions.back()));
            }

        }

        for (size_t i = 0; i < playerNodes.size(); ++i)
        {
            PositionComponent* positionComponent = static_cast<PositionComponent*>(playerNodes[i]->getComponent(typeid(PositionComponent).name()));

            nodeLastPositions.resize(playerNodes.size());

            if (!renderSystem.hasEntity(playerNodes[i]) && !collisionSystem.hasEntity(playerNodes[i]))
            {
                renderSystem.addEntities(playerNodes[i]);
                collisionSystem.addEntities(playerNodes[i]);
                triggerObjectsID.push_back(playerNodes[i]->getId());
            }

            if (i == 0)
            {
                positionComponent->positionXY[0] = lastPlayerPosition[0];
                positionComponent->positionXY[1] = lastPlayerPosition[1];

                nodeLastPositions[i] = {lastPlayerPosition[0], lastPlayerPosition[1]};
            }
            else
            {
                PositionComponent* positionComponent = static_cast<PositionComponent*>(playerNodes[i]->getComponent(typeid(PositionComponent).name()));

                nodeLastPositions[i] = {positionComponent->positionXY[0], positionComponent->positionXY[1]};

                positionComponent->positionXY[0] = nodeLastPositions[i - 1][0];
                positionComponent->positionXY[1] = nodeLastPositions[i - 1][1];
            }

            if(triggers[playerNodes[i]->getId()])
            {
                gameOver = true;
            }

            //std::cout << "Player Node " << i << " position: (" << positionComponent->positionXY[0] << ", " << positionComponent->positionXY[1] << ")" << std::endl;
        }

        if (triggers[arcadeBoundaryLeft.getId()] || triggers[arcadeBoundaryRight.getId()] ||
            triggers[arcadeBoundaryTop.getId()] || triggers[arcadeBoundaryDown.getId()])
        {
            gameOver = true;
        }

        renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));

        timeSystem.endFrame();
    }
}

Entity* spawnEnemy(int screenOffset[2])
{
    Entity* enemy = new Entity("Enemy", 0, 0);
    std::vector<std::string> enemySprite = {"[**]"};
    enemy->addComponent(new SpriteComponent(enemySprite));

    std::array<int, 2> screenOffsetArray = {screenOffset[0], screenOffset[1]};
    ScreenCenter(*enemy, screenOffsetArray);

    ecs.addEntities(enemy);
    renderSystem.addEntities(enemy);
    collisionSystem.addEntities(enemy);

    return enemy;
}

class Bullet {
public:
    Entity* bulletEntity;
    bool bulletActive;

    Bullet() : bulletEntity(nullptr), bulletActive(false) {}

    void createBullet(int startX, int startY)
    {
        bulletEntity = new Entity("Bullet", 0, 0);
        std::vector<std::string> bulletSprite = {"|"};
        bulletEntity->addComponent(new SpriteComponent(bulletSprite));

        PositionComponent* bulletPosition = static_cast<PositionComponent*>(bulletEntity->getComponent(typeid(PositionComponent).name()));

        bulletPosition->positionXY[0] = startX;
        bulletPosition->positionXY[1] = startY;

        ecs.addEntities(bulletEntity);
        renderSystem.addEntities(bulletEntity);
        collisionSystem.addEntities(bulletEntity);
        bulletActive = true;
    }

    void moveBullet(std::vector<Entity*> &enemies)
    {
        if (!bulletActive) return;

        PositionComponent* bulletPosition = static_cast<PositionComponent*>(bulletEntity->getComponent(typeid(PositionComponent).name()));
        int movementDirection[2] = {0, -1}; // Move up

        std::vector<int> triggerObjectsID = {arcadeBoundaryTop.getId()};

        for (int i = 0; i < enemies.size(); ++i)
        {
            triggerObjectsID.push_back(enemies[i]->getId());
        }

        std::vector<bool> triggers = movementSystem.Move(*bulletEntity, movementDirection, collisionSystem, triggerObjectsID);

        for (int i = 0; i < enemies.size(); ++i)
        {
            if(triggers[enemies[i]->getId()])
            {
                ecs.removeEntity(enemies[i]);
                renderSystem.removeEntity(enemies[i]);
                collisionSystem.removeEntity(enemies[i]);

                enemies.erase(enemies.begin() + i);

                deactivateBullet();
            }
        }

        if(triggers[arcadeBoundaryTop.getId()])
        {
            deactivateBullet();
        }
    }

private:
    void deactivateBullet() {
        bulletActive = false;
        ecs.removeEntity(bulletEntity);
        renderSystem.removeEntity(bulletEntity);
        collisionSystem.removeEntity(bulletEntity);
    }
};

void HandlePlayerInput(Entity& player, Bullet& bullet)
{
    std::vector<int> triggerObjectsID = {};
    std::vector<bool> triggers;
    movementSystem.MoveInput(player, 'a', new int[2] {-1, 0}, collisionSystem, triggerObjectsID, triggers);
    movementSystem.MoveInput(player, 'd', new int[2] {1, 0}, collisionSystem, triggerObjectsID, triggers);

    if (inputSystem.getKeyState('j') == InputSystem::KEY_DOWN && !bullet.bulletActive)
    {
        PositionComponent* playerPosition = static_cast<PositionComponent*>(player.getComponent(typeid(PositionComponent).name()));
        bullet.createBullet(playerPosition->positionXY[0] + 1, playerPosition->positionXY[1] - 1);
    }
}

void MoveEnemies(std::vector<Entity*>& rowEnemies, int enemiesMovementDirection[], int& enemyMoveCounter, bool& gameOver, Entity& player)
{
    enemyMoveCounter++;

    if (enemyMoveCounter >= 5) {
        for (int i = 0; i < rowEnemies.size(); ++i)
        {
            std::vector<int> triggerObjectsID = {arcadeBoundaryLeft.getId(), arcadeBoundaryRight.getId()};
            std::vector<bool> enemyTriggers;

            if(enemiesMovementDirection[0] == -1)
            {
                Entity lastEnemy = *rowEnemies.back();

                PositionComponent* lastEnemyPosition = static_cast<PositionComponent*>(lastEnemy.getComponent(typeid(PositionComponent).name()));
                int lastEnemyPredictedPosition[2] = {lastEnemyPosition->positionXY[0] - 1, lastEnemyPosition->positionXY[1]};

                enemyTriggers = collisionSystem.wouldCollide(lastEnemy, lastEnemyPredictedPosition);
            }
            else
            {
                Entity firstEnemy = *rowEnemies.front();

                PositionComponent* firstEnemyPosition = static_cast<PositionComponent*>(firstEnemy.getComponent(typeid(PositionComponent).name()));
                int firstEnemyPredictedPosition[2] = {firstEnemyPosition->positionXY[0] + 1, firstEnemyPosition->positionXY[1]};

                enemyTriggers = collisionSystem.wouldCollide(firstEnemy, firstEnemyPredictedPosition);
            }

            movementSystem.Move(*rowEnemies[i], enemiesMovementDirection, collisionSystem, triggerObjectsID);

            if (enemyTriggers[arcadeBoundaryLeft.getId()] || enemyTriggers[arcadeBoundaryRight.getId()])
            {
                enemiesMovementDirection[0] *= -1;

                for (int j = 0; j < rowEnemies.size(); ++j)
                {
                    PositionComponent* enemyPosition = static_cast<PositionComponent*>(rowEnemies[j]->getComponent(typeid(PositionComponent).name()));
                    enemyPosition->positionXY[1] += 1;
                }
            }

            if (enemyTriggers[player.getId()])
            {
                gameOver = true;
            }
        }
        enemyMoveCounter = 0;
    }
}

void ETGameMain()
{
    // Player entity
    Entity player("Player", 0, 0);
    std::vector<std::string> playerSprite = {"[-]"};
    player.addComponent(new SpriteComponent(playerSprite));

    Entity fruit("Fruit", 0, 0);
    std::vector<std::string> fruitSprite = {"*"};
    fruit.addComponent(new SpriteComponent(fruitSprite));

    ecs.addEntities({&player});
    renderSystem.addEntities({&player, &fruit});
    collisionSystem.addEntities({&player, &fruit});
    ScreenCenter(player, {0, 2});

    int numberOfEnemies = 6;
    std::vector<Entity*> rowEnemies;
    int enemiesMovementDirection[2] = {-1, 0};
    int enemyMoveCounter = 0;

    for (int i = 0; i < numberOfEnemies / 2; ++i)
    {
        rowEnemies.push_back(spawnEnemy(new int[2]{6 - (i * 6), -10}));
    }

    ecs.listEntities();

    bool gameOver = false;
    renderSystem.hideCursor();

    Bullet bullet;

    while (!gameOver)
    {
        timeSystem.startFrame();
        inputSystem.Update();

        // Handle player input and shooting
        HandlePlayerInput(player, bullet);

        // Move enemies
        MoveEnemies(rowEnemies, enemiesMovementDirection, enemyMoveCounter, gameOver, player);

        // Move bullet and check collisions
        bullet.moveBullet(rowEnemies);

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    renderSystem.showCursor();
}

int main()
{
    CreateArcade();

    SnakeGameMain();
    //ETGameMain();

    return 0;
}