add_library(kiki_utilities
    utilities.c
)
install(TARGETS kiki_utilities DESTINATION lib)

add_library(kiki_pwd_mng
    kiPassword.c
    kiPasswordFactory.c
    kiPasswordMsgPackPersister.c
    msgpack_serialize.c
    kiMCryptEncryptor.c
    secret_key.c
    MCryptIV.c
)

target_link_libraries(kiki_pwd_mng
    kiki_utilities
    uuid
    mcrypt
    ${msgpack-static}
)
install(TARGETS kiki_pwd_mng DESTINATION lib)

add_subdirectory(cli-frontend)