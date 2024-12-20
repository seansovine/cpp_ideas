cmake_minimum_required(VERSION 3.21)

# Set required C++ version.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(CppIdeas
        VERSION 0.1.0
        DESCRIPTION "A project for trying things out in C++."
        LANGUAGES CXX)

add_executable(MaybeOwning maybe_owning/pointer_holder.cpp)

# Basic multithreaded console app.

if(UNIX)
	add_executable(SimpleConcurrentGame simple_concurrent_console_game/simple_game.cpp)
endif()

# For ncurses version.
set(CURSES_NEED_NCURSES ON)
find_package(Curses)

if(CURSES_HAVE_NCURSES_H)
	# Our curses wrapper library.
	add_library(CursesConsole curses_console/curses_console.h curses_console/curses_console.cpp)
	target_include_directories(CursesConsole PUBLIC curses_console/)
	target_link_libraries(CursesConsole ${CURSES_LIBRARIES})

	# Version of console app using curses.
	add_executable(SimpleConcurrentGameCurses simple_concurrent_console_game/simple_game_curses.cpp)
	target_link_libraries(SimpleConcurrentGameCurses CursesConsole)
endif()
