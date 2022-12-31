## =============================================================================
##  Cento
##  Copyright : Oliver John Hitchcock
##  SPDX-License-Identifier: BSL-1.0
## =============================================================================
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Cento
    VERSION 0.1.00
    DESCRIPTION "An implementation of John K. Outsterhout's Corner Stiching."
    LANGUAGES CXX
)

# determine whether this is a standalone project or included by other projects
if (NOT DEFINED CENTO_STANDALONE_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(CENTO_STANDALONE_PROJECT ON)
    else()
        set(CENTO_STANDALONE_PROJECT OFF)
    endif ()
endif()

option(CENTO_ACUS  "Build the Acus application" ${CENTO_STANDALONE_PROJECT})
option(CENTO_TESTS "Build tests"                ${CENTO_STANDALONE_PROJECT})

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

if(CENTO_ACUS)
    include(cmake/CPM.cmake)
    add_subdirectory(acus)
endif()

if (CENTO_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

if(CENTO_ACUS AND CENTO_TESTS)
    add_test(NAME CentoAcusExample1 COMMAND cento_acus "${CMAKE_SOURCE_DIR}/testcase/example1.txt")
    add_test(NAME CentoAcusExample2 COMMAND cento_acus "${CMAKE_SOURCE_DIR}/testcase/example2.txt")
    add_test(NAME CentoAcusExample3 COMMAND cento_acus "${CMAKE_SOURCE_DIR}/testcase/example3.txt")
endif()
