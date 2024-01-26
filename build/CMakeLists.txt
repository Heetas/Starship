cmake_minimum_required(VERSION 3.22)
project(Starship C)

set(CMAKE_C_STANDARD 23)

add_executable(Server
        ../Server.c
        ../lib.h
        ../network.h
        ../game.h)

add_executable(Client
        ../Client.c
        ../lib.h
        ../network.h
        ../game.h)