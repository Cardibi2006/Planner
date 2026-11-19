add_library(planner_core STATIC
    src/plan.cpp
    src/scheduler.cpp)
target_link_libraries(planner_core PUBLIC hierarchy)
target_include_directories(planner_core PUBLIC include)

add_executable(test_container tests/test_container.cpp)
target_link_libraries(test_container PRIVATE planner_core)
add_test(NAME stage6_container COMMAND test_container)