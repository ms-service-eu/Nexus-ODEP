if not exist build mkdir build
cd build
cmake -G Ninja ..
cmake --build .
./nexus_odep_sim
