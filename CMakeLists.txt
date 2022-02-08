cmake_minimum_required(VERSION 3.10)

project(arc)

set(CMAKE_C_FLAGS "-g -Wall -Werror")

add_executable(arc
  src/main.c 

  src/args.c
  src/args.h

  src/arc_structures.c
  src/arc_structures.h
)
