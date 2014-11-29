# CMake build file for prayer
# By: Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)

cmake_minimum_required(VERSION 2.2)
project( prayer )

set( COMPILE_FLAGS "-std=c89 -pedantic -W -Wall -Wextra -Werror -O" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}" )

# Specify header files
set( HEADERS
    src/defs.h
    src/prayer.h
    src/config.h
    )

# Specify sources
set( SRCS
    src/prayer.c
    src/config.c
    )

# Specify executables
add_executable( prayer src/main.c ${SRCS} )
target_link_libraries( prayer -lm )
