#pragma once

#include <windows.h> // for Sleep

#include "../Entity.h"
#include "System.h"
#include "../Components/PositionComponent.h"
#include "../Components/SpriteComponent.h"

class RenderSystem : public System {
public:
    std::vector<Entity> entitiesToRender;

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

        //std::cout << "Clearing screen..." << std::endl;
    }

    void RenderScreen(int screenWidth, int screenHeight)
    {
        std::vector<std::vector<char>> previousScreen(screenHeight, std::vector<char>(screenWidth, ' '));

        RenderFrame(screenWidth, screenHeight, previousScreen);
    }

    void RenderFrame(int screenWidth, int screenHeight, std::vector<std::vector<char>> previousScreen)
    {
        std::vector<std::vector<char>> screen(screenHeight, std::vector<char>(screenWidth, ' '));

        for (Entity& entity : entitiesToRender) {
            SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));
            PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));

            if (sprite != nullptr && position != nullptr) {
                int startX = position->positionXY[0];
                int startY = position->positionXY[1];

                for (int i = 0; i < sprite->sprite.size(); ++i) {
                    int y = startY + i;

                    for (int j = 0; j < sprite->sprite[i].size(); ++j) {
                        int x = startX + j;

                        if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
                            screen[y][x] = sprite->sprite[i][j];
                        }
                    }
                }
            } else {
                if (sprite == nullptr) {
                    std::cerr << "Sprite not found in the entity." << std::endl;
                }

                if (position == nullptr) {
                    std::cerr << "Position not found in the entity." << std::endl;
                }
            }
        }

        // Print the screen grid only if there's a difference from the previous screen
        bool screenChanged = false;
        for (int y = 0; y < screenHeight; ++y) {
            for (int x = 0; x < screenWidth; ++x) {
                if (screen[y][x] != previousScreen[y][x]) {
                    screenChanged = true;
                    break;
                }
            }
            if (screenChanged) break;
        }

        if (screenChanged)
        {
            clearScreen();
            for (const auto& row : screen)
            {
                for (const auto& ch : row)
                {
                    std::cout << ch;
                }
                std::cout << std::endl;
            }
            std::cout << std::flush;
        }

        previousScreen = screen;
    }
};