# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

set(TEST_NAME "${MAIN_EXECUTABLE_NAME_FROM_CONFIG}_test")

message(STATUS "Configuring ${TEST_NAME}...")

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cc
  ${PROJECT_SOURCE_DIR}/core/location_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/file_util_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/range_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/string_util_test.cc
  ${PROJECT_SOURCE_DIR}/core/base/vec_test.cc
  ${PROJECT_SOURCE_DIR}/core/diagnostics/system_info_test.cc

  ${PROJECT_SOURCE_DIR}/i18n/base/translator_test.cc

  ${PROJECT_SOURCE_DIR}/unicode/utf8/stream_test.cc

  ${PROJECT_SOURCE_DIR}/frontend/base/string/string_interner_test.cc
  ${PROJECT_SOURCE_DIR}/frontend/base/keyword/keyword_test.cc
  ${PROJECT_SOURCE_DIR}/frontend/base/token/token_stream_test.cc
  ${PROJECT_SOURCE_DIR}/frontend/base/token/token_test.cc

  # ${PROJECT_SOURCE_DIR}/frontend/ast/data/ast/node_test.cc

  ${PROJECT_SOURCE_DIR}/frontend/diagnostic/engine/diagnostic_engine_test.cc

  ${PROJECT_SOURCE_DIR}/frontend/processor/lexer/lexer_test.cc

  ${PROJECT_SOURCE_DIR}/frontend/processor/parser/parser_test.cc

  ${PROJECT_SOURCE_DIR}/frontend/processor/resolver/symbol/symbol_table_test.cc

  ${PROJECT_SOURCE_DIR}/frontend/pipeline/frontend_integration_test.cc
)

add_executable(${TEST_NAME} ${SOURCES})

target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_INCLUDE_DIRECTORIES})
target_compile_options(${TEST_NAME} PRIVATE
      $<$<COMPILE_LANGUAGE:C>:${PROJECT_C_CXX_COMPILE_OPTIONS}>
      $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_C_CXX_COMPILE_OPTIONS}>
)
target_compile_definitions(${TEST_NAME} PRIVATE ${PROJECT_COMPILE_DEFINITIONS})
target_link_options(${TEST_NAME} PRIVATE ${PROJECT_LINK_OPTIONS})
target_link_directories(${TEST_NAME} PRIVATE ${PROJECT_LINK_DIRECTORIES})
target_link_libraries(${TEST_NAME} PRIVATE
      core
      base
      diagnostic
      lexer
      ast
      parser
      resolver
      i18n
      unicode
      ${GTEST_LIBRARIES}
      ${GMOCK_LIBRARIES}
      ${PROJECT_LINK_LIBRARIES}
)

set_target_properties(${TEST_NAME} PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
)

if(ENABLE_RUN_TESTING_POST_BUILD)
  set(NEED_RUN TRUE)

  if(MINGW_BUILD)
    set(EXE_COMMAND wine $<TARGET_FILE:${TEST_NAME}>)
  else()
    set(EXE_COMMAND $<TARGET_FILE:${TEST_NAME}>)
  endif()

  if(NEED_RUN AND ENABLE_COVERAGE)
    set(COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage/${TEST_NAME}")
    FILE(MAKE_DIRECTORY ${COVERAGE_DIR})
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
      COMMENT "Run & generate coverage report"
      COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${COVERAGE_DIR}/profile.profraw ${EXE_COMMAND}
      COMMAND llvm-profdata merge -output=${COVERAGE_DIR}/merged.profdata ${COVERAGE_DIR}/profile.profraw
      COMMAND llvm-cov show $<TARGET_FILE:${TEST_NAME}>
      -instr-profile=${COVERAGE_DIR}/merged.profdata
      -format=html
      -output-dir=${COVERAGE_DIR}/html

      WORKING_DIRECTORY ${PROJECT_ROOT_DIR}
    )
    set(NEED_RUN FALSE)
  endif()

  if(NEED_RUN)
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
      COMMENT "Run tests if it has never been executed."
      COMMAND ${EXE_COMMAND}
    )
  endif()
endif()

if(ENABLE_INSTALL_TESTING)
  install(
    TARGETS ${TEST_NAME}
    RUNTIME
    COMPONENT Runtime
  )

  install(
    FILES "${CMAKE_PDB_OUTPUT_DIRECTORY}/${TEST_NAME}.pdb"
    DESTINATION bin
    COMPONENT Runtime
    OPTIONAL
  )
endif()

# add_test(
#   NAME ${TEST_NAME}
#   COMMAND ${TEST_NAME}
# )

if(ENABLE_VERBOSE)
  describe_target(${TEST_NAME})
endif()

# include(GoogleTest)
# gtest_discover_tests(${TEST_NAME})
