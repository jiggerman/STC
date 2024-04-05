project(STC)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_library(stc stc.cpp)
add_executable(main main.cpp)
target_link_libraries(main stc Threads::Threads)
