#pragma once

#include "../Entity.h"
#include "../Components/PositionComponent.h"
#include "System.h"

class TransformSystem : public System
{
public:
    void UpdateMove(Entity *entity, int x, int y)
    {
        PositionComponent *positionComponent = static_cast<PositionComponent*>(entity->getComponent(typeid(PositionComponent).name()));

        if(positionComponent != nullptr)
        {
            positionComponent->positionXY[0] = x;
            positionComponent->positionXY[1] = y;
        }
        else
        {
            std::cout << "PositionComponent not found in the entity." << std::endl;
        }
    }
};