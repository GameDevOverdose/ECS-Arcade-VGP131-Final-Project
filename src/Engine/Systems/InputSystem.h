#pragma once

#include <conio.h>
#include <unordered_map>

class InputSystem
{
public:
    enum KeyState {
        KEY_UP,
        KEY_DOWN,
        KEY_HELD,
        KEY_RELEASED
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

    while (_kbhit()) {
        char key = _getch();
        keyStates[key] = KEY_DOWN;
    }

    for (auto& [key, state] : keyStates) {
        if (state == KEY_DOWN) {
            if (prevKeyStates[key] == KEY_DOWN || prevKeyStates[key] == KEY_HELD) {
                keyStates[key] = KEY_HELD;
            }
        } else {
            if (prevKeyStates[key] == KEY_DOWN || prevKeyStates[key] == KEY_HELD) {
                keyStates[key] = KEY_RELEASED;
            } else {
                keyStates[key] = KEY_UP;
            }
        }
    }
}

InputSystem::KeyState InputSystem::getKeyState(char key) {
    if (keyStates.find(key) != keyStates.end()) {
        return keyStates[key];
    } else {
        return KEY_UP;
    }
}

void InputSystem::setKeyState(char key, KeyState state) {
    keyStates[key] = state;
}
