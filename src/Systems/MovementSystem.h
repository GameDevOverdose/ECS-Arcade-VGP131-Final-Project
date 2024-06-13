#pragma once

#include "../Entity.h"
#include "CollisionSystem.h"
#include "InputSystem.h"
#include "TransformSystem.h"

InputSystem inputSystem;
TransformSystem transformSystem;

class MovementSystem {
public:
    MovementSystem() = default;

    void MoveInput(Entity& entity, char input, int* moveXY, CollisionSystem& collisionSystem)
    {
        PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));

        if (inputSystem.getKeyState(input) == InputSystem::KEY_HELD)
        {
            // Predict the new position
            int predictedPosition[2] = {position->positionXY[0] + moveXY[0], position->positionXY[1] + moveXY[1]};

            // Check if the new position would result in a collision
            if (!collisionSystem.wouldCollide(entity, predictedPosition))
            {
                // If not, perform the move
                transformSystem.UpdateTransform(&entity, moveXY[0], moveXY[1]);
            }
        }
    }

    void Move(Entity& entity, int* moveXY, CollisionSystem& collisionSystem, std::vector<int> collisionObjectsID, std::vector<int> triggerObjectsID, std::vector<int> collisions)
    {
        // Predict the new position
        PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
        int predictedPosition[2] = {position->positionXY[0] + moveXY[0], position->positionXY[1] + moveXY[1]};

        std::vector<bool> collisionVector = collisionSystem.wouldCollide(entity, predictedPosition);

        for(int i = 0; i < collisionVector.size(); i++)
        {
            if(collisionVector[i] == true)
            {
                for(int j = 0; j < collisionObjectsID.size(); j++)
                {
                    if(i == collisionObjectsID[j])
                    {
                        transformSystem.UpdateTransform(&entity, moveXY[0], moveXY[1]);
                    }
                }

                for(int j = 0; j < triggerObjectsID.size(); j++)
                {
                    if(i == triggerObjectsID[j])
                    {
                        return;
                    }
                }
            }
        }
    }
};