#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>

class Component
{
private:

public:

};

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

class HealthComponent : public Component
{
public:
    int health;

    HealthComponent(int health)
    {
        this->health = health;
    }

    int GetHealth(int health)
    {
        return health;
    }
};

class Sprite : public Component
{
public:
    std::vector <std::vector <char>> sprite;

    Sprite(std::vector <std::vector <char>> sprite)
    {
        this->sprite = sprite;
    }
};

class Entity
{
private:
    int id;
    std::unordered_map <std::string, Component*> components;
    std::vector<std::string> componentList;

public:
    Entity(int id)
    {
        this->id = id;
    }

    int getId()
    {
        return id;
    }

    template <typename T>
    void addComponent(T* Component)
    {
        for(int i = 0; i < componentList.size(); ++i)
        {
            if(typeid(T).name() == componentList[i])
            {
                std::cout << "Component already exists in the entity." << std::endl;
                return;
            }
        }

        components[typeid(T).name()] = Component;
        componentList.emplace_back(typeid(T).name());
    }

    Component* getComponent(const std::string& componentType)
    {
        Component* component = components[componentType];

        if (component == nullptr)
        {
            std::cout << "Component not found in the entity." << std::endl;

            return nullptr;
        }

        return component;
    }

    void removeComponent(const std::string& componentType)
    {
        components.erase(componentType);
        componentList.erase(std::remove(componentList.begin(), componentList.end(), componentType), componentList.end());
    }

    std::vector<std::string> listComponents()
    {
        for(int i = 0; i < componentList.size(); ++i)
        {
            std::cout << componentList[i] << std::endl;
        }

        return componentList;
    }
};

class System
{
private:

public:

};

class Move : public System
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

class Health : public System
{
public:
    void UpdateHealth(Entity *entity, int change)
    {
        HealthComponent *healthComponent = static_cast<HealthComponent*>(entity->getComponent(typeid(HealthComponent).name()));

        if(healthComponent != nullptr)
        {
            healthComponent->health += change;
        }
        else
        {
            std::cout << "HealthComponent not found in the entity." << std::endl;
        }
    }
};

class Render : public System
{
public:
    std::vector <Entity> entitiesToRender;

    void UpdateRender()
    {
        for (Entity& entity : entitiesToRender)
        {
            Sprite *sprite = static_cast<Sprite*>(entity.getComponent(typeid(Sprite).name()));
            PositionComponent *position = static_cast<PositionComponent*>(entity.getComponent(typeid(PositionComponent).name()));

            if(sprite != nullptr)
            {
                for (int i = 0; i < position->positionXY[1]; ++i)
                {
                    std::cout << std::endl;
                }

                for(int i = 0; i < sprite->sprite.size(); ++i)
                {
                    for (int i = 0; i < position->positionXY[0]; ++i)
                    {
                        std::cout << " ";
                    }

                    for(int j = 0; j < sprite->sprite[i].size(); ++j)
                    {
                        std::cout << sprite->sprite[i][j];
                    }

                    std::cout << std::endl;
                }
            }
            else
            {
                std::cout << "Sprite not found in the entity." << std::endl;
            }
        }
    }

};

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

int main()
{
    // INITIALIZATION OF SYSTEMS
    Move moveSys;
    Health healthSys;
    Render renderSys;

    Entity player(1);
    Entity enemy(2);

    // INITIALIZATION OF ECS
    ECS* ecs = new ECS(player);


    //CODE
    std::vector <std::vector<char>> sprite = {{'(', '-', ')'}, {'(', '0', ')'}, {'(', '_', ')'}};

    PositionComponent position(0,0);
    Sprite spriteComp(sprite);
    player.addComponent(&position);
    player.addComponent(&spriteComp);

    moveSys.UpdateMove(&player, 10, 2);

    std::vector <std::vector<char>> sprite2 = {{'(', '-', ')'}, {'(', '0', ')'}, {'(', '_', ')'}};

    PositionComponent position2(0,0);
    Sprite spriteComp2(sprite2);
    enemy.addComponent(&position2);
    enemy.addComponent(&spriteComp2);

    moveSys.UpdateMove(&player, 20, 0);
    moveSys.UpdateMove(&enemy, 10, 0);

    renderSys.entitiesToRender.push_back(player);
    renderSys.entitiesToRender.push_back(enemy);

    renderSys.UpdateRender();

    /*PositionComponent position(0,0);
    HealthComponent health(100);

    entity.addComponent(&position);
    entity.addComponent(&health);

    std::vector <std::string> components= entity.listComponents();

    std::cout << "X: " << position.positionXY[0] << std::endl;
    std::cout << "Y: " << position.positionXY[1] << std::endl;

    std::cout << "Health: " << health.health << std::endl;

    moveSys.UpdateMove(&entity, 1, 2, 3);
    healthSys.UpdateHealth(&entity, 10);

    HealthComponent *healthComp = static_cast<HealthComponent*>(entity.getComponent(typeid(HealthComponent).name()));

    std::cout << typeid(HealthComponent).name() << std::endl;*/

    return 0;
}
