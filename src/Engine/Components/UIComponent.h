#pragma once

#include <xstring>
#include "Component.h"

class UIComponent : public Component
{
private:
    std::string content;
    int color; // Color code

public:
    UIComponent(const std::string& content, int color)
    {
        this->content = content;
        this->color = color;
    }

    UIComponent(const std::string& content)
    {
        this->content = content;
    }

    // Getters
    std::string getContent() const { return content; }
    int getColor() const { return color; }

    // Setters
    void setContent(const std::string& newContent) { content = newContent; }
    void setColor(int newColor) { color = newColor; }
};