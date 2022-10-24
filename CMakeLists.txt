cmake_minimum_required(VERSION 3.23)
project(assignment2 C)

add_executable(assignment2 queue.h sut.h sut.c)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(assignment2 PRIVATE Threads::Threads)
