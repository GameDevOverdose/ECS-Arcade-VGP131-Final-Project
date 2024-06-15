#pragma once

#include <algorithm>
#include "Entity.h"
#include "Systems/System.h"

class ECS
{
private:
    std::vector<Entity*> entities;
    //std::vector<System> systems;

public:
    ECS(Entity entity)
    {
        entities.push_back(&entity);
    }

    ECS()
    {}

    void addEntities(std::vector <Entity*> entity)
    {
        for (Entity* e : entity)
        {
            entities.push_back(e);
        }

        sortEntitiesById();
    }

    void addEntities(Entity *entity)
    {
        entities.push_back(entity);

        sortEntitiesById();
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
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    }

    void sortEntitiesById()
    {
        std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b){ return a->getId() < b->getId();});
    }

    void listEntities()
    {
        for(int i = 0; i < entities.size(); ++i)
        {
            std::cout << entities[i]->getName() << ", ID: " << entities[i]->getId() << std::endl;
        }

        if(entities.size() == 0)
        {
            std::cout << "No entities in the ECS." << std::endl;
        }
    }

    int getEntityCount()
    {
        return entities.size();
    }
};