cmake_minimum_required(VERSION 3.16)
project(Final_Project)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*")
add_executable(Final_Project ${SOURCES}
        src/Engine/Components/Component.h
        src/Engine/Components/PositionComponent.h
        src/Engine/Entity.h
        src/Engine/Systems/System.h
        src/Engine/Systems/TransformSystem.h
        src/Engine/Components/SpriteComponent.h
        src/Engine/Systems/RenderSystem.h
        src/Engine/ECS.h
        src/Engine/Systems/TimeSystem.h
        src/Engine/Systems/InputSystem.h
        src/Engine/Systems/CollisionSystem.h
        src/Engine/Systems/MovementSystem.h
        src/Engine/Components/UIComponent.h
        src/Engine/Components/AnimationComponent.h)

# Find Curses
if (APPLE)
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(Final_Project ${CURSES_LIBRARIES})
endif()
