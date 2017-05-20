
add_library(blurgather
  context.c
  stream.c
  password.c
  repository.c
  array_repository.c
  msgpack_persister.c
  msgpack_serialize.c
  mcrypt_cryptor.c
  secret_key.c
  iv.c
  string.c
  cryptor.c
  urandom_iv.c
)

target_link_libraries(blurgather
    mcrypt
    ${msgpack-static}
)

install(TARGETS blurgather DESTINATION lib)

#add_subdirectory(cli-frontend)
