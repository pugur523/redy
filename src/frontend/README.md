# Redy Frontend


## Summary

This directory contains the source code of the redy frontend, which is the most complex part of our project since we currently delegate the backend compilation process to LLVM IR.
Here's a high-level overview of the components:


## Processors

### Lexer
Tokenizes source code and output as a `TokenStream`
If the source code is lexically invalid, it reports source error

### Parser
Syntax analysis - parses `TokenStream` into an `AstContext`

### Resolver
Performs semantic analysis on the `AstContext` to resolve names and types, desugar, and convert it to `HirContext`.

### HirAnalyzer
Performs semantic analysis on the `HirContext` to optimize and convert it to `MirContext`.

### MirAnalyzer
Performs semantic analysis on the `MirContext` to check lifetimes and borrowing for safety.


## Main Data Structures

### File
Owns source code content as a UTF-8 encoded string.
All processors use this interface to get a reference to the code, enabling zero-copy access for memory efficiency.

### Utf8Stream
Stream for utf8 string that provides `peek()`, `next()`, `rewind()`, etc.

### TokenStream
Vector of tokens that provides `peek()`, `next()`, `rewind()`, etc.

### AstContext
Abstract Syntax Tree - Preserves only syntactic information, not semantic information.

### HirContext
High-Level Intermediate Representation - Desugared and semantically analyzed code

### MirContext
Mid-Level Intermediate Representation - SSA-based IR with a control flow graph (CFG), which is more suitable for rigorous analysis.


## Pipeline

### Processing Flow

```mermaid
flowchart TD
    A(Source File) --> B[Lexer::tokenize]
    B --> C(TokenStream)
    C --> D[Parser::parse]
    D --> E(AstContext)
    E --> F[Resolver::analyze]
    F --> G(HirContext)
    G --> H[HirAnalyzer::convert]
    H --> I(MirContext)
    I --> J[MirAnalyzer::convert]
```

### Module Dependency Graph

```mermaid
graph TD

subgraph utilities
    B(core)
    C(unicode)
end

subgraph frontend
    D(base)
    E(diagnostic)     --> D
    F(lexer)          --> D
    G(parser)         --> D
    H(resolver)       --> D
    I(hir_analyzer)   --> D
    J(mir_analyzer)   --> D

    K(ast)            --> D
    L(hir)            --> D
    M(mir)            --> D


    G --> K
    H --> K
    H --> L
    I --> L
    I --> M
    J --> M
end

frontend --> utilities
```

> [!NOTE]
> **AST**: **A**bstract **S**yntax **T**ree<br/>
> **IR**: **I**ntermediate **R**epresentation<br/>
> **HIR**: **H**igh-level **IR**<br/>
> **MIR**: **M**iddle-level **IR**<br/>
> **CFG**: **C**ontrol **F**low **G**raph<br/>
> **SSA**: **S**tatic **S**ingle **A**ssignment<br/>
