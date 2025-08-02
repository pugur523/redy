# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

set(MODULE_NAME "parser")
set(MODULE_OBJECTS_NAME "parser_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  parser.cc

  parse/assignment.cc
  parse/expression.cc
  parse/function_call.cc
  parse/function.cc
  parse/param_list.cc
  parse/param.cc
  parse/statement.cc
  parse/type.cc

  parse/expression/binary.cc
  parse/expression/primary.cc
  parse/expression/unary.cc

  parse/statement/assignment.cc
  parse/statement/block.cc
  parse/statement/expression.cc
  parse/statement/for.cc
  parse/statement/if.cc
  parse/statement/return.cc
  parse/statement/variable_declaration.cc
  parse/statement/while.cc
)

add_library(${MODULE_OBJECTS_NAME} OBJECT ${SOURCES})

setup_module(${MODULE_NAME} ${MODULE_OBJECTS_NAME}
  INCLUDE_DIRS ${PROJECT_INCLUDE_DIRECTORIES}
  COMPILE_OPTIONS ${PROJECT_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS base ast ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()

