cmake_minimum_required (VERSION 2.6)

project (betterss)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(betterss src/fpng.cpp src/encoder.cpp src/screenmanager.cpp src/main.cpp)
target_link_libraries(betterss screen)