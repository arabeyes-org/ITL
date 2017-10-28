# CMake build file for ITL
# By: Mohamed A.M. Bamakhrama (mohamed@alumni.tum.de)

cmake_minimum_required(VERSION 2.6)
project( itl )
include(GNUInstallDirs)
set(LIBRARY_VERSION_MAJOR "0")
set(LIBRARY_VERSION_MINOR "1")
set(LIBRARY_VERSION_PATCH "8")
set(LIBRARY_VERSION "${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}")
set(LIBRARY_VERSION_FULL "${LIBRARY_VERSION}.${LIBRARY_VERSION_PATCH}")
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
add_library( itlShared SHARED  ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( itlShared -lm )
set_target_properties(itlShared PROPERTIES VERSION "${LIBRARY_VERSION_FULL}" SOVERSION "${LIBRARY_VERSION_MAJOR}" OUTPUT_NAME "itl" )
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(includedir "\${prefix}/include")
set(PACKAGE_NAME "libitl")
set(VERSION "${LIBRARY_VERSION_FULL}")
configure_file("libitl.pc.in" "libitl.pc" @ONLY)

install( FILES ${HEADER_FILES} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/itl" )
install( TARGETS itl DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install( TARGETS itlShared DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install(FILES "libitl.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/)

# Specify executables
add_executable( demo_hijri hijri/demo_hijri.c )
target_link_libraries( demo_hijri itl -lm )

add_executable( demo_prayer prayertime/demo_prayer.c )
target_link_libraries( demo_prayer itl -lm )

