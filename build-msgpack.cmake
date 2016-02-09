set(msgpack_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/extern)

ExternalProject_Add(msgpack-c
  GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
  GIT_TAG "cpp-1.3.0"
  INSTALL_DIR ${msgpack_PREFIX}
  CMAKE_ARGS ${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX=${msgpack_PREFIX} -DMSGPACK_BUILD_EXAMPLES=OFF -DMSGPACK_ENABLE_CXX=OFF -DMSGPACK_ENABLE_SHARED=OFF
)

include_directories(${msgpack_PREFIX}/include)
set(msgpack-static ${msgpack_PREFIX}/lib/libmsgpack.a)
