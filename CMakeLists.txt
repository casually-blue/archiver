cmake_minimum_required(VERSION 3.10)

project(arc)

set(CMAKE_C_FLAGS "-g -Wall -Werror")

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

add_executable(arc
  src/main.c 

  src/args.c
  src/args.h

  src/arc_structures.h
  src/arc_structures.c

  src/memmap_file.h
  src/memmap_file.c

  src/varray.h
  src/astring.h
)

target_link_libraries(arc crypto astring varray)

option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
