
if(BSD OR APPLE)
  message(STATUS "fmemopen is not available on current platform: will download a wrapper for funopen")
  set(FMEMOPEN_LIBRARY fmemopen)

  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/fmemopen/src/fmemopen.c"
    COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/fmemopen"
    COMMAND git clone "https://github.com/NimbusKit/memorymapping" "${CMAKE_CURRENT_BINARY_DIR}/fmemopen")

  add_library(fmemopen
    "${CMAKE_CURRENT_BINARY_DIR}/fmemopen/src/fmemopen.c")
endif()

add_library(blurgather
  context.c
  stream.c
  map.c
  password.c
  repository.c
  persister.c
  array_repository.c
  msgpack_persister.c
  msgpack_serialize.c
  mcrypt_cryptor.c
  secret_key.c
  iv.c
  string.c
  cryptor.c
  encryption.c
  urandom_iv.c
  password_to_map.c
)

add_dependencies(blurgather msgpackc-target)

target_link_libraries(blurgather
  m
  mcrypt
  msgpackc
  ${FMEMOPEN_LIBRARY}
)

install(TARGETS blurgather DESTINATION lib)

add_subdirectory(cli-frontend)
