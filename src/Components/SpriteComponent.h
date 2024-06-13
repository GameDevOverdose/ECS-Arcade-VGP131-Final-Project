#pragma once

#include <vector>
#include "Component.h"

class SpriteComponent : public Component
{
public:
    std::vector <std::string> sprite;

    SpriteComponent(std::vector <std::string> sprite)
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