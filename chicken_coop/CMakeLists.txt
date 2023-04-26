cmake_minimum_required(VERSION 3.23)
project(chicken_coop)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(chicken_coop
        door_controller.cpp
        detect_egg_and_weigh.cpp
        chicken_light_controller.cpp main.cpp dispense_food_water.cpp temp_controller.cpp table_generator.cpp table_generator.h humidity_monitor.cpp)
