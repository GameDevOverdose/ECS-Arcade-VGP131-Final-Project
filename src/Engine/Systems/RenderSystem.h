#pragma once

#include <Windows.h> // for Sleep

#include "../Entity.h"
#include "System.h"
#include "../Components/PositionComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/UIComponent.h"

class RenderSystem : public System {
public:
    std::vector<Entity*> entitiesToRender;

    void addEntities(std::vector <Entity*> entity)
    {
        for (Entity* e : entity)
        {
            entitiesToRender.push_back(e);
        }
    }

    void addEntities(Entity *entity)
    {
        entitiesToRender.push_back(entity);
    }

    void removeEntities(std::vector<Entity*> *entityList)
    {
        for (Entity* e : *entityList)
        {
            removeEntity(e);
        }
    }

    void removeEntity(Entity* entity)
    {
        entitiesToRender.erase(std::remove(entitiesToRender.begin(), entitiesToRender.end(), entity), entitiesToRender.end());
    }

    bool hasEntity(Entity* entity)
    {
        return std::find(entitiesToRender.begin(), entitiesToRender.end(), entity) != entitiesToRender.end();
    }

    void hideCursor()
    {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(hOut, &cursorInfo);
    #else
        std::cout << "\033[?25l";
    #endif
    }

    void showCursor()
    {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(hOut, &cursorInfo);
    #else
        std::cout << "\033[?25h";
    #endif
    }

    void clearScreen()
    {
        system("cls");
    }

    void RenderScreen(int screenWidth, int screenHeight, int screenWidthLeft, int screenWidthRight, int screenHeightUpper, int screenHeightLower)
    {
        int width = screenWidthRight - screenWidthLeft;
        int height = screenHeightLower - screenHeightUpper;

        std::vector<std::vector<char>> previousScreen(height, std::vector<char>(width, ' '));

        RenderFrame(screenWidth, screenHeight, screenWidthLeft, screenWidthRight, screenHeightUpper, screenHeightLower, previousScreen);
    }

    void RenderFrame(int screenWidth, int screenHeight, int screenWidthLeft, int screenWidthRight, int screenHeightUpper, int screenHeightLower, std::vector<std::vector<char>> previousScreen)
    {
        int width = screenWidthRight - screenWidthLeft;
        int height = screenHeightLower - screenHeightUpper;

        std::vector<std::vector<char>> screen(height, std::vector<char>(width, ' '));

        // Add border for debugging
        /*for (int i = 0; i < width; ++i) {
            screen[0][i] = '-';
            screen[height - 1][i] = '-';
        }
        for (int i = 0; i < height; ++i) {
            screen[i][0] = '|';
            screen[i][width - 1] = '|';
        }*/

        for (Entity *entity : entitiesToRender)
        {
            PositionComponent* position = static_cast<PositionComponent*>(entity->getComponent(typeid(PositionComponent).name()));
            std::vector<std::string> entitySprite;

            if (entity->hasComponent<SpriteComponent>())
            {
                SpriteComponent* sprite = static_cast<SpriteComponent*>(entity->getComponent(typeid(SpriteComponent).name()));
                entitySprite = sprite->sprite;
            } else if (entity->hasComponent<UIComponent>())
            {
                UIComponent* uiComponent = static_cast<UIComponent*>(entity->getComponent(typeid(UIComponent).name()));
                entitySprite = { uiComponent->getContent() };  // Treat the UIComponent's content as a sprite with a single row
            }

            if (!entitySprite.empty() && position != nullptr)
            {
                int startX = position->positionXY[0];
                int startY = position->positionXY[1];

                for (int i = 0; i < entitySprite.size(); ++i)
                {
                    int y = startY + i - screenHeightUpper;

                    for (int j = 0; j < entitySprite[i].size(); ++j)
                    {
                        int x = startX + j - screenWidthLeft;

                        if (x >= 0 && x < width && y >= 0 && y < height)
                        {
                            screen[y][x] = entitySprite[i][j];
                        }
                    }
                }
            }
            else
            {
                if (entitySprite.empty()) {
                    std::cerr << "Sprite or UIComponent not found in the entity." << std::endl;
                }

                if (position == nullptr) {
                    std::cerr << "Position not found in the entity." << std::endl;
                }
            }
        }

        // Create a fullScreen vector that represents the entire screen
        std::vector<std::vector<char>> fullScreen(screenHeight, std::vector<char>(screenWidth, ' '));

        // Copy the screen vector into the fullScreen vector at the correct position
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                fullScreen[y + screenHeightUpper][x + screenWidthLeft] = screen[y][x];
            }
        }

        // Resize previousScreen to match fullScreen
        previousScreen.resize(fullScreen.size());
        for (auto& row : previousScreen) {
            row.resize(fullScreen[0].size(), ' ');
        }

        bool screenChanged = false;
        for (int y = screenHeightUpper; y < screenHeightLower; ++y)
        {
            for (int x = screenWidthLeft; x < screenWidthRight; ++x)
            {
                if (fullScreen[y][x] != previousScreen[y][x])
                {
                    screenChanged = true;
                    break;
                }
            }
        }

        if (screenChanged)
        {
            clearScreen();
            for (const auto& row : fullScreen)
            {
                for (const auto& ch : row)
                {
                    std::cout << ch;
                }
                std::cout << std::endl;
            }
            std::cout << std::flush;
        }

        previousScreen = fullScreen;
    }

    void renderUI() {
        for (Entity* entity : entitiesToRender) {
            if (entity->hasComponent<UIComponent>()) {
                UIComponent* uiComponent = static_cast<UIComponent*>(entity->getComponent(typeid(UIComponent).name()));
                PositionComponent* position = static_cast<PositionComponent*>(entity->getComponent(typeid(PositionComponent).name()));

                if (uiComponent != nullptr && position != nullptr) {
                    int startX = position->positionXY[0];
                    int startY = position->positionXY[1];
                    std::string content = uiComponent->getContent();

                    // Move the cursor to the start of each line of the entity's position
                    std::cout << "\033[" << startY + 1 << ";" << startX + 1 << "H";
                    std::cout << content << std::endl;
                } else {
                    if (uiComponent == nullptr) {
                        std::cerr << "UIComponent not found in the entity." << std::endl;
                    }

                    if (position == nullptr) {
                        std::cerr << "Position not found in the entity." << std::endl;
                    }
                }
            }
        }
    }
};