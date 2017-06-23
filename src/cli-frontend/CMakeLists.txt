
if(APPLE)
  set(CLIPBOARD_SRC )
  set(CLIPBOARD_LIBS blurpasteboard)
  add_library(blurpasteboard pasteboard.m)
  target_link_libraries(blurpasteboard objc)
  set(BLUR_SPECIFIC_FLAGS "-framework Foundation -framework AppKit")
elseif(GTK3)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
  include_directories(${GTK3_INCLUDE_DIRS})
  link_directories(${GTK3_LIBRARY_DIRS})
  add_definitions(${GTK3_CFLAGS_OTHER})

  set(CLIPBOARD_SRC gtk3_clipboard.c)
  set(CLIPBOARD_LIBS ${GTK3_LIBRARIES})
else()
  set(CLIPBOARD_SRC null_clipboard.c)
endif()

add_executable(blur
  create_context.c
  getfield.c
  ask_secret_key.c
  find_string_index.c
  run_options.c
  run_command.c
  blur.c

  # clipboard support
  ${CLIPBOARD_SRC}

  # commands
  cmd/add.c
  cmd/get.c
  cmd/info.c
  cmd/list.c
  cmd/remove.c

  # options
  options/unlock_from_stdin.c
  options/persistence_filepath.c
)

target_link_libraries(blur
  blurgather

  ${CLIPBOARD_LIBS}
)

set_target_properties(blur
  PROPERTIES LINK_FLAGS "${BLUR_SPECIFIC_FLAGS}"
)
