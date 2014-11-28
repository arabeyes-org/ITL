# CMake build file for prayer
# By: Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)

cmake_minimum_required(VERSION 2.2)
project( prayer )

set( COMPILE_FLAGS "-std=c89 -pedantic -W -Wall -Wextra -Werror -O" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}" )

# Specify header files
set( HEADERS
    defs.h
    prayer.h
    config.h
    )

# Specify sources
set( SRCS
    prayer.c
    config.c
    )

# Specify executables
add_executable( prayer main.c ${SRCS} )
target_link_libraries( prayer -lm )
