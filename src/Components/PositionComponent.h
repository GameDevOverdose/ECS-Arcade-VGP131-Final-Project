#pragma once

#include "Component.h"

class PositionComponent : public Component
{
public:
    int positionXY[2];

    PositionComponent(int x, int y)
    {
        positionXY[0] = x;
        positionXY[1] = y;
    }
};
