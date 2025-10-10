cmake_minimum_required(VERSION 3.20)
project(TIENDA_FICTICIA LANGUAGES CXX)

# ====== Config global ======
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Salidas prolijas (bin/lib) dentro del build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ====== Core library ======
# Toma TODOS los .cpp dentro de source/** automáticamente
file(GLOB_RECURSE CORE_SOURCES
     CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/source/*.cpp")

add_library(core ${CORE_SOURCES})

# Headers públicos en headers/
target_include_directories(core PUBLIC
  ${CMAKE_SOURCE_DIR}/headers
)

# Warnings fuertes
if (MSVC)
  target_compile_options(core PRIVATE /W4 /permissive- /Zc:__cplusplus /std:c++20)
else()
  target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ====== Executable principal ======
# Si tu main está en la raíz como main.cpp, dejá esto así.
# Si lo pusiste en otro lado, cambiá la ruta acá una sola vez.
add_executable(app ${CMAKE_SOURCE_DIR}/main.cpp)
target_link_libraries(app PRIVATE core)

# En Windows, dejá el exe en build/bin
set_target_properties(app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ====== (Opcional) Tests ======
option(BUILD_TESTS "Build tests with CTest" OFF)
if (BUILD_TESTS)
  enable_testing()
  file(GLOB_RECURSE TEST_SOURCES
       CONFIGURE_DEPENDS
       "${CMAKE_SOURCE_DIR}/tests/*.cpp")
  add_executable(tests ${TEST_SOURCES})
  target_link_libraries(tests PRIVATE core)
  if (MSVC)
    target_compile_options(tests PRIVATE /W4 /permissive- /Zc:__cplusplus /std:c++20)
  else()
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
  endif()
  add_test(NAME all_tests COMMAND tests)
endif()

# ====== (Opcional) Target de formato con clang-format ======
# Para usar: cmake --build build --target format
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if (CLANG_FORMAT_EXE)
  file(GLOB_RECURSE ALL_CXX CONFIGURE_DEPENDS
       "${CMAKE_SOURCE_DIR}/source/*.cpp"
       "${CMAKE_SOURCE_DIR}/headers/*.h"
       "${CMAKE_SOURCE_DIR}/headers/*.hpp")
  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CXX}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formateando código con clang-format")
endif()
