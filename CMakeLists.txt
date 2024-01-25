cmake_minimum_required(VERSION 3.22)
project(Starship C)

set(CMAKE_C_STANDARD 23)

add_executable(Starship Server.c Client.c lib.h
        network.h
        game.h)