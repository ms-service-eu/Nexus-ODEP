add_executable(odep_controller 
    src/main_controller.cpp
)

target_link_libraries(odep_controller PRIVATE odep_core)
