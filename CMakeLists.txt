cmake_minimum_required(VERSION 3.28)
project(uuid C)

set(CMAKE_C_STANDARD 23)

add_executable(uuid main.c
        uuid.c
        uuid.h)
