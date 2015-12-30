set(SWEETGREEN_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/extern)

ExternalProject_Add(sweetgreen
  GIT_REPOSITORY https://github.com/jonathangingras/sweetgreen.git
  INSTALL_DIR ${SWEETGREEN_PREFIX}
  CMAKE_ARGS ${CMAKE_ARGS} -DCMAKE_INSTALL_PREFIX=${SWEETGREEN_PREFIX}
)

include_directories(${SWEETGREEN_PREFIX}/include)
