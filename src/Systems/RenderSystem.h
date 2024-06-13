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

        for (Entity& entity : entitiesToRender) {
            SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));
            PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));

            if (sprite != nullptr && position != nullptr) {
                int startX = position->positionXY[0];
                int startY = position->positionXY[1];

                for (int i = 0; i < sprite->sprite.size(); ++i) {
                    int y = startY + i - screenHeightUpper;

                    for (int j = 0; j < sprite->sprite[i].size(); ++j) {
                        int x = startX + j - screenWidthLeft;

                        if (x >= 0 && x < width && y >= 0 && y < height) {
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

    void RenderEntityFrame(Entity& entity)
    {
#ifdef _WIN32
        // Enable ANSI escape codes on Windows
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
#endif

        SpriteComponent* sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));
        PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));

        if (sprite != nullptr && position != nullptr) {
            for (int i = 0; i < sprite->sprite.size(); ++i) {
                // Move the cursor to the start of each line of the entity's position
                std::cout << "\033[" << position->positionXY[1] + i + 1 << ";" << position->positionXY[0] + 1 << "H";

                for (int j = 0; j < sprite->sprite[i].size(); ++j) {
                    std::cout << sprite->sprite[i][j];
                }
                std::cout << std::endl;
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
};