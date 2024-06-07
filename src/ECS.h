#pragma once

#include "Entity.h"
#include "Systems/System.h"

class ECS
{
private:
    std::vector<Entity*> entities;
    std::vector<System> systems;

public:
    ECS(Entity entity)
    {
        entities.push_back(&entity);
    }

    void addEntities(Entity entity)
    {
        entities.push_back(&entity);
    }

    void listEntities()
    {
        for(int i = 0; i < entities.size(); ++i)
        {
            std::cout << entities[i]->getId() << std::endl;
        }
    }
};