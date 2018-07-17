cmake_minimum_required(VERSION 2.8)
find_package(PkgConfig REQUIRED)

project(mtsynth)
set(mtsynth_VERSION "1.0.0")
set(mtsynth_DESCRIPTION "Map Text Synthesizer")

add_library(mtsynth SHARED
    src/map_text_synthesizer.cpp
    src/mts_basehelper.cpp
    src/mts_bghelper.cpp
    src/mts_implementation.cpp
    src/mts_texthelper.cpp
    src/mts_utilities.cpp
    )

set_target_properties(mtsynth PROPERTIES
    VERSION ${mtsynth_VERSION}
    PUBLIC_HEADER inc/map_text_synthesizer.hpp)

pkg_check_modules(GLIB REQUIRED glib-2.0)
if (GLIB_FOUND)
    message(STATUS "glib:   YES")
else()
    message(STATUS "glib:   NO")
endif()

pkg_check_modules(PANGO REQUIRED pangocairo)
if (PANGO_FOUND)
    message(STATUS "pango:   YES")
    target_include_directories(mtsynth PUBLIC ${PANGO_INCLUDE_DIRS})
    target_link_libraries(mtsynth INTERFACE ${PANGO_LDFLAGS})
else()
    message(STATUS "pango:   NO")
endif()

pkg_check_modules(OPENCV REQUIRED opencv)
if (OPENCV_FOUND)
    message(STATUS "opencv:   YES")
    target_include_directories(mtsynth PUBLIC ${OPENCV_INCLUDE_DIRS})
    target_link_libraries(mtsynth INTERFACE ${OPENCV_LDFLAGS})
else()
    message(STATUS "opencv:   NO")
endif()

configure_file(mtsynth.pc.in mtsynth.pc @ONLY)

target_include_directories(mtsynth PRIVATE inc)
target_include_directories(mtsynth PRIVATE src)

install(TARGETS mtsynth
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include)

install(FILES ${CMAKE_BINARY_DIR}/mtsynth.pc
    DESTINATION lib/pkgconfig)
