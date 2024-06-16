#pragma once

#include <iostream>
#include <unordered_map>
#include "Components/Component.h"

#include "Components/PositionComponent.h"

class Entity
{
private:
    int id;
    std::string name;

    std::unordered_map <std::string, Component*> components;
    std::vector<std::string> componentList;

public:
    Entity(std::string name, int x, int y)
    {
        static int next_id = 0;
        id = next_id++;

        this->name = name;

        PositionComponent* positionComponent = new PositionComponent(x, y);
        addComponent(positionComponent);
    }

    Entity(std::string name)
    {
        static int next_id = 0;
        id = next_id++;

        this->name = name;
    }

    int getId()
    {
        return id;
    }

    bool operator==(const Entity& other) const
    {
        return id == other.id;  // Compare entities based on their id or any other unique property
    }

    std::string getName()
    {
        return name;
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
        }

        return componentList;
    }
};