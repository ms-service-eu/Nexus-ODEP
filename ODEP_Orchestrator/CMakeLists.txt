add_executable(odep_orchestrator 
    src/main_orchestrator.cpp
)


target_include_directories(odep_orchestrator PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ODEP_Node/include"
)

target_link_libraries(odep_orchestrator PRIVATE odep_core)
