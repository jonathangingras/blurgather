cmake_minimum_required(VERSION 3.1)
project(blurgather)

#common hierarchy output structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#external dependencies
include(CMakeShortcuts/ExternalProjectLocal.cmake)
include(CMakeShortcuts/CTestShortcuts.cmake)

ExternalProject_Add_Local(msgpackc-target
  GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
  GIT_TAG "cpp-2.1.1"
  CMAKE_ARGS "${CMAKE_ARGS} -DMSGPACK_BUILD_EXAMPLES=OFF -DMSGPACK_ENABLE_CXX=OFF -DMSGPACK_ENABLE_SHARED=OFF -DCMAKE_C_FLAGS=-w"
)

#includes
include_directories(include)
include_directories(include/blurgather)

#install paths
install(DIRECTORY include/blurgather DESTINATION include)

#sub directories
add_subdirectory(src)

#do not compile tests by default
option(BUILD_TESTING "compile tests" OFF)

if(BUILD_TESTING)
  enable_testing()

  ExternalProject_Add_Local(prufen-target
    GIT_REPOSITORY "https://github.com/jonathangingras/prufen"
    GIT_TAG "v0.1.0"
  )

  add_subdirectory(test)
endif()
