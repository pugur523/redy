# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

set(MODULE_NAME "base")
set(MODULE_OBJECTS_NAME "base_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  data/string_arena.cc
  keyword/keyword.cc
  token/token.cc
  token/token_stream.cc

  string/string_interner.cc
)

add_library(${MODULE_OBJECTS_NAME} OBJECT ${SOURCES})

setup_module(${MODULE_NAME} ${MODULE_OBJECTS_NAME}
  INCLUDE_DIRS ${PROJECT_INCLUDE_DIRECTORIES}
  C_CXX_COMPILE_OPTIONS ${PROJECT_C_CXX_COMPILE_OPTIONS}
  NASM_COMPILE_OPTIONS ${PROJECT_NASM_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS core unicode ${ZLIB_LIBRARIES} ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()
