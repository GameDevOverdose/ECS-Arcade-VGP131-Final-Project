#pragma once

#include <vector>
#include "Component.h"

class SpriteComponent : public Component
{
public:
    std::vector <std::vector <char>> sprite;

    SpriteComponent(std::vector <std::vector <char>> sprite)
    {
        this->sprite = sprite;
    }

    int getHeight()
    {
        return sprite.size();
    }

    int getWidth()
    {
        return sprite[0].size();
    }
};