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

    void flipSpriteHorizontally()
    {
        for(auto& row : sprite)
        {
            std::reverse(row.begin(), row.end());
        }
    }

    void flipSpriteVertically()
    {
        std::reverse(sprite.begin(), sprite.end());
    }
};