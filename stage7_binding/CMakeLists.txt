add_library(planner SHARED cpp/c_api.cpp)
target_link_libraries(planner PRIVATE planner_core)
target_include_directories(planner PRIVATE cpp)

set_target_properties(planner PROPERTIES
    OUTPUT_NAME "planner"
    PREFIX "lib"
    WINDOWS_EXPORT_ALL_SYMBOLS ON)