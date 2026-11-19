add_library(hierarchy STATIC
    src/base_task.cpp
    src/deadline_task.cpp
    src/periodic_task.cpp
    src/overdue_task.cpp)
target_include_directories(hierarchy PUBLIC include)

add_executable(test_hierarchy tests/test_hierarchy.cpp)
target_link_libraries(test_hierarchy PRIVATE hierarchy)
add_test(NAME stage3_hierarchy COMMAND test_hierarchy)

add_executable(stage1_task_class ../stage1_task_class/task.cpp)
add_test(NAME stage1_task_class COMMAND stage1_task_class)