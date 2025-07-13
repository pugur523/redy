# Frontend

## Pipeline

### State Diagram

```mermaid
stateDiagram-v2
    [*] --> FileManager
    FileManager --> Lexer
    Lexer --> TokenStream
    TokenStream --> Parser
    Parser --> AST
    AST --> Sema
    Sema --> HIR
    HIR --> MIR
    MIR --> CFG_SSA
    CFG_SSA --> [*]
```

- **AST**: Abstract Syntax Tree
- **Sema**: Semantic Analysis
- **IR**: Intermediate Representation
- **HIR**: High-Level IR
- **MIR**: Middle-Level IR
- **CFG**: Control Flow Graph
- **SSA**: Static Single Assignment
