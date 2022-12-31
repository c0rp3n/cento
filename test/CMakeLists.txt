## =============================================================================
##  Cento
##  Copyright : Oliver John Hitchcock
##  SPDX-License-Identifier: BSL-1.0
## =============================================================================

add_executable(cento_test
    main.cpp
    explore.cpp
    find.cpp
    insert.cpp
    join.cpp
    merge.cpp
    point.cpp
    rect.cpp
    remove.cpp
    split.cpp
    tile.cpp)

set_target_properties(cento_test PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

target_include_directories(cento_test
    PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/GSL/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/moneta/include"
        "${PROJECT_SOURCE_DIR}/thirdparty/ut/include")

target_link_libraries(cento_test PRIVATE Cento::cento)

add_test(NAME CentoTest COMMAND cento_test)
