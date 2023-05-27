cmake_minimum_required(VERSION 3.10)

project(CPCC VERSION 0.0.4)

add_definitions("-DCPCC_VER=\"${PROJECT_VERSION}\"")
add_definitions("-DCPCC_INCLUDE_DIR=\"/usr/local/include/cpcc/\"")

add_executable(cpcc src/main.cpp)

target_compile_options(cpcc PRIVATE -O3 -std=c++20)

install(TARGETS cpcc DESTINATION bin)
install(DIRECTORY src/cpcc/ DESTINATION /usr/local/include/cpcc)