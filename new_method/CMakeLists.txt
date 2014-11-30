# CMake build file for prayer
# By: Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)

cmake_minimum_required(VERSION 2.2)
project( prayer )

IF (UNIX)
    IF ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        set( COMPILE_FLAGS "-std=c89 -Weverything -O2" )
    ELSEIF ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        set( COMPILE_FLAGS "-std=c89 -W -Wall -Wextra -Werror -O2" )
    ENDIF()
    set( LINKER_FLAGS "-lm" )
    set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}" )
ELSE (WIN32 AND MSVC)
    set( COMPILE_FLAGS "/Za /Wall /O2" )
    set( LINKER_FLAGS "" )
    set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}" )
ENDIF (UNIX)

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
target_link_libraries( prayer ${LINKER_FLAGS} )
