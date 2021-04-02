cmake_minimum_required(VERSION 3.14)
project("project_wifi")
set(CMAKE_CXX_STANDARD 11)
add_subdirectory(libtins)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(projectwifi src/main.cpp)
target_link_libraries(projectwifi tins Threads::Threads)

add_executable(sniffer src/sniffer.cpp)
target_link_libraries(sniffer tins)

add_executable(data_sender src/data_sender.cpp)
target_link_libraries(data_sender tins Threads::Threads)

add_executable(beacon src/beacon.cpp)
target_link_libraries(beacon tins)