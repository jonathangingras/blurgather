ExternalProject_Add(sweetgreen
  GIT_REPOSITORY https://github.com/jonathangingras/sweetgreen.git
  #INSTALL_DIR ${SWEETGREEN_PREFIX}
  #CMAKE_ARGS ${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX=${EXTERN_PREFIX}
  INSTALL_COMMAND ""
  )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/sweetgreen-prefix/src/sweetgreen/include)
