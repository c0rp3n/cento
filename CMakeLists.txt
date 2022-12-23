## =============================================================================
##  Cento
##  Copyright : Oliver John Hitchcock
##  SPDX-License-Identifier: BSL-1.0
## =============================================================================
cmake_minimum_required(VERSION 3.10)

project(Cento
    VERSION 0.0.01
    DESCRIPTION "An implementation of John K. Outsterhout's Corner Stiching."
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Do stuff depending on the compiler
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(cento_lib INTERFACE)
target_include_directories(
    cento_lib
        INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/centoConfigVersion.cmake"
    COMPATIBILITY SameMinorVersion
)

install(
    TARGETS cento_lib
    EXPORT centoTargets
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime
)

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/centoConfig.cmake.in"
                              "${PROJECT_BINARY_DIR}/centoConfig.cmake"
                              INSTALL_DESTINATION lib/cmake/cento)

install(EXPORT centoTargets DESTINATION lib/cmake/cento)
install(FILES "${PROJECT_BINARY_DIR}/centoConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/centoConfig.cmake"
        DESTINATION lib/cmake/cento)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

add_library(Cento::cento ALIAS cento_lib)

add_executable(cento_parse
    src/main.cpp)
set_target_properties(cento_parse PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(cento_parse
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/GSL/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/moneta/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/ut/include")
target_link_libraries(cento_parse PRIVATE Cento::cento)

include(CTest)
add_executable(cento_test
    test/main.cpp
    test/explore.cpp
    test/find.cpp
    test/join.cpp
    test/merge.cpp
    test/point.cpp
    test/rect.cpp
    test/split.cpp
    test/tile.cpp)
set_target_properties(cento_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(cento_test
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/GSL/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/moneta/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/ut/include")
target_link_libraries(cento_test PRIVATE Cento::cento)
add_test(NAME CentoTest COMMAND cento_test)
