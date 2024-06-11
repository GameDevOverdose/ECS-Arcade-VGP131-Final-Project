cmake_minimum_required(VERSION 3.16)
project(Final_Project)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*")
add_executable(Final_Project ${SOURCES}
        src/Components/Component.h
        src/Components/PositionComponent.h
        src/Entity.h
        src/Systems/System.h
        src/Systems/TransformSystem.h
        src/Components/SpriteComponent.h
        src/Systems/RenderSystem.h
        src/ECS.h
        src/Systems/TimeSystem.h
        src/Systems/InputSystem.h
        src/Systems/CollisionSystem.h)
