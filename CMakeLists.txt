cmake_minimum_required(VERSION 3.1)
project(kiki_pwd_mng)
include(ExternalProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(build-msgpack.cmake)

include_directories(include)
include_directories(include/kiki)
include_directories(include/kiki/pwd_mng)
install(DIRECTORY include/kiki DESTINATION include)

add_subdirectory(src)
add_subdirectory(test)