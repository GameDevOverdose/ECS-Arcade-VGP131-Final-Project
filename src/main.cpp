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

void SwitchScene()
{
    ecs.removeAllEntities();
    renderSystem.removeAllEntities();
    collisionSystem.removeAllEntities();
}

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
            "| |  __                | |",
            "| | [__]  __      O    | |",
            "| |      [__]    / \\   | |",
            "\\ |_____________/___\\__| /",
            "|                        |",
            "|     ______________     |",
            "|    |          ___ |    |",
            "|    | INSERT-> |_| |    |",
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

bool arcadesCompleted[3] = {0, 0, 0};

void HubScene(int arr[2]);

void EndingCutscene();

Entity* GameState(std::string content, std::array<int, 2> offset)
{
    Entity* gameStateUI = new Entity("GameStateUI", 0, 0);
    gameStateUI->addComponent(new UIComponent(content, 47));

    ScreenCenter(*gameStateUI, offset);

    return gameStateUI;
}

bool ExitArcadeGame()
{
    if(inputSystem.getKeyState('q') == InputSystem::KEY_DOWN)
    {
        return true;
    }
}

Entity* ETSpawnEnemy(int screenOffset[2], int enemyType)
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

    void moveBullet(std::vector<std::vector<Entity*>> &enemies, int& enemyCount, int& enemySpeed, int& enemySpeedIncrease)
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

                    enemyCount--;
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

void ETHandlePlayerInput(Entity& player, Bullet& bullet)
{
    std::vector<int> triggerObjectsID = {};
    std::vector<bool> triggers;
    movementSystem.MoveInput(player, 'a', new int[2] {-1, 0}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_HELD);
    movementSystem.MoveInput(player, 'd', new int[2] {1, 0}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_HELD);

    if (inputSystem.getKeyState('b') == InputSystem::KEY_DOWN && !bullet.bulletActive)
    {
        PositionComponent* playerPosition = static_cast<PositionComponent*>(player.getComponent(typeid(PositionComponent).name()));
        bullet.createBullet(playerPosition->positionXY[0] + 1, playerPosition->positionXY[1] - 1);
    }
}

void ETMoveEnemies(std::vector<Entity*>& rowEnemies, int enemiesMovementDirection[], int& enemyMoveCounter, int& enemySpeed, bool& gameOver, Entity& player)
{
    enemyMoveCounter++;

    if (enemyMoveCounter >= enemySpeed) {
        for (int i = 0; i < rowEnemies.size(); ++i)
        {
            PositionComponent* enemyPosition = static_cast<PositionComponent*>(rowEnemies[i]->getComponent(typeid(PositionComponent).name()));
            std::vector<int> triggerObjectsID = {arcadeBoundaryLeft.getId(), arcadeBoundaryRight.getId()};
            std::vector<bool> enemyTriggers;

            if(enemiesMovementDirection[0] == -1)
            {
                Entity lastEnemy = *rowEnemies.back();

                PositionComponent* lastEnemyPosition = static_cast<PositionComponent*>(lastEnemy.getComponent(typeid(PositionComponent).name()));
                int lastEnemyPredictedPosition[2] = {lastEnemyPosition->positionXY[0], lastEnemyPosition->positionXY[1]};

                enemyTriggers = collisionSystem.wouldCollide(lastEnemy, lastEnemyPredictedPosition);
            }
            else
            {
                Entity firstEnemy = *rowEnemies.front();

                PositionComponent* firstEnemyPosition = static_cast<PositionComponent*>(firstEnemy.getComponent(typeid(PositionComponent).name()));
                int firstEnemyPredictedPosition[2] = {firstEnemyPosition->positionXY[0], firstEnemyPosition->positionXY[1]};

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

            std::vector<bool> gameOverTrigger = collisionSystem.wouldCollide(*rowEnemies[i], new int[2] {enemyPosition->positionXY[0], enemyPosition->positionXY[1]});

            if (gameOverTrigger[player.getId()] || gameOverTrigger[arcadeBoundaryDown.getId()])
            {
                gameOver = true;
            }
        }
        enemyMoveCounter = 0;
    }
}

void ETGameScene()
{
    // Player entity
    Entity player("Player", 0, 0);
    std::vector<std::string> playerSprite = {"[-]"};
    player.addComponent(new SpriteComponent(playerSprite));

    Entity etGameTitle("ETGameTitle", 0, 0);
    std::vector<std::string> etTitleSprite = {
            " _____ ",
            "|  ___|",
            "| |__  ",
            "|  __| ",
            "| |___ ",
            "\\____/ X T R A",
            "T E R R E S T R I A L"
    };
    etGameTitle.addComponent(new SpriteComponent(etTitleSprite));

    //ScoreUI
    Entity enemyCounter("EnemyCounter", 0, 0);
    enemyCounter.addComponent(new UIComponent("Score: 0", 47));

    int enemyCount = 0;

    //Controls UI
    Entity controlsUI("Controls UI", 0, 0);
    controlsUI.addComponent(new UIComponent("Controls:", 47));
    Entity moveControlsUI("Move Controls UI", 0, 0);
    moveControlsUI.addComponent(new UIComponent("Move: A (Left) / D (Right)", 47));
    Entity shootControlsUI("Shoot Controls UI ", 0, 0);
    shootControlsUI.addComponent(new UIComponent("Shoot: B (Shoot)", 47));

    ScreenCenter(enemyCounter, {35, -2});

    ScreenCenter(controlsUI, {36, 0});
    ScreenCenter(moveControlsUI, {36, 2});
    ScreenCenter(shootControlsUI, {36, 3});

    ScreenCenter(etGameTitle, {-45, 0});

    ecs.addEntities({&player, &enemyCounter, &controlsUI, &moveControlsUI, &shootControlsUI, &etGameTitle});
    renderSystem.addEntities({&player, &enemyCounter, &controlsUI, &moveControlsUI, &shootControlsUI, &etGameTitle});
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
        rowEnemies1.push_back(ETSpawnEnemy(new int[2]{6 - (i * 6), -10}, 1));

        enemyCount++;
    }

    for (int i = 0; i < 4; ++i)
    {
        rowEnemies2.push_back(ETSpawnEnemy(new int[2]{6 - (i * 4), -8}, 2));

        enemyCount++;
    }

    for (int i = 0; i < 2; ++i)
    {
        rowEnemies3.push_back(ETSpawnEnemy(new int[2]{6 - (i * 12), -6}, 3));

        enemyCount++;
    }

    bool gameOver = false;

    Bullet bullet;

    int exitPosition[2] = {11, 17};

    renderSystem.hideCursor();

    while (!gameOver)
    {
        timeSystem.startFrame();
        inputSystem.Update();

        if(ExitArcadeGame())
            break;

        // Handle player input and shooting
        ETHandlePlayerInput(player, bullet);

        // Move enemies
        ETMoveEnemies(rowEnemies1, enemiesMovementDirection1, enemyMoveCounter, enemySpeed, gameOver, player);
        ETMoveEnemies(rowEnemies2, enemiesMovementDirection2, enemyMoveCounter, enemySpeed, gameOver, player);
        ETMoveEnemies(rowEnemies3, enemiesMovementDirection3, enemyMoveCounter, enemySpeed, gameOver, player);

        if(enemyCount == 0)
        {
            arcadesCompleted[0] = true;

            renderSystem.addEntities(GameState("GAME WON!", {0, -4}));

            renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

            timeSystem.sleepFor(std::chrono::milliseconds(2000));

            SwitchScene();

            break;
        }

        // Update score UI
        UIComponent* countUI = static_cast<UIComponent*>(enemyCounter.getComponent(typeid(UIComponent).name()));
        countUI->setContent("Enemies: " + std::to_string(enemyCount));

        std::vector<std::vector<Entity*>> enemies = {rowEnemies1, rowEnemies2, rowEnemies3};
        bullet.moveBullet(enemies, enemyCount, enemySpeed, enemySpeedIncrease);

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    if(gameOver)
    {
        renderSystem.addEntities(GameState("GAME OVER", {0, -8}));

        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

        timeSystem.sleepFor(std::chrono::milliseconds(2000));
    }

    SwitchScene();

    HubScene(exitPosition);
}

void CroakHandlePlayerInput(Entity& player, bool &reachedTop)
{
    std::vector<int> triggerObjectsID = {arcadeBoundaryTop.getId()};
    std::vector<bool> triggers;
    triggers.resize(ecs.getHighestEntityId() + 3);

    movementSystem.MoveInput(player, 'w', new int[2] {0, -1}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_DOWN);
    movementSystem.MoveInput(player, 'w', new int[2] {0, -1}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_HELD);
    movementSystem.MoveInput(player, 'a', new int[2] {-3, 0}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_DOWN);
    movementSystem.MoveInput(player, 's', new int[2] {0, 1}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_DOWN);
    movementSystem.MoveInput(player, 'd', new int[2] {3, 0}, collisionSystem, triggerObjectsID, triggers, InputSystem::KEY_DOWN);

    if(triggers[arcadeBoundaryTop.getId()])
    {
        reachedTop = true;
    }
}

void MoveEntityInDirection(Entity& entity, Entity& player, bool& gameOver, int direction[2], int spawnPoint[2])
{
    ecs.addEntities(&entity);
    renderSystem.addEntities(&entity);
    collisionSystem.addEntities(&entity);

    // Get the PositionComponent of the entity
    PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
    int originalPosition[2] = {position->positionXY[0], position->positionXY[1]};

    // Check if the entity would collide with the left or right boundary or the player
    std::vector<bool> collisionVector = movementSystem.Move(entity, direction, collisionSystem, {arcadeBoundaryLeft.getId(), arcadeBoundaryRight.getId(), player.getId()});

    if (collisionVector[arcadeBoundaryLeft.getId()] || collisionVector[arcadeBoundaryRight.getId()])
    {
        // If a collision is detected with the boundary, reset the entity's position to its original spot
        const std::array<int, 2>& spawn = {spawnPoint[0], spawnPoint[1]};

        ScreenCenter(entity, spawn);
        position->positionXY[1] = originalPosition[1];
    }
    else if (collisionVector[player.getId()])
    {
        // If a collision is detected with the player, set gameOver to true
        gameOver = true;
    }
}

void CroakGameScene()
{
    // Player entity
    Entity player("Player", 0, 0);
    std::vector<std::string> playerSprite = {"^(_)^"};
    player.addComponent(new SpriteComponent(playerSprite));

    PositionComponent* playerPosition = static_cast<PositionComponent*>(player.getComponent(typeid(PositionComponent).name()));

    int leftSpawn[2] = {-6, 0};
    int rightSpawn[2] = {7, 0};

    Entity bike("Bike", 0, 0);
    std::vector<std::string> bikeSprite = {"0|-]0"};
    bike.addComponent(new SpriteComponent(bikeSprite));
    ScreenCenter(bike, {-6, 0});

    Entity bike2("Bike2", 0, 0);
    bike2.addComponent(new SpriteComponent(bikeSprite));
    ScreenCenter(bike2, {-6, -6});

    Entity bike3("Bike3", 0, 0);
    bike3.addComponent(new SpriteComponent(bikeSprite));
    ScreenCenter(bike3, {-6, -8});

    Entity car("Car", 0, 0);
    std::vector<std::string> carSprite = {"0[)-(]0",
                                          "0[)-(]0"};
    car.addComponent(new SpriteComponent(carSprite));
    ScreenCenter(car, {6, -4});

    Entity croakGameTitle("CroakGameTitle", 0, 0);
    std::vector<std::string> croakTitleSprite = {
            "  ____ ",
            " / ___|",
            "| |    ",
            "| |___    O",
            " \\____| R   A  K"
    };
    croakGameTitle.addComponent(new SpriteComponent(croakTitleSprite));

    Entity rowCounter("RowCounter", 0, 0);
    rowCounter.addComponent(new UIComponent("Rows: 0", 47));

    int rowCount = 0;

    //Controls UI
    Entity controlsUI("Controls UI", 0, 0);
    controlsUI.addComponent(new UIComponent("Controls:", 47));
    Entity moveControlsUI_1("Move Controls UI-1", 0, 0);
    moveControlsUI_1.addComponent(new UIComponent("Move: A (Left) / D (Right)", 47));
    Entity moveControlsUI_2("Move Controls UI-2", 0, 0);
    moveControlsUI_2.addComponent(new UIComponent("W (Forward) / S (Backward)", 47));

    ScreenCenter(rowCounter, {36, -3});

    ScreenCenter(controlsUI, {37, -1});
    ScreenCenter(moveControlsUI_1, {37, 1});
    ScreenCenter(moveControlsUI_2, {37, 2});

    ScreenCenter(croakGameTitle, {-45, 0});

    ecs.addEntities({&player, &rowCounter, &controlsUI, &moveControlsUI_1, &moveControlsUI_2, &croakGameTitle});
    renderSystem.addEntities({&player, &rowCounter, &controlsUI, &moveControlsUI_1, &moveControlsUI_2, &croakGameTitle});
    collisionSystem.addEntities(&player);
    ScreenCenter(player, {0, 2});

    std::vector<Entity*> rowEnemies;
    int enemySpeed = 10;

    bool gameOver = false;
    bool reachedTop = false;

    int exitPosition[2] = {32, 17};

    renderSystem.hideCursor();

    while (!gameOver)
    {
        timeSystem.startFrame();
        inputSystem.Update();

        if(ExitArcadeGame())
            break;

        ExitArcadeGame();

        // Handle player input and shooting
        CroakHandlePlayerInput(player, reachedTop);

        if(true)
        {
            int frameCounter = 0;

            if (frameCounter % 20 == 0)
            {
                MoveEntityInDirection(bike, player, gameOver, new int[2] {1, 0}, leftSpawn);
            }

            frameCounter++;
        }

        if(true)
        {
            int frameCounter = 0;

            if (frameCounter % 10 == 0)
            {
                MoveEntityInDirection(car, player, gameOver, new int[2] {-1, 0}, rightSpawn);
            }

            frameCounter++;
        }

        if(true)
        {
            int frameCounter = 0;

            if (frameCounter % 20 == 0)
            {
                MoveEntityInDirection(bike2, player, gameOver, new int[2] {-1, 0}, rightSpawn);
            }

            frameCounter++;
        }

        if(true)
        {
            int frameCounter = 0;

            if (frameCounter % 20 == 0)
            {
                MoveEntityInDirection(bike3, player, gameOver, new int[2] {1, 0}, leftSpawn);
            }

            frameCounter++;
        }

        if(reachedTop)
        {
            arcadesCompleted[1] = true;

            renderSystem.addEntities(GameState("GAME WON!", {0, -4}));

            renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

            timeSystem.sleepFor(std::chrono::milliseconds(2000));

            SwitchScene();

            break;
        }

        rowCount = playerPosition->positionXY[1] - 4;

        // Update score UI
        UIComponent* countUI = static_cast<UIComponent*>(rowCounter.getComponent(typeid(UIComponent).name()));
        countUI->setContent("Rows: " + std::to_string(rowCount));

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

        timeSystem.endFrame();
    }

    if(gameOver)
    {
        renderSystem.addEntities(GameState("GAME OVER", {0, -8}));

        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

        timeSystem.sleepFor(std::chrono::milliseconds(2000));
    }

    SwitchScene();

    HubScene(exitPosition);
}

void SerpentSpawnFruit(Entity& fruit, int areaWidthLeft, int areaWidthRight, int areaHeightTop, int areaHeightBottom, CollisionSystem& collisionSystem)
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

Entity* SerpentCreatePlayerNode(const std::array<int, 2>& nextNodesPosition)
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

void SerpentGameScene()
{
    // Player entity
    Entity player("Player", 0, 0);
    std::vector<std::string> playerSprite = {"0"};
    player.addComponent(new SpriteComponent(playerSprite));

    ScreenCenter(player, {0, 0});

    std::vector<Entity*> playerNodes;
    std::vector<std::array<int, 2>> nodeLastPositions;

    Entity fruit("Fruit", 0, 0);
    std::vector<std::string> fruitSprite = {"*"};
    fruit.addComponent(new SpriteComponent(fruitSprite));

    Entity serpentGameTitle("SerpentGameTitle", 0, 0);
    std::vector<std::string> serpentTitleSprite = {
            "  _____",
            " / ____|",
            "| (___",
            " \\___ \\",
            " ____) |",
            "|_____/",
            "   E   ",
            "    R  ",
            "   P   ",
            "  E    ",
            "   N   ",
            "  T    "
    };
    serpentGameTitle.addComponent(new SpriteComponent(serpentTitleSprite));

    Entity score("Score", 0, 0);
    score.addComponent(new UIComponent("Score: 0", 47));

    int scoreCount = 0;

    //Controls UI
    Entity controlsUI("Controls UI", 0, 0);
    controlsUI.addComponent(new UIComponent("Controls:", 47));
    Entity moveControlsUI_1("Move Controls UI-1", 0, 0);
    moveControlsUI_1.addComponent(new UIComponent("Move: A (Left) / D (Right)", 47));
    Entity moveControlsUI_2("Move Controls UI-2", 0, 0);
    moveControlsUI_2.addComponent(new UIComponent("W (Up) / S (Down)", 47));

    ScreenCenter(score, {35, -2});

    ScreenCenter(controlsUI, {36, 0});
    ScreenCenter(moveControlsUI_1, {37, 2});
    ScreenCenter(moveControlsUI_2, {37, 3});

    ScreenCenter(serpentGameTitle, {-45, 0});

    ecs.addEntities({&player, &fruit, &score, &controlsUI, &moveControlsUI_1, &moveControlsUI_2, &serpentGameTitle});
    renderSystem.addEntities({&player, &fruit, &score, &controlsUI, &moveControlsUI_1, &moveControlsUI_2, &serpentGameTitle});
    collisionSystem.addEntities({&player, &fruit});

    std::array<int, 2> movementDirection = {0, 0};
    char input = 'n';
    char lastInput = 'n';

    bool gameOver = false;

    SerpentSpawnFruit(fruit, 47, screenWidth - 46, 4, screenHeight - 13, collisionSystem);

    int exitPosition[2] = {53, 17};

    renderSystem.hideCursor();

    while (!gameOver)
    {
        timeSystem.startFrame();
        inputSystem.Update();

        if(ExitArcadeGame())
            break;

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
            SerpentSpawnFruit(fruit, 51, screenWidth - 55, 4, screenHeight - 13, collisionSystem);

            scoreCount++;

            // Update score UI
            UIComponent* scoreUI = static_cast<UIComponent*>(score.getComponent(typeid(UIComponent).name()));
            scoreUI->setContent("Score: " + std::to_string(scoreCount));

            if(playerNodes.empty())
            {
                playerNodes.push_back(SerpentCreatePlayerNode(lastPlayerPosition));
            }
            else if(playerNodes.size() == 1)
            {
                playerNodes.push_back(SerpentCreatePlayerNode(nodeLastPositions.back()));
                playerNodes.push_back(SerpentCreatePlayerNode(nodeLastPositions.back()));
            }
            else
            {
                playerNodes.push_back(SerpentCreatePlayerNode(nodeLastPositions.back()));
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

        if(scoreCount > 259)
        {
            arcadesCompleted[2] = true;

            renderSystem.addEntities(GameState("GAME WON!", {0, -4}));

            renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));

            timeSystem.sleepFor(std::chrono::milliseconds(2000));

            SwitchScene();

            break;
        }

        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 1, (screenHeight - 1));
        //renderSystem.RenderScreen(screenWidth, screenHeight, 47, (screenWidth - 46), 1, (screenHeight - 1));
        //renderSystem.renderUI();

        timeSystem.endFrame();
    }

    SwitchScene();

    HubScene(exitPosition);
}

void PlayerInput(Entity& player, bool &movingRight, std::vector<int> triggerObjectsID, bool &onDialogueTrigger, bool firstCutsceneOver, Entity *insertCoinUI, Entity *doorTrigger, std::vector<bool> &triggers)
{
    std::vector<bool> collisions;
    collisions.resize(ecs.getHighestEntityId() + 3);

    SpriteComponent* sprite = static_cast<SpriteComponent*>(player.getComponent(typeid(SpriteComponent).name()));

    if (inputSystem.getKeyState('a') == InputSystem::KEY_HELD && movingRight == true)
    {
        sprite->flipSpriteHorizontally();
        movingRight = false;
    }
    else if (inputSystem.getKeyState('d') == InputSystem::KEY_HELD && movingRight == false)
    {
        sprite->flipSpriteHorizontally();
        movingRight = true;
    }

    movementSystem.MoveInput(player, 'a', new int[2] {-3, 0}, collisionSystem, triggerObjectsID, collisions, InputSystem::KEY_HELD);
    movementSystem.MoveInput(player, 'd', new int[2] {3, 0}, collisionSystem, triggerObjectsID, collisions, InputSystem::KEY_HELD);

    PositionComponent* playerPosition = static_cast<PositionComponent*>(player.getComponent(typeid(PositionComponent).name()));
    triggers = collisionSystem.wouldCollide(player, new int[2] {playerPosition->positionXY[0], playerPosition->positionXY[1]});

    if(!triggers.empty())
    {
        if(firstCutsceneOver)
        {
            if(inputSystem.getKeyState('c') == InputSystem::KEY_DOWN)
            {
                if (triggers[triggerObjectsID[0]])
                {
                    ecs.removeEntity(doorTrigger);
                    collisionSystem.removeEntity(doorTrigger);

                    SwitchScene();

                    CreateArcade();
                    ETGameScene();
                }
                else if (triggers[triggerObjectsID[1]])
                {
                    SwitchScene();

                    CreateArcade();
                    CroakGameScene();
                }
                else if (triggers[triggerObjectsID[2]])
                {
                    SwitchScene();

                    CreateArcade();
                    SerpentGameScene();
                }
            }

            PositionComponent* insertCoinUIPosition = static_cast<PositionComponent*>(insertCoinUI->getComponent(typeid(PositionComponent).name()));

            if (triggers[triggerObjectsID[0]])
            {
                ecs.addEntities(insertCoinUI);
                renderSystem.addEntities(insertCoinUI);

                insertCoinUIPosition->positionXY[0] = 7;
                insertCoinUIPosition->positionXY[1] = 12;
            }
            else if (triggers[triggerObjectsID[1]])
            {
                ecs.addEntities(insertCoinUI);
                renderSystem.addEntities(insertCoinUI);

                insertCoinUIPosition->positionXY[0] = 28;
                insertCoinUIPosition->positionXY[1] = 12;
            }
            else if (triggers[triggerObjectsID[2]])
            {
                ecs.addEntities(insertCoinUI);
                renderSystem.addEntities(insertCoinUI);

                insertCoinUIPosition->positionXY[0] = 49;
                insertCoinUIPosition->positionXY[1] = 12;
            }
            else
            {
                ecs.removeEntity(insertCoinUI);
                renderSystem.removeEntity(insertCoinUI);
            }
        }
    }

    if (triggers[triggerObjectsID[3]] && movingRight)
    {
        onDialogueTrigger = true;
    }
    else
    {
        onDialogueTrigger = false;
    }
}

bool StartingCutscene(Entity &dialogueUI, bool &cutsceneActivated, int &dialogueCount)
{
    cutsceneActivated = true;

    UIComponent* uiComponent = static_cast<UIComponent*>(dialogueUI.getComponent(typeid(UIComponent).name()));
    PositionComponent* positionComponent = static_cast<PositionComponent*>(dialogueUI.getComponent(typeid(PositionComponent).name()));

    ecs.addEntities(&dialogueUI);
    renderSystem.addEntities(&dialogueUI);

    std::vector <std::string> dialogues =
    {
            "Welcome to the ECS Arcade!",
            "I'm the owner of this place.",
            "Well...",
            "My dad was, before me, but he's retired now.",
            "During his time, we only had 3 machines-",
            "that would play the same game.",
            "But due to some recent advancements-",
            "a company called JAMA or JAMMA...I think?",
            "anyways, because of that we were able to",
            "reprogram the machines to play different games.",
            "Ah the wonders of technology!",
            "Wish this stuff existed when I was your age.",
            "...",
            "You know what? Here, take this bunch of coins.",
            "Just go upto the Arcades-",
            "and press 'C' to insert the coin.",
            "Pretty sure this isn't good for business but-",
            "I never was a good businessman either.",
    };

    std::vector <std::array <int, 2>> dialogueOffset =
    {
            {43, -4},
            {43, -4},
            {45, -4},
            {36, -4},
            {38, -4},
            {43, -4},
            {40, -4},
            {38, -4},
            {38, -4},
            {35, -4},
            {44, -4},
            {36, -4},
            {45, -4},
            {35, -4},
            {45, -4},
            {42, -4},
            {36, -4},
            {39, -4},
     };

    while(dialogueCount < dialogues.size())
    {
        timeSystem.startFrame();
        inputSystem.Update();

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

        timeSystem.startFrame();
        inputSystem.Update();

        uiComponent->setContent(dialogues[dialogueCount]);

        ScreenCenter(dialogueUI, dialogueOffset[dialogueCount]);

        if(inputSystem.getKeyState('t') == InputSystem::KEY_DOWN)
        {
            dialogueCount++;
        }

        timeSystem.endFrame();
    }

    uiComponent->setContent("");

    return true;
}

void EndingCutscene(Entity &dialogueUI, bool &cutsceneActivated, int &dialogueCount)
{
    cutsceneActivated = true;

    dialogueCount = 0;

    UIComponent* uiComponent = static_cast<UIComponent*>(dialogueUI.getComponent(typeid(UIComponent).name()));
    PositionComponent* positionComponent = static_cast<PositionComponent*>(dialogueUI.getComponent(typeid(PositionComponent).name()));

    ecs.addEntities(&dialogueUI);
    renderSystem.addEntities(&dialogueUI);

    std::vector <std::string> dialogues =
            {
                    "You completed 2 of the games?",
                    "I wouldn't expect you to complete Serpent-",
                    "So that's quite impressive I must say!",
                    "...",
                    "By beating both those games you have proved-",
                    "that you're worthy.",
                    "Worthy of-",
                    "This Arcade.",
                    "You were Efficient",
                    "You were Competent",
                    "If not Skilled",
                    "But you have that Spark in you.",
                    "I have now decided-",
                    "You are my heir.",
                    "You will now Inherit the ECS Ar-",
                    "...",
                    "Wait that doesn't sound right",
            };

    std::vector <std::array <int, 2>> dialogueOffset =
            {
                    {43, -4},
                    {37, -4},
                    {39, -4},
                    {45, -4},
                    {36, -4},
                    {45, -4},
                    {45, -4},
                    {45, -4},
                    {44, -4},
                    {44, -4},
                    {44, -4},
                    {43, -4},
                    {45, -4},
                    {44, -4},
                    {41, -4},
                    {45, -4},
                    {43, -4},
            };

    if(arcadesCompleted[0] && arcadesCompleted[1] && arcadesCompleted[2])
    {
        dialogues =
                {
                        "You completed all 3 games?",
                        "I...I",
                        "I am speechless.",
                        "...",
                        "By beating those games you have proved-",
                        "that you're worthy.",
                        "Worthy of-",
                        "This Arcade.",
                        "You were Efficient",
                        "You were Competent",
                        "You Were Skilled",
                        "Seeing you play makes me ECStatic.",
                        "I have now decided-",
                        "You are my heir.",
                        "You will now Inherit the ECS Ar-",
                        "...",
                        "Wait that doesn't sound right",
                };

        dialogueOffset =
                {
                        {43, -4},
                        {45, -4},
                        {45, -4},
                        {45, -4},
                        {38, -4},
                        {45, -4},
                        {45, -4},
                        {45, -4},
                        {44, -4},
                        {44, -4},
                        {44, -4},
                        {41, -4},
                        {45, -4},
                        {44, -4},
                        {41, -4},
                        {45, -4},
                        {43, -4},
                };
    }

    while(dialogueCount < dialogues.size())
    {
        timeSystem.startFrame();
        inputSystem.Update();

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

        timeSystem.startFrame();
        inputSystem.Update();

        uiComponent->setContent(dialogues[dialogueCount]);

        ScreenCenter(dialogueUI, dialogueOffset[dialogueCount]);

        if(inputSystem.getKeyState('t') == InputSystem::KEY_DOWN)
        {
            dialogueCount++;
        }

        timeSystem.endFrame();
    }

    uiComponent->setContent("");

    SwitchScene();

    ecs.addEntities(&dialogueUI);
    renderSystem.addEntities(&dialogueUI);

    if(arcadesCompleted[0] && arcadesCompleted[1] && arcadesCompleted[2])
    {
        renderSystem.clearScreen();

        timeSystem.sleepFor(std::chrono::milliseconds(1000));

        uiComponent->setContent("THE END");
    }
    else
    {
        renderSystem.clearScreen();

        timeSystem.sleepFor(std::chrono::milliseconds(1500));

        uiComponent->setContent("GAME OVER");

        ScreenCenter(dialogueUI, {0, 0});

        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

        timeSystem.sleepFor(std::chrono::milliseconds(2000));

        uiComponent->setContent("GAME OVER?");
    }

    ScreenCenter(dialogueUI, {0, 0});

    renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

    if(arcadesCompleted[0] && arcadesCompleted[1] && arcadesCompleted[2])
    {
        timeSystem.sleepFor(std::chrono::milliseconds(3000));
    }
    else
    {
        timeSystem.sleepFor(std::chrono::milliseconds(1750));
    }
}

bool onDialogueTrigger = false;
bool cutsceneActivated = false;

bool firstCutsceneOver = false;

Entity *talkUI = GameState("Press 'T' to Talk", {24, 1});
Entity *cutsceneDialogue = GameState("", {43, -4});
int dialogueCount = 0;

void HubScene(int playerStartPos[2])
{
    // Player entity
    Entity player("Player", playerStartPos[0], playerStartPos[1]);
    std::vector<std::string> playerSprite = {
            "+-----+",
            "|  0 0|",
            "|   - |",
            "+-----+",
            "    || ",
            "-------",
            "|_____|",
            "|_____|",
            "|     |",
            "-------",
            " || | | ",
            " ||_|_|_",
            " |___|__|"
    };
    player.addComponent(new SpriteComponent(playerSprite));

    Entity shopkeeper("Shopkeeper", 100, 12);
    std::vector<std::string> shopkeeperSprite = {
            "+-------+",
            "| o o   |",
            "|  _    |",
            "+-------+",
            "  | |    ",
            "---------",
            "|\\ /\\ / |",
            "| \\\\//  |",
            "|  \\/   |",
            "|       |",
            "|       |",
            "---------",
            "  | | || ",
            "  | | ||",
            "  | | || ",
            "  | | ||",
            "  | | || ",
            " |__|_||"
    };
    shopkeeper.addComponent(new SpriteComponent(shopkeeperSprite));


    int leftSpawn[2] = {-6, 0};
    int rightSpawn[2] = {6, 0};

    Entity background("Background", 0, 0);
    std::vector<std::string> backgroundSprite = {
            " \\                                                          ",
            "  \\                                                          ",
            "   \\                                                         ",
            "    \\                                                        ",
            "     \\------------------------------------------------------------------------------------------------------------------",
            "     |                                                       ",
            "     |                                                       ",
            "     |                                                       ",
            "|\\   |                                                       ",
            "| \\  |                                                       ",
            "|  \\ |                                                       ",
            "|  | |                                                       ",
            "|  | |                                                       ",
            "|  | |                                                       ",
            "|  | |                                                       ",
            "|  | |                                                       ",
            "|  | |                                                       ",
            "|O | |                                                       ",
            "|  | |                                                       ",
            "|  | |                                                                                      ____        ",
            "|  | |                                                                                      \\   \\      ",
            "|  | |                                                                                      |\\   \\     ",
            "|  | |                                                                                      | \\___\\    ",
            "|  | |                                                                                      |  | |     ",
            "|  | |                                                                                      |  | |     ",
            "|  | /--------------------------------------------------------------------------------------|  | |----------------------",
            "|  |/                                                                                       |  | |     ",
            "|  /                                                                                        |  | |     ",
            "| /                                                                                         |\\ | |     ",
            "|/                                                                                            \\| |     ",
            "/                                                                                             \\ | |     "
    };
    background.addComponent(new SpriteComponent(backgroundSprite));

    Entity arcadesTitle("ArcadesTitle", 9, 5);
    std::vector<std::string> arcadesTitleSprite = {
            "   _____                            .___             ",
            "  /  _  \\_______   ____ _____     __| _/____   ______",
            " /  /_\\  \\_  __ \\_/ ___\\\\__  \\   / __ |/ __ \\ /  ___/",
            "/    |    \\  | \\/\\  \\___ / __ \\_/ /_/ \\  ___/ \\___ \\ ",
            "\\____|__  /__|    \\___  >____  /\\____ |\\___  >____  >",
            "        \\/            \\/     \\/      \\/    \\/     \\/ "
    };
    arcadesTitle.addComponent(new SpriteComponent(arcadesTitleSprite));

    Entity etArcade("ETArcade", 9, 13);
    std::vector<std::string> etArcadeSprite = {
            "___________",
            "| _______ |",
            "| |     | |",
            "| |     | |",
            "| |     | |",
            "| ------- |",
            "|      [] |",
            "|         |",
            "|  ,---.  |",
            "|  |---   |",
            "|  |      |",
            "|  `---'  |",
            "|         |",
    };
    etArcade.addComponent(new SpriteComponent(etArcadeSprite));

    Entity croakArcade("CroakArcade", 30, 13);
    std::vector<std::string> croakArcadeSprite = {
            "___________",
            "| _______ |",
            "| |     | |",
            "| |     | |",
            "| |     | |",
            "| ------- |",
            "|      [] |",
            "|         |",
            "|  ,---.  |",
            "|  |      |",
            "|  |      |",
            "|  `---'  |",
            "|         |",
    };
    croakArcade.addComponent(new SpriteComponent(croakArcadeSprite));

    Entity serpentArcade("SerpentArcade", 51, 13);
    std::vector<std::string> serpentArcadeSprite = {
            "___________",
            "| _______ |",
            "| |     | |",
            "| |     | |",
            "| |     | |",
            "| ------- |",
            "|      [] |",
            "|         |",
            "|  ,---.  |",
            "|  `---.  |",
            "|      |  |",
            "|  `---'  |",
            "|         |",
    };
    serpentArcade.addComponent(new SpriteComponent(serpentArcadeSprite));

    Entity doorCollider("DoorCollider", 3, 0);
    Entity ownerCollider("DoorCollider", 92, 0);
    std::vector<std::string> horizontalColliderSprite = {
            "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|", "|"
    };
    doorCollider.addComponent(new SpriteComponent(horizontalColliderSprite));
    ownerCollider.addComponent(new SpriteComponent(horizontalColliderSprite));

    Entity talkTrigger("Trigger", 82, 17);
    Entity exitTrigger("Trigger", 5, 17);
    std::vector<std::string> triggerSprite = {
            "--------", "--------", "--------", "--------", "--------", "--------", "--------", "--------", "--------"
    };
    talkTrigger.addComponent(new SpriteComponent(triggerSprite));
    exitTrigger.addComponent(new SpriteComponent(triggerSprite));

    ecs.addEntities({&player, &background, &arcadesTitle, &shopkeeper, &doorCollider, &ownerCollider, &etArcade, &croakArcade, &serpentArcade, &talkTrigger, &exitTrigger});
    renderSystem.addEntities({&background, &arcadesTitle, &etArcade, &croakArcade, &serpentArcade, &player, &shopkeeper});
    collisionSystem.addEntities({&player, &doorCollider, &ownerCollider, &etArcade, &croakArcade, &serpentArcade, &talkTrigger, &exitTrigger});

    std::vector<int> triggerObjectsID = {etArcade.getId(), croakArcade.getId(), serpentArcade.getId(), talkTrigger.getId(), exitTrigger.getId()};

    Entity *insertCoinUI = GameState("Insert Coin 'C'", {24, 1});

    bool movingRight = true;
    bool arcadesFinished = false;

    if(arcadesCompleted[0] && arcadesCompleted[1])
    {
        arcadesFinished = true;
    }

    std::vector<bool> triggers;

    renderSystem.hideCursor();

    while (true)
    {
        timeSystem.startFrame();
        inputSystem.Update();

        //std::cout << "Arcade Completed: " << arcadesCompleted[0] << " " << arcadesCompleted[1] << " " << arcadesCompleted[2] << std::endl;

        if(!cutsceneActivated)
            PlayerInput(player, movingRight, triggerObjectsID, onDialogueTrigger, firstCutsceneOver, insertCoinUI, &exitTrigger, triggers);

        if(triggers[exitTrigger.getId()])
        {
            UIComponent* uiComponent = static_cast<UIComponent*>(cutsceneDialogue->getComponent(typeid(UIComponent).name()));
            PositionComponent* positionComponent = static_cast<PositionComponent*>(cutsceneDialogue->getComponent(typeid(PositionComponent).name()));

            if (ExitArcadeGame() && !firstCutsceneOver)
            {
                ecs.removeEntity(&player);
                renderSystem.removeEntity(&player);

                ecs.addEntities(cutsceneDialogue);
                renderSystem.addEntities(cutsceneDialogue);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(2500));

                std::vector <std::string> dialogues =
                        {
                                "Did he just leave?",
                                "...",
                                "Damn, he was my only customer too"
                        };

                std::vector <std::array <int, 2>> dialogueOffset =
                        {
                                {44, -4},
                                {45, -4},
                                {41, -4},
                        };

                uiComponent->setContent(dialogues[0]);
                ScreenCenter(*cutsceneDialogue, dialogueOffset[0]);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(1500));

                uiComponent->setContent(dialogues[1]);
                ScreenCenter(*cutsceneDialogue, dialogueOffset[1]);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(3000));

                uiComponent->setContent(dialogues[2]);
                ScreenCenter(*cutsceneDialogue, dialogueOffset[2]);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(1000));

                break;
            }
            else if (ExitArcadeGame() && firstCutsceneOver)
            {
                ecs.removeEntity(&player);
                renderSystem.removeEntity(&player);

                ecs.removeEntity(insertCoinUI);
                renderSystem.removeEntity(insertCoinUI);

                ecs.addEntities(cutsceneDialogue);
                renderSystem.addEntities(cutsceneDialogue);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(2500));

                std::vector <std::string> dialogues =
                        {
                                "...",
                                "Did he just steal all my money?"
                        };

                std::vector <std::array <int, 2>> dialogueOffset =
                        {
                                {45, -4},
                                {43, -4},
                        };

                uiComponent->setContent(dialogues[0]);
                ScreenCenter(*cutsceneDialogue, dialogueOffset[0]);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(1500));

                uiComponent->setContent(dialogues[1]);
                ScreenCenter(*cutsceneDialogue, dialogueOffset[1]);

                renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

                timeSystem.sleepFor(std::chrono::milliseconds(2000));

                break;
            }
        }

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

        if(onDialogueTrigger == true)
        {
            if (inputSystem.getKeyState('t') == InputSystem::KEY_DOWN)
            {
                firstCutsceneOver = StartingCutscene(*cutsceneDialogue, cutsceneActivated, dialogueCount);
            }

            if(arcadesFinished && inputSystem.getKeyState('t') == InputSystem::KEY_DOWN)
            {
                EndingCutscene(*cutsceneDialogue, cutsceneActivated, dialogueCount);
                exit(0);
            }
        }

        if(!firstCutsceneOver || arcadesFinished)
        {
            if(onDialogueTrigger)
            {
                ecs.addEntities(talkUI);
                renderSystem.addEntities(talkUI);
            }
            else
            {
                ecs.removeEntity(talkUI);
                renderSystem.removeEntity(talkUI);
            }
        }
        else
        {
            cutsceneActivated = false;

            ecs.removeEntity(talkUI);
            renderSystem.removeEntity(talkUI);
        }

        timeSystem.endFrame();
    }
}

int main()
{
    Entity gameTitle("GameTitle", 0, 0);
    std::vector<std::string> gameTitleSprite = {
            " _______  _______  _______",
            "|       ||       ||       |",
            "|    ___||       ||  _____|",
            "|   |___ |       || |_____",
            "|    ___||      _||_____  |",
            "|   |___ |     |_  _____| |",
            "|_______||_______||_______|",
            " _______  ______    _______  _______  ______   _______",
            "|   _   ||    _ |  |       ||   _   ||      | |       |",
            "|  |_|  ||   | ||  |       ||  |_|  ||  _    ||    ___|",
            "|       ||   |_||_ |       ||       || | |   ||   |___",
            "|       ||    __  ||      _||       || |_|   ||    ___|",
            "|   _   ||   |  | ||     |_ |   _   ||       ||   |___",
            "|__| |__||___|  |_||_______||__| |__||______| |_______|"
    };
    gameTitle.addComponent(new SpriteComponent(gameTitleSprite));

    Entity enterUI("EnterUI", 0, 0);
    enterUI.addComponent(new UIComponent("Press 'E' to Enter the Arcade", 47));

    ScreenCenter(gameTitle, {-45, -2});
    ScreenCenter(enterUI, {-42, 7});

    ecs.addEntities({&gameTitle, &enterUI});
    renderSystem.addEntities({&gameTitle, &enterUI});

    renderSystem.hideCursor();

    while (true)
    {
        timeSystem.startFrame();
        inputSystem.Update();

        // Render screen
        renderSystem.RenderScreen(screenWidth, screenHeight, 0, screenWidth, 0, screenHeight);

        if(inputSystem.getKeyState('e') == InputSystem::KEY_DOWN)
        {
            SwitchScene();

            HubScene(new int[2]{7, 17});

            break;
        }

        timeSystem.endFrame();
    }

    renderSystem.showCursor();
}