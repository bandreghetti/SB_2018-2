cmake_minimum_required(VERSION 2.8.3)
project(SB2018-2)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

add_definitions(-std=c++11)
add_definitions(-g)

include_directories(
  montador/include/
  ligador/include/
  common/include/
)

link_directories(
)

add_executable(montador.out
  montador/src/montador.cpp
  montador/src/preprocessor.cpp
  montador/src/assembler.cpp
  common/src/utils.cpp
)

add_executable(ligador.out
  ligador/src/ligador.cpp
  ligador/src/linker.cpp
  common/src/utils.cpp
)
