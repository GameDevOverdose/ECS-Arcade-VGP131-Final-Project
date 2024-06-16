#pragma once

#include <algorithm>
#include "../Entity.h"
#include "CollisionSystem.h"
#include "InputSystem.h"
#include "TransformSystem.h"

InputSystem inputSystem;
TransformSystem transformSystem;

class MovementSystem {
public:
    MovementSystem() = default;

    void MoveInput(Entity& entity, char input, int* moveXY, CollisionSystem& collisionSystem, std::vector<int> triggerObjectsID, std::vector<bool> &triggers)
    {
        // Predict the new position
        PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
        int predictedPosition[2] = {position->positionXY[0] + moveXY[0], position->positionXY[1] + moveXY[1]};

        std::vector<bool> collisionVector = collisionSystem.wouldCollide(entity, predictedPosition);

        bool allFalseExceptTrigger = true;

        if (inputSystem.getKeyState(input) == InputSystem::KEY_HELD)
        {
            for(int i = 0; i < collisionVector.size(); i++)
            {
                if(collisionVector[i] == true)
                {
                    if(!triggerObjectsID.empty())
                    {
                        for(int j = 0; j < triggerObjectsID.size(); j++)
                        {
                            if(i == triggerObjectsID[j])
                            {
                                allFalseExceptTrigger = true;
                                break;
                            }
                            else
                            {
                                allFalseExceptTrigger = false;
                            }
                        }
                    }
                    else
                    {
                        allFalseExceptTrigger = false;
                        break;
                    }
                }
            }

            if(allFalseExceptTrigger == true)
            {
                transformSystem.UpdateTransform(&entity, moveXY[0], moveXY[1]);
            }

            for(int i = 0; i < collisionVector.size(); i++)
            {
                if(collisionVector[i] == true)
                {
                    for(int j = 0; j < triggerObjectsID.size(); j++)
                    {
                        if(i == triggerObjectsID[j])
                        {
                            triggers[i] = true;
                        }
                    }
                }
            }
        }
    }

    std::vector<bool> Move(Entity& entity, int* moveXY, CollisionSystem& collisionSystem, std::vector<int> triggerObjectsID)
    {
        // Predict the new position
        PositionComponent* position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));
        int predictedPosition[2] = {position->positionXY[0] + moveXY[0], position->positionXY[1] + moveXY[1]};

        std::vector<bool> collisionVector = collisionSystem.wouldCollide(entity, predictedPosition);
        std::vector<bool> triggers(collisionVector.size());

        bool allFalseExceptTrigger = true;

        for(int i = 0; i < collisionVector.size(); i++)
        {
            if(collisionVector[i] == true)
            {
                if(!triggerObjectsID.empty())
                {
                    for(int j = 0; j < triggerObjectsID.size(); j++)
                    {
                        if(i == triggerObjectsID[j])
                        {
                            allFalseExceptTrigger = true;
                            break;
                        }
                        else
                        {
                            allFalseExceptTrigger = false;
                        }
                    }
                }
                else
                {
                    allFalseExceptTrigger = false;
                    break;
                }
            }
        }

        if(allFalseExceptTrigger == true)
        {
            transformSystem.UpdateTransform(&entity, moveXY[0], moveXY[1]);
        }

        for(int i = 0; i < collisionVector.size(); i++)
        {
            if(collisionVector[i] == true)
            {
                for(int j = 0; j < triggerObjectsID.size(); j++)
                {
                    if(i == triggerObjectsID[j])
                    {
                        triggers[i] = true;
                    }
                }
            }
        }

        return triggers;
    }
};