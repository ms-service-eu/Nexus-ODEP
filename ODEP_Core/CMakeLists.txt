add_library(odep_core INTERFACE)

# Zwingt CMake, den absoluten Systempfad für die Includes zu nutzen
target_include_directories(odep_core INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
