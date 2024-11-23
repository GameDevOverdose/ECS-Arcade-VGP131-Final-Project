#pragma once

#ifdef _WIN32
#include <conio.h>
#else
#include <ncurses.h>
#endif
#include <unordered_map>

class InputSystem
{
public:
    enum KeyState {
        KEY_STATE_UP,
        KEY_STATE_DOWN,
        KEY_STATE_HELD,
        KEY_STATE_RELEASED
    };

    void Update();
    KeyState getKeyState(char key);
    void setKeyState(char key, KeyState state);

private:
    std::unordered_map<char, KeyState> keyStates;
    std::unordered_map<char, KeyState> prevKeyStates;
};

void InputSystem::Update() {
    prevKeyStates = keyStates;

#if _WIN32
    while (_kbhit()) {
        char key = _getch();
        keyStates[key] = KEY_STATE_DOWN;
    }
#else
    if (int key = getch(); key != ERR) {
        keyStates[key] = KEY_STATE_DOWN;
    }
#endif

    for (auto& [key, state] : keyStates) {
        if (state == KEY_STATE_DOWN) {
            if (prevKeyStates[key] == KEY_STATE_DOWN || prevKeyStates[key] == KEY_STATE_HELD) {
                keyStates[key] = KEY_STATE_HELD;
            }
        } else {
            if (prevKeyStates[key] == KEY_STATE_DOWN || prevKeyStates[key] == KEY_STATE_HELD) {
                keyStates[key] = KEY_STATE_RELEASED;
            } else {
                keyStates[key] = KEY_STATE_UP;
            }
        }
    }
}

InputSystem::KeyState InputSystem::getKeyState(char key) {
    if (keyStates.find(key) != keyStates.end()) {
        return keyStates[key];
    } else {
        return KEY_STATE_UP;
    }
}

void InputSystem::setKeyState(char key, KeyState state) {
    keyStates[key] = state;
}
