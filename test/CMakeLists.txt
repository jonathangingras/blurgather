
function(add_test_case test_case)
  add_executable(${test_case}_test ${test_case}_test.c)
  target_link_libraries(${test_case}_test blurgather)
endfunction()

add_test_case(password)
add_test_case(password_msgpack_persister)
add_test_case(password_msgpack_persister_integration)
add_test_case(mcrypt_cryptor)
add_test_case(mem_stream)
add_test_case(file_stream)
