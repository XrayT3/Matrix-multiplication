cmake_minimum_required(VERSION 3.23)
project(SEM)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Release")

if(MSVC)
    add_compile_options("/W4")
    add_compile_options("/O2")
else()
    add_compile_options("-Wall" "-Wextra")
    add_compile_options("-O3")
endif()

add_executable(SEM.bin main.cpp)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(SEM.bin Threads::Threads)
