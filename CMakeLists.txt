cmake_minimum_required(VERSION 3.22)
project(untitled8)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)


add_executable(untitled8 main.cpp login.cpp login.h file.cpp Path_QZA.cpp Path_QZA.h dataStore.cpp dataStore.h)
add_executable(learn_place path_test.cpp abc.cpp test/testencrypt.cpp)
target_link_libraries(untitled8 PUBLIC Threads::Threads)