#pragma once

#include <iostream>
#include "../Components/PositionComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Entity.h"

class CollisionSystem
{
public:
    std::vector<Entity> entities;

    void addEntity(Entity entity)
    {
        entities.push_back(entity);
    }

    bool checkCollision(Entity& entityA, Entity& entityB)
    {
        PositionComponent* posA = static_cast<PositionComponent*>(entityA.getComponent(typeid(PositionComponent).name()));
        SpriteComponent* spriteA = static_cast<SpriteComponent*>(entityA.getComponent(typeid(SpriteComponent).name()));
        PositionComponent* posB = static_cast<PositionComponent*>(entityB.getComponent(typeid(PositionComponent).name()));
        SpriteComponent* spriteB = static_cast<SpriteComponent*>(entityB.getComponent(typeid(SpriteComponent).name()));

        if (posA && spriteA && posB && spriteB)
        {
            int aLeft = posA->positionXY[0];
            int aRight = posA->positionXY[0] + spriteA->getWidth();
            int aTop = posA->positionXY[1];
            int aBottom = posA->positionXY[1] + spriteA->getHeight();

            int bLeft = posB->positionXY[0];
            int bRight = posB->positionXY[0] + spriteB->getWidth();
            int bTop = posB->positionXY[1];
            int bBottom = posB->positionXY[1] + spriteB->getHeight();

            bool xOverlap = (aLeft < bRight) && (aRight > bLeft);
            bool yOverlap = (aTop < bBottom) && (aBottom > bTop);

            return xOverlap && yOverlap;
        }

        return false;
    }

    void UpdateDoubleCollisions(Entity& entity)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            if (entity.getId() != entities[i].getId())
            {
                if (checkCollision(entity, entities[i]))
                {
                    // Handle collision
                    std::cout << "Collision detected between Entity " << entity.getId()
                              << " and Entity " << entities[i].getId() << std::endl;
                }
            }
        }
    }

    void UpdateAllCollisions()
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            for (size_t j = i + 1; j < entities.size(); ++j)
            {
                if (checkCollision(entities[i], entities[j]))
                {
                    // Handle collision
                    std::cout << "Collision detected between Entity " << entities[i].getId()
                              << " and Entity " << entities[j].getId() << std::endl;
                }
            }
        }
    }

    bool wouldCollide(Entity& entity, int* predictedPosition)
    {
        for (size_t i = 0; i < entities.size(); ++i)
        {
            if (entity.getId() != entities[i].getId())
            {
                PositionComponent* posB = static_cast<PositionComponent*>(entities[i].getComponent(typeid(PositionComponent).name()));
                SpriteComponent* spriteB = static_cast<SpriteComponent*>(entities[i].getComponent(typeid(SpriteComponent).name()));

                int aLeft = predictedPosition[0];
                int aRight = predictedPosition[0] + static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()))->getWidth();
                int aTop = predictedPosition[1];
                int aBottom = predictedPosition[1] + static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()))->getHeight();

                int bLeft = posB->positionXY[0];
                int bRight = posB->positionXY[0] + spriteB->getWidth();
                int bTop = posB->positionXY[1];
                int bBottom = posB->positionXY[1] + spriteB->getHeight();

                bool xOverlap = (aLeft < bRight) && (aRight > bLeft);
                bool yOverlap = (aTop < bBottom) && (aBottom > bTop);

                if (xOverlap && yOverlap)
                {
                    return true;
                }
            }
        }

        return false;
    }
};