cmake_minimum_required(VERSION 2.8.3)
project(SB2018-2)

add_definitions(-std=c++14)
add_definitions(-g)

include_directories(
  montador/include/
)

link_directories(
)

add_executable(montador.out
  montador/src/montador.cpp
  montador/src/utils.cpp
  montador/src/preprocessor.cpp
)
