cmake_minimum_required(VERSION 2.8)
project(blurgather)

#common hierarchy output structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
include_directories(include)
include_directories(include/kiki)
include_directories(include/blurgather)

#install paths
install(DIRECTORY include/blurgather DESTINATION include)
install(DIRECTORY include/kiki DESTINATION include)

#subdirectories
add_subdirectory(src)
if(WITH_TESTS)
  add_subdirectory(test)
endif()
