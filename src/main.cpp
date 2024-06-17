#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string>

#include "Engine/Systems/RenderSystem.h"
#include "Engine/Systems/TimeSystem.h"
#include "Engine/Systems/InputSystem.h"
#include "Engine/Systems/MovementSystem.h"
#include "Engine/Systems/CollisionSystem.h"

#include "Engine/ECS.h"
#include "Engine/Components/UIComponent.h"

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
    int entityWidth = 0;
    int entityHeight = 0;

    if (entity.hasComponent<SpriteComponent>()) {
        SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));
        entityWidth = sprite->getWidth();
        entityHeight = sprite->getHeight();
    } else if (entity.hasComponent<UIComponent>()) {
        UIComponent* uiComponent = static_cast<UIComponent*>(entity.getComponent(typeid(UIComponent).name()));
        entityWidth = uiComponent->getContent().size();
        entityHeight = 1;  // Assuming the UIComponent's content is a single line
    }

    entityPosition->positionXY[0] = (screenWidth - entityWidth) / 2 + offset[0];
    entityPosition->positionXY[1] = (screenHeight - entityHeight) / 2 + offset[1];
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

    Entity score("Score", 0, 0);
    score.addComponent(new UIComponent("Score: 0", 47));

    int scoreCount = 0;

    ecs.addEntities({&player, &fruit});
    renderSystem.addEntities({&player, &fruit, &score});
    collisionSystem.addEntities({&player, &fruit});
    ScreenCenter(player, {0, 0});
    ScreenCenter(score, {35, 0});

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

            scoreCount++;

            // Update score UI
            UIComponent* scoreUI = static_cast<UIComponent*>(score.getComponent(typeid(UIComponent).name()));
            scoreUI->setContent("Score: " + std::to_string(scoreCount));

            if(playerNodes.empty())
            {
                playerNodes.push_back(CreatePlayerNode(lastPlayerPosition));
            }
            else if(playerNodes.size() == 1)
            {
                playerNodes.push_back(CreatePlayerNode(nodeLastPositions.back()));
                playerNodes.push_back(CreatePlayerNode(nodeLastPositions.back()));
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

        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));
        //renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));
        //renderSystem.renderUI();

        timeSystem.endFrame();
    }
}

Entity* spawnEnemy(int screenOffset[2], int enemyType)
{
    Entity* enemy = new Entity("Enemy", 0, 0);

    std::vector<std::string> enemySprite1 = {"[**]"};
    std::vector<std::string> enemySprite2 = {"[&]"};
    std::vector<std::string> enemySprite3 = {"[#_#]"};

    if(enemyType == 1)
    {
        enemy->addComponent(new SpriteComponent(enemySprite1));
    }
    else if(enemyType == 2)
    {
        enemy->addComponent(new SpriteComponent(enemySprite2));
    }
    else
    {
        enemy->addComponent(new SpriteComponent(enemySprite3));
    }

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

    void moveBullet(std::vector<std::vector<Entity*>> &enemies, int& enemySpeed, int& enemySpeedIncrease)
    {
        if (!bulletActive) return;

        PositionComponent* bulletPosition = static_cast<PositionComponent*>(bulletEntity->getComponent(typeid(PositionComponent).name()));
        int movementDirection[2] = {0, -1}; // Move up

        std::vector<int> triggerObjectsID = {arcadeBoundaryTop.getId()};

        for (int i = 0; i < enemies.size(); ++i)
        {
            for (int j = 0; j < enemies[i].size(); ++j)
            {
                triggerObjectsID.push_back(enemies[i][j]->getId());
            }
        }

        std::vector<bool> triggers = movementSystem.Move(*bulletEntity, movementDirection, collisionSystem, triggerObjectsID);

        for (int i = 0; i < enemies.size(); ++i)
        {
            for (int j = 0; j < enemies[i].size(); ++j)
            {
                if(triggers[enemies[i][j]->getId()])
                {
                    ecs.removeEntity(enemies[i][j]);
                    renderSystem.removeEntity(enemies[i][j]);
                    collisionSystem.removeEntity(enemies[i][j]);

                    enemies[i].erase(enemies[i].begin() + j);

                    deactivateBullet();

                    if(enemySpeed > 1)
                    {
                        enemySpeed -= enemySpeedIncrease;
                    }
                }
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

void MoveEnemies(std::vector<Entity*>& rowEnemies, int enemiesMovementDirection[], int& enemyMoveCounter, int& enemySpeed, bool& gameOver, Entity& player)
{
    enemyMoveCounter++;

    if (enemyMoveCounter >= enemySpeed) {
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

            if (enemyTriggers[player.getId()] || enemyTriggers[arcadeBoundaryDown.getId()])
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

    ecs.addEntities(&player);
    renderSystem.addEntities(&player);
    collisionSystem.addEntities(&player);
    ScreenCenter(player, {0, 2});

    int numberOfEnemies = 6;
    //std::vector<Entity*> rowEnemies;
    std::vector<Entity*> rowEnemies1;
    std::vector<Entity*> rowEnemies2;
    std::vector<Entity*> rowEnemies3;
    int enemiesMovementDirection1[2] = {-1, 0};
    int enemiesMovementDirection2[2] = {1, 0};
    int enemiesMovementDirection3[2] = {-1, 0};
    int enemyMoveCounter = 0;
    int enemySpeed = 10;
    int enemySpeedIncrease = 1;

    for (int i = 0; i < 3; ++i)
    {
        rowEnemies1.push_back(spawnEnemy(new int[2]{6 - (i * 6), -10}, 1));
    }

    for (int i = 0; i < 4; ++i)
    {
        rowEnemies2.push_back(spawnEnemy(new int[2]{6 - (i * 4), -8}, 2));
    }

    for (int i = 0; i < 2; ++i)
    {
        rowEnemies3.push_back(spawnEnemy(new int[2]{6 - (i * 12), -6}, 3));
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
        MoveEnemies(rowEnemies1, enemiesMovementDirection1, enemyMoveCounter, enemySpeed, gameOver, player);
        MoveEnemies(rowEnemies2, enemiesMovementDirection2, enemyMoveCounter, enemySpeed, gameOver, player);
        MoveEnemies(rowEnemies3, enemiesMovementDirection3, enemyMoveCounter, enemySpeed, gameOver, player);

        std::vector<std::vector<Entity*>> enemies = {rowEnemies1, rowEnemies2, rowEnemies3};
        bullet.moveBullet(enemies, enemySpeed, enemySpeedIncrease);

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    if(gameOver)
    {
        std::cout << "Game Over!" << std::endl;

        timeSystem.sleepFor(std::chrono::milliseconds(2000));
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