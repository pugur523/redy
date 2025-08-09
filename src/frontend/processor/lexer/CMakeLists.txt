# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

set(MODULE_NAME "lexer")
set(MODULE_OBJECTS_NAME "lexer_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  lexer.cc

  ascii.cc
  other.cc
  unicode.cc

  literal/char.cc
  literal/numeric.cc
  literal/str.cc
)

add_library(${MODULE_OBJECTS_NAME} OBJECT ${SOURCES})

setup_module(${MODULE_NAME} ${MODULE_OBJECTS_NAME}
  INCLUDE_DIRS ${PROJECT_INCLUDE_DIRECTORIES}
  C_CXX_COMPILE_OPTIONS ${PROJECT_C_CXX_COMPILE_OPTIONS}
  NASM_COMPILE_OPTIONS ${PROJECT_NASM_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS base unicode ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()
