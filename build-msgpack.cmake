ExternalProject_Add(msgpack-c
  GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
  GIT_TAG "cpp-2.0.0"
  #INSTALL_DIR ${msgpack_PREFIX}
  #-DCMAKE_INSTALL_PREFIX=${EXTERN_PREFIX}
  CMAKE_ARGS ${CMAKE_ARGS} -DMSGPACK_BUILD_EXAMPLES=OFF -DMSGPACK_ENABLE_CXX=OFF -DMSGPACK_ENABLE_SHARED=OFF -DMSGPACK_BUILD_TESTS=OFF
  INSTALL_COMMAND ""
  )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/msgpack-c-prefix/src/msgpack-c/include)

set(msgpack-static ${CMAKE_CURRENT_BINARY_DIR}/msgpack-c-prefix/src/msgpack-c-build/libmsgpackc.a)
