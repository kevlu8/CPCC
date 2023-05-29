cmake_minimum_required(VERSION 3.10)

project(CPCC VERSION 0.2.0)

add_definitions("-DCPCC_VER=\"${PROJECT_VERSION}\"")
add_definitions("-DCPCC_INCLUDE_DIR=\"/usr/local/include/cpcc/\"")

add_executable(cpcc src/main.cpp src/warn.cpp)

target_compile_options(cpcc PRIVATE -O3 -std=c++20)

find_package(Boost 1.75.0 REQUIRED COMPONENTS system)
target_link_libraries(cpcc PRIVATE Boost::system
	-lboost_system
	-lboost_program_options
)

install(TARGETS cpcc DESTINATION bin)
install(DIRECTORY src/cpcc/ DESTINATION /usr/local/include/cpcc)
