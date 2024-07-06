cmake_minimum_required(VERSION 3.21)

# Set required C++ version.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(CppIdeas
        VERSION 0.1.0
        DESCRIPTION "A project for trying things out in C++."
        LANGUAGES CXX)

add_executable(maybeOwning maybe_owning/pointer_holder.cpp)

if(UNIX)
	add_executable(simpleConcurrentGame simple_concurrent_console_game/simple_game.cpp)
endif()
