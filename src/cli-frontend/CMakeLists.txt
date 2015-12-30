
if(APPLE)
  set(CLIPBOARD_SRC )
  set(CLIPBOARD_LIBS blurpasteboard) 
  add_library(blurpasteboard pasteboard.m) 
  target_link_libraries(blurpasteboard objc)
  set(BLUR_SPECIFIC_FLAGS "-framework Foundation -framework AppKit")
elseif(UNIX)
  set(CLIPBOARD_SRC_FILE xclipboard.c)
endif()

add_executable(blur
  blur.c
  ${CLIPBOARD_SRC}
)

target_link_libraries(blur
  blurgather
  ${CLIPBOARD_LIBS}
)

set_target_properties(blur PROPERTIES
  LINK_FLAGS "${BLUR_SPECIFIC_FLAGS}"
)
