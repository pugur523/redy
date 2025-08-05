# Redy Frontend

## Summary

This directory contains the source code of the redy frontend, which is the most complex part of our project since we currently delegate the backend compilation process to LLVM IR.  
Here's a high-level overview of the components:

- **lexer**: Lexical analysis — converts source code into a stream of tokens
- **parser**: Syntax analysis — parses the token stream into an Abstract Syntax Tree (AST)
- **sema**: Semantic analysis — resolves names and types in the AST and lowers it to HIR
- **hir**: High-level Intermediate Representation — desugared and semantically analyzed code
- **mir**: Mid-level Intermediate Representation — SSA-based IR with control-flow graph

## Pipeline

### Processing Flow

```mermaid
flowchart TD
    A(Source File) --> B[Lexer::lex_all]
    B --> C(TokenStream)
    C --> D[Parser::parse_all]
    D --> E(AstContext)
    E --> F[SemanticAnalyzer::sema_all]
    F --> G(Hir with semantic info)
    G --> H(Mir with CFG + SSA)
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
    E(diagnostic) --> D
    F(lexer)      --> D
    G(ast)        --> D
    H(parser)     --> D
    I(sema)       --> D
    J(hir)        --> D
    K(mir)        --> D


    H --> G
    I --> G
end

frontend --> utilities
```

> [!NOTE]
> **ast**: **A**bstract **S**yntax **T**ree<br/>
> **sema**: **Sem**antic **A**nalysis<br/>
> **ir**: **I**ntermediate **R**epresentation<br/>
> **hir**: **H**igh-level **IR**<br/>
> **mir**: **M**iddle-level **IR**<br/>
> **cfg**: **C**ontrol **F**low **G**raph<br/>
> **ssa**: **S**tatic **S**ingle **A**ssignment<br/>
