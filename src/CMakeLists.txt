add_library(kiki_utilities
    utilities.c
)
install(TARGETS kiki_utilities DESTINATION lib)

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
)

target_link_libraries(blurgather
    kiki_utilities
    mcrypt
    ${msgpack-static}
)
install(TARGETS blurgather DESTINATION lib)

add_subdirectory(cli-frontend)
