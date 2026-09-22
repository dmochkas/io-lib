target_sources(${IO_LIB} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/io_base.c")

if (IO_PLATFORM STREQUAL "posix")
	target_sources(${IO_LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/io_posix.c")
else ()
	message(FATAL_ERROR "IO_PLATFORM '${IO_PLATFORM}' is not implemented yet")
endif ()
