cmake_minimum_required(VERSION 3.20)

project(Sven
    VERSION      0.4.0
    DESCRIPTION  "A simple, beginner-friendly 2D game library"
    LANGUAGES    CXX
)

# ─── C++ standard ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# ─── Build type default ───────────────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ─── Dependencies ─────────────────────────────────────────────────────────────
# We look for SDL3, SDL3_image, and SDL3_mixer using their official CMake
# packages. Install them first:
#   Linux (apt):  sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-mixer-dev
#   macOS:        brew install sdl3 sdl3_image sdl3_mixer
#   Windows:      vcpkg install sdl3 sdl3-image sdl3-mixer
find_package(SDL3       REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_mixer REQUIRED CONFIG)

# ─── Sven library target ──────────────────────────────────────────────────────
add_library(sven STATIC
    src/core.cpp
    src/window.cpp
    src/graphics.cpp
    src/input.cpp
    src/texture.cpp
    src/audio.cpp
)

target_include_directories(sven
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src   # for sven_internal.h
)

target_link_libraries(sven PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer)

# Warn loudly in Debug mode so we catch issues early.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(sven PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:
            -Wall -Wextra -Wpedantic -Wshadow
        >
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4
        >
    )
endif()

# ─── Examples ─────────────────────────────────────────────────────────────────
option(SVEN_BUILD_EXAMPLES "Build the example programs" ON)

if(SVEN_BUILD_EXAMPLES)
    add_executable(hello examples/hello.cpp)
    target_link_libraries(hello PRIVATE sven)

    add_executable(movement examples/movement.cpp)
    target_link_libraries(movement PRIVATE sven)

    add_executable(sprite examples/sprite.cpp)
    target_link_libraries(sprite PRIVATE sven)

    add_executable(audio examples/audio.cpp)
    target_link_libraries(audio PRIVATE sven)
endif()

# ─── Demos ────────────────────────────────────────────────────────────────────
option(SVEN_BUILD_DEMOS "Build the demo games" ON)

if(SVEN_BUILD_DEMOS)
    add_executable(pong demos/pong/pong.cpp)
    target_link_libraries(pong PRIVATE sven)
endif()

# ─── Tests ────────────────────────────────────────────────────────────────────
option(SVEN_BUILD_TESTS "Build the unit tests" ON)

if(SVEN_BUILD_TESTS)
    enable_testing()

    add_executable(test_vec2 tests/test_vec2.cpp)
    target_link_libraries(test_vec2 PRIVATE sven)
    add_test(NAME Vec2 COMMAND test_vec2)

    add_executable(test_rect tests/test_rect.cpp)
    target_link_libraries(test_rect PRIVATE sven)
    add_test(NAME Rect COMMAND test_rect)

    add_executable(test_resource_failure tests/test_resource_failure.cpp)
    target_link_libraries(test_resource_failure PRIVATE sven)
    add_test(NAME ResourceFailure COMMAND test_resource_failure)

    # Headless drivers so tests run without a display or sound card (CI).
    set_tests_properties(ResourceFailure PROPERTIES
        ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy"
    )
endif()

# ─── Install rules ────────────────────────────────────────────────────────────
# Running `cmake --install build` copies files to the right places and
# generates a CMake package so other projects can do:
#
#   find_package(Sven CONFIG REQUIRED)
#   target_link_libraries(mygame PRIVATE Sven::sven)
#
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS sven
    EXPORT  SvenTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES include/sven.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT SvenTargets
    FILE        SvenTargets.cmake
    NAMESPACE   Sven::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Sven
)

# SvenConfig.cmake — re-finds SDL3/SDL3_image/SDL3_mixer (sven's PUBLIC
# dependencies) so find_package(Sven) "just works" for consumers, then
# includes the exported targets above.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SvenConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SvenConfig.cmake
    @ONLY
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SvenConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SvenConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SvenConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Sven
)

# ─── Summary ──────────────────────────────────────────────────────────────────
message(STATUS "")
message(STATUS "Sven ${PROJECT_VERSION}")
message(STATUS "  Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Examples   : ${SVEN_BUILD_EXAMPLES}")
message(STATUS "  Demos      : ${SVEN_BUILD_DEMOS}")
message(STATUS "  Tests      : ${SVEN_BUILD_TESTS}")
message(STATUS "")
