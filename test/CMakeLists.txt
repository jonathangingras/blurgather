function(add_gtest_case test_case)
    add_executable(${test_case}_test
        ${test_case}_test.cc
    )
    target_link_libraries(${test_case}_test
        blurgather

        gtest
        gtest_main
        pthread
    )
endfunction()

function(add_test_case test_case)
  add_executable(${test_case}_test ${test_case}_test.c)
  target_link_libraries(${test_case}_test blurgather)
endfunction()

add_test_case(password)
add_test_case(password_msgpack_persister)
add_test_case(password_msgpack_persister_integration)
#add_gtest_case(mcrypt_cryptor)
