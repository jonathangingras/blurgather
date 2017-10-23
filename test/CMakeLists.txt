
add_library(bg_tests mocks.c)
add_dependencies(bg_tests sweetgreen-target)
target_link_libraries(bg_tests blurgather)

function(add_test_case test_case)
  add_test_executable(${test_case}_test ${test_case}_test.c)
  target_link_libraries(${test_case}_test bg_tests)
endfunction()

add_test_case(password)
add_test_case(array_repository)
add_test_case(msgpack_persister)
add_test_case(mcrypt_cryptor)
add_test_case(mem_stream)
add_test_case(file_stream)
add_test_case(string)
add_test_case(integration)
add_test_case(map)
add_test_case(encryption)

get_filename_component(blur_test_script_path "blur_test.py" ABSOLUTE)
add_test("blur_test" python3 ${blur_test_script_path})
