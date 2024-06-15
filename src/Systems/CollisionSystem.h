#pragma once

#include <iostream>
#include "../Components/PositionComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Entity.h"
#include "../ECS.h"

class CollisionSystem
{
public:
    std::vector<Entity*> colliderEntities;
    ECS *ecs;

    CollisionSystem(ECS *ecs)
    {
        this->ecs = ecs;
    }

    void addEntities(std::vector <Entity*> entity)
    {
        for (Entity* e : entity)
        {
            colliderEntities.push_back(e);
        }
    }

    void addEntities(Entity *entity)
    {
        colliderEntities.push_back(entity);
    }

    void removeEntities(std::vector<Entity*> *entityList)
    {
        for (Entity* e : *entityList)
        {
            removeEntity(e);
        }
    }

    void removeEntity(Entity* entity)
    {
        colliderEntities.erase(std::remove(colliderEntities.begin(), colliderEntities.end(), entity), colliderEntities.end());
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
        for (size_t i = 0; i < colliderEntities.size(); ++i)
        {
            if (entity.getId() != colliderEntities[i]->getId())
            {
                if (checkCollision(entity, *colliderEntities[i]))
                {
                    // Handle collision
                    std::cout << "Collision detected between Entity " << entity.getId()
                              << " and Entity " << colliderEntities[i]->getId() << std::endl;
                }
            }
        }
    }

    void UpdateAllCollisions()
    {
        for (size_t i = 0; i < colliderEntities.size(); ++i)
        {
            for (size_t j = i + 1; j < colliderEntities.size(); ++j)
            {
                if (checkCollision(*colliderEntities[i], *colliderEntities[j]))
                {
                    // Handle collision
                    std::cout << "Collision detected between Entity " << colliderEntities[i]->getId()
                              << " and Entity " << colliderEntities[j]->getId() << std::endl;
                }
            }
        }
    }

    std::vector<bool> wouldCollide(Entity& entity, int* predictedPosition)
    {
        std::vector<bool> collisionVector(ecs->getEntityCount(), false);

        for (size_t i = 0; i < colliderEntities.size(); ++i)
        {
            if (entity.getId() != colliderEntities[i]->getId())
            {
                PositionComponent* posB = static_cast<PositionComponent*>(colliderEntities[i]->getComponent(typeid(PositionComponent).name()));
                SpriteComponent* spriteB = static_cast<SpriteComponent*>(colliderEntities[i]->getComponent(typeid(SpriteComponent).name()));

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
                    collisionVector[colliderEntities[i]->getId()] = true;
                }
            }
        }

        return collisionVector;
    }
};