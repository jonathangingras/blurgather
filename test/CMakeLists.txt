function(add_pwd_mng_test className)
    add_executable(${className}Test
        ${className}Test.cc
    )
    target_link_libraries(${className}Test
        kiki_pwd_mng

        gtest
        gtest_main
        pthread
    )
endfunction()

add_pwd_mng_test(kiPassword)
add_pwd_mng_test(kiPasswordMsgPackPersister)
add_pwd_mng_test(kiPasswordMCryptEncryptor)