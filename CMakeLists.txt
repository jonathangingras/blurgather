cmake_minimum_required(VERSION 3.1)
project(blurgather)

#common hierarchy output structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXTERN_PREFIX ${CMAKE_SOURCE_DIR}/extern)
file(MAKE_DIRECTORY ${EXTERN_PREFIX})

if(ANDROID)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/msgpack-c/include)
	set(msgpack-static ${CMAKE_CURRENT_SOURCE_DIR}/msgpack-c/android_build/${ANDROID_ABI}/libmsgpack.a)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libuuid/include)
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libuuid/android_build/${ANDROID_ABI}/src)
else()
  #external dependencies
  include(ExternalProject)
  include(build-msgpack.cmake)
  include(locate-sweetgreen.cmake)
endif()

#includes
include_directories(${EXTERN_PREFIX}/include)
include_directories(include)

#install paths
install(DIRECTORY include/blurgather DESTINATION include)

#do not compile tests by default
option(WITH_TESTS "compile tests" OFF)

#subdirectories
add_subdirectory(src)
if(WITH_TESTS)
  add_subdirectory(test)
endif()
