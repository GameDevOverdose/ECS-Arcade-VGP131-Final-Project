#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <forward_list>
#include <queue>
#include <stack>

class Component
{
private:

public:

};

class PositionComponent : public Component
{
public:
    int positionXYZ[3];

    PositionComponent(int x, int y, int z)
    {
        positionXYZ[0] = x;
        positionXYZ[1] = y;
        positionXYZ[2] = z;
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

class VelocityComponent : public Component
{
public:
    float velocity;

    VelocityComponent(float velocity)
    {
        this->velocity = velocity;
    }

    float GetVelocity(float velocity)
    {
        return velocity;
    }
};

class MaterialComponent : public Component
{
public:
    std::string material;

    MaterialComponent(std::string material)
    {
        this->material = material;
    }

    std::string GetMaterial(std::string material)
    {
        return material;
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
    void UpdateMove(Entity *entity, int x, int y, int z)
    {
        PositionComponent *positionComponent = static_cast<PositionComponent*>(entity->getComponent(typeid(PositionComponent).name()));

        if(positionComponent != nullptr)
        {
            positionComponent->positionXYZ[0] = x;
            positionComponent->positionXYZ[1] = y;
            positionComponent->positionXYZ[2] = z;
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

class Material : public System
{
public:
    void ChangeMaterial(Entity *entity, std::string newMaterial)
    {
        MaterialComponent *materialComponent = static_cast<MaterialComponent*>(entity->getComponent(typeid(MaterialComponent).name()));

        if(materialComponent != nullptr)
        {
            materialComponent->material = newMaterial;
        }
        else
        {
            std::cout << "MaterialComponent not found in the entity." << std::endl;
        }
    }
};

class ECS
{
private:
    std::vector<Entity> entities;
    std::vector<System> systems;

public:
    ECS(Entity entity)
    {
        entities.push_back(entity);
    }

    void addEntities(Entity entity)
    {
        entities.push_back(entity);
    }

    void listEntities()
    {
        for(int i = 0; i < entities.size(); ++i)
        {
            std::cout << entities[i].getId() << std::endl;
        }
    }
};

int main()
{
    Entity entity(1);

    ECS* ecs = new ECS(entity);

    PositionComponent position(0,0,0);
    HealthComponent health(100);
    VelocityComponent velocity(50);
    MaterialComponent material("Fabric");

    entity.addComponent(&position);
    entity.addComponent(&health);
    entity.addComponent(&velocity);
    entity.addComponent(&material);

    std::vector <std::string> components= entity.listComponents();

    std::cout << "X: " << position.positionXYZ[0] << std::endl;
    std::cout << "Y: " << position.positionXYZ[1] << std::endl;
    std::cout << "Z: " << position.positionXYZ[2] << std::endl;

    std::cout << "Health: " << health.health << std::endl;

    std::cout << "Material: " << material.material << std::endl;

    System system;
    Move move;
    Health healthSys;
    Material materialSys;

    move.UpdateMove(&entity, 1, 2, 3);
    healthSys.UpdateHealth(&entity, 10);
    materialSys.ChangeMaterial(&entity, "Wood");

    HealthComponent *healthComp = static_cast<HealthComponent*>(entity.getComponent(typeid(HealthComponent).name()));

    std::cout << typeid(HealthComponent).name() << std::endl;

    return 0;
}
