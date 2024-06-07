#pragma once

#include "../Entity.h"
#include "System.h"
#include "../Components/PositionComponent.h"
#include "../Components/SpriteComponent.h"

class RenderSystem : public System
{
public:
    std::vector <Entity> entitiesToRender;

    void UpdateRender()
    {
        for (Entity& entity : entitiesToRender)
        {
            SpriteComponent *sprite = static_cast<SpriteComponent*>(entity.getComponent(typeid(SpriteComponent).name()));
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