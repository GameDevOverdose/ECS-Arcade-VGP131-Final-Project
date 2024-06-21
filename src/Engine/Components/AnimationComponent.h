#pragma once

#include <vector>
#include <string>
#include "Component.h"

class AnimationComponent : public Component
{
public:
    std::vector <std::vector <std::string>> frames;

    AnimationComponent(std::vector <std::vector <std::string>> frames)
    {
        this->frames = frames;
    }

    AnimationComponent(std::vector <std::string> sprite)
    {
        frames.push_back(sprite);
    }

    int getHeight()
    {
        return frames[0].size();
    }

    int getWidth()
    {
        return frames[0][0].size();
    }

    void flipSpriteHorizontally()
    {
        for(auto& frame : frames)
        {
            for(auto& row : frame)
            {
                std::reverse(row.begin(), row.end());
            }
        }
    }

    void flipSpriteVertically()
    {
        std::reverse(frames.begin(), frames.end());
    }

    void addFrame(std::vector <std::string> frame)
    {
        frames.push_back(frame);
    }

    void removeFrame(int index)
    {
        frames.erase(frames.begin() + index);
    }

    void clearFrames()
    {
        frames.clear();
    }

    void setFrames(std::vector <std::vector <std::string>> frames)
    {
        this->frames = frames;
    }

    std::vector <std::vector <std::string>> getFrames()
    {
        return frames;
    }

    std::vector <std::string> getFrame(int index)
    {
        return frames[index];
    }
};