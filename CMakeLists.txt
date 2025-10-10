cmake_minimum_required(VERSION 3.10.0)
project(tienda-ficticia VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Headers propios
include_directories(${CMAKE_SOURCE_DIR}/include)

# Ejecutable principal (agregá más si querés varios bins)
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
# Tests opcionales (si usás CTest)
enable_testing()
# add_executable(tests tests/main.cpp)
# add_test(NAME all_tests COMMAND tests)
