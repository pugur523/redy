# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

set(MODULE_NAME "parser")
set(MODULE_OBJECTS_NAME "parser_objects")

message(STATUS "Configuring ${MODULE_NAME} module...")

set(SOURCES
  parser.cc

  expression.cc
  statement.cc

  chore/attribute_use.cc
  chore/capture.cc
  chore/enum_variant.cc
  chore/expression_sequence.cc
  chore/field.cc
  chore/parameter.cc
  chore/storage_attributes.cc
  chore/type_reference.cc

  expression/array.cc
  expression/await.cc
  expression/binary.cc
  expression/block.cc
  expression/break.cc
  expression/closure.cc
  expression/construct.cc
  expression/continue.cc
  expression/field_access.cc
  expression/for.cc
  expression/function_call.cc
  expression/function_macro_call.cc
  expression/grouped.cc
  expression/if.cc
  expression/index.cc
  expression/literal.cc
  expression/loop.cc
  expression/match.cc
  expression/method_call.cc
  expression/method_macro_call.cc
  expression/path.cc
  expression/postfix.cc
  expression/primary.cc
  expression/range.cc
  expression/return.cc
  expression/tuple.cc
  expression/unary.cc
  expression/while.cc

  statement/assign.cc
  statement/attribute.cc
  statement/declaration.cc
  statement/expression.cc
  statement/use.cc

  statement/declaration/enum.cc
  statement/declaration/function.cc
  statement/declaration/impl.cc
  statement/declaration/module.cc
  statement/declaration/redirect.cc
  statement/declaration/struct.cc
  statement/declaration/trait.cc
  statement/declaration/union.cc
)

add_library(${MODULE_OBJECTS_NAME} OBJECT ${SOURCES})

setup_module(${MODULE_NAME} ${MODULE_OBJECTS_NAME}
  INCLUDE_DIRS ${PROJECT_INCLUDE_DIRECTORIES}
  C_CXX_COMPILE_OPTIONS ${PROJECT_C_CXX_COMPILE_OPTIONS}
  NASM_COMPILE_OPTIONS ${PROJECT_NASM_COMPILE_OPTIONS}
  COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS}
  LINK_OPTIONS ${PROJECT_LINK_OPTIONS}
  LINK_DIRS ${PROJECT_LINK_DIRECTORIES}
  LINK_LIBS core base ast diagnostic ${PROJECT_LINK_LIBRARIES}
)

if(ENABLE_VERBOSE)
  describe_target(${MODULE_NAME})
endif()
