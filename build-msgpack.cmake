ExternalProject_Add(msgpack-c
  GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
  GIT_TAG "cpp-1.3.0"
  INSTALL_DIR ${msgpack_PREFIX}
  CMAKE_ARGS ${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX=${EXTERN_PREFIX} -DMSGPACK_BUILD_EXAMPLES=OFF -DMSGPACK_ENABLE_CXX=OFF -DMSGPACK_ENABLE_SHARED=OFF
)

set(msgpack-static ${EXTERN_PREFIX}/lib/libmsgpack.a)
