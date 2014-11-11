# CMake build file for ITL
# By: Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)

cmake_minimum_required(VERSION 2.6)
project( itl )
message( "Installation target directory is: " ${CMAKE_INSTALL_PREFIX} )
message( "To override this value, use 'make -DCMAKE_INSTALL_PREFIX=$DIR'" )

set( COMPILE_FLAGS "-ansi -pedantic -Wall -O2" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}" )

# Specify header files
set( HEADER_FILES
        hijri/hijri.h
        prayertime/prayer.h
        prayertime/astro.h
        )

# Specify sources
set( SOURCE_FILES
        hijri/hijri.c
        prayertime/astro.c
        prayertime/prayer.c
        hijri/umm_alqura.c
        )

add_library( itl STATIC ${HEADER_FILES} ${SOURCE_FILES} )
install( FILES ${HEADER_FILES} DESTINATION "include/" )
install( TARGETS itl DESTINATION "lib/" )

# Specify executables
add_executable( demo_hijri hijri/demo_hijri.c )
target_link_libraries( demo_hijri itl -lm )

add_executable( demo_prayer prayertime/demo_prayer.c )
target_link_libraries( demo_prayer itl -lm )

