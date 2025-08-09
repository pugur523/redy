# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

set(MODULE_NAME "ast")
set(MODULE_OBJECTS_NAME "ast_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  ast.cc 
  # base/base_node.cc

  # nodes/assignment_node.cc
  # nodes/binary_op_node.cc
  # nodes/block_node.cc
  # nodes/call_node.cc
  # nodes/expression_statement_node.cc
  # nodes/for_node.cc
  # nodes/function_node.cc
  # nodes/identifier_node.cc
  # nodes/if_node.cc
  # nodes/literal_node.cc
  # nodes/parameter_list_node.cc
  # nodes/parameter_node.cc
  # nodes/program_node.cc
  # nodes/return_node.cc
  # nodes/type_node.cc
  # nodes/unary_op_node.cc
  # nodes/variable_declaration_node.cc
  # nodes/while_node.cc
)

add_library(${MODULE_OBJECTS_NAME} OBJECT ${SOURCES})

setup_module(${MODULE_NAME} ${MODULE_OBJECTS_NAME}
  INCLUDE_DIRS ${PROJECT_INCLUDE_DIRECTORIES}
  C_CXX_COMPILE_OPTIONS ${PROJECT_C_CXX_COMPILE_OPTIONS}
  NASM_COMPILE_OPTIONS ${PROJECT_NASM_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS base ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()

