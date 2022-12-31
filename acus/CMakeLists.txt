## =============================================================================
##  Cento
##  Copyright : Oliver John Hitchcock
##  SPDX-License-Identifier: BSL-1.0
## =============================================================================

CPMAddPackage("gh:fmtlib/fmt#9.1.0")
CPMAddPackage("gh:foonathan/lexy#v2022.12.0")

add_executable(cento_acus main.cpp)

set_target_properties(cento_acus PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

target_include_directories(cento_acus
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/GSL/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/moneta/include")

target_link_libraries(cento_acus PRIVATE Cento::cento fmt::fmt foonathan::lexy)
