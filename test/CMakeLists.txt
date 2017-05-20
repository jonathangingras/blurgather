
add_library(bg_tests mocks.c)

function(add_test_case test_case)
  add_test_executable(${test_case}_test ${test_case}_test.c)
  target_link_libraries(${test_case}_test blurgather bg_tests)
endfunction()

add_test_case(password)
add_test_case(array_repository)
#add_test_case(password_msgpack_persister_integration)
#add_test_case(mcrypt_cryptor)
add_test_case(mem_stream)
add_test_case(file_stream)
add_test_case(string)
