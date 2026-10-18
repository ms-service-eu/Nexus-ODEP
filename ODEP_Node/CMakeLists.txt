add_executable(odep_node 
    src/main_node.cpp
)

target_include_directories(odep_node PRIVATE include)
# Verlinke die gemeinsame Core-Lib
target_link_libraries(odep_node PRIVATE odep_core)
