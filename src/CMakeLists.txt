add_library(bg_utilities
    utilities.c
)
install(TARGETS bg_utilities DESTINATION lib)

add_library(blurgather
    stream.c
    password.c
    password_factory.c
    repository.c
    password_msgpack_persister.c
    msgpack_serialize.c
    mcrypt_cryptor.c
    secret_key.c
    iv.c
    mcrypt_iv.c
    string.c
)

target_link_libraries(blurgather
    bg_utilities
    mcrypt
    ${msgpack-static}
)
install(TARGETS blurgather DESTINATION lib)

add_subdirectory(cli-frontend)
