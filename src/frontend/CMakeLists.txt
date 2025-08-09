# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

# add_subdirectory(pipeline)

add_subdirectory(base)
add_subdirectory(diagnostic)

# data structures
add_subdirectory(ast)
# add_subdirectory(hir)
# add_subdirectory(mir)

# processors
add_subdirectory(lexer)
add_subdirectory(parser)
# add_subdirectory(ast_analyzer)
# add_subdirectory(hir_analyzer)
# add_subdirectory(mir_analyzer)
