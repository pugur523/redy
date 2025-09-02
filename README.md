<div align="center">
  <img src="src/build/resources/assets/logo_r.svg" width="192" alt="redy">
  <h1>redy</h1>

  A new programming language that prioritizes performance and memory safety.

  [![CI](https://github.com/pugur523/redy/actions/workflows/ci.yml/badge.svg)](https://github.com/pugur523/redy/actions/workflows/ci.yml)
  [![Issues](https://img.shields.io/github/issues/pugur523/redy.svg)](https://github.com/pugur523/redy/issues)
  [![License](https://img.shields.io/badge/License-Apache%20License%20Version%202.0-red)](LICENSE)<br/>
  [![C](https://img.shields.io/badge/C-blue?logo=c)](https://www.c-language.org/)
  [![C++](https://img.shields.io/badge/C++-blue?logo=cplusplus)](https://isocpp.org/)
  [![LLVM](https://img.shields.io/badge/LLVM-21-emerald?logo=llvm)](https://llvm.org/docs/index.html)
  [![Document](https://img.shields.io/badge/Document-purple)](https://pugur523.github.io/redy_doc/)
  [![日本語版](https://img.shields.io/badge/日本語版-blue)](README.ja.md)
</div>

> [!NOTE]
> This project is a work in progress and cannot yet compile a **Hello World** program.

---

# ☄ Overview

**redy** is a new programming language to achieve both high performance and robust safety.

It emphasizes performance in all aspects, including reducing computation, memory usage, and improving the cache hit rate, as well as minimizing the size of compiled binaries, which is often overlooked in an era of abundant computing resources.

---

# 📖 Table Of Contents

- [☄ Overview](#-overview)
- [📖 Table Of Contents](#-table-of-contents)
- [🔥 Features](#-features)
    - [🛡️ Complete Memory Safety](#️-complete-memory-safety)
    - [🚫 Zero-Cost Access Control](#-zero-cost-access-control)
    - [☄ Blazing Fast](#-blazing-fast)
    - [🍃 No Lifetime Annotation Required](#-no-lifetime-annotation-required)
- [📦 Quick Start](#-quick-start)
  - [🍭 Install redy](#-install-redy)
  - [✏ Coding First Program](#-coding-first-program)
- [🚀 Roadmap to First Stable Release](#-roadmap-to-first-stable-release)
- [🪪 License](#-license)
- [❤️ Credits](#️-credits)

---

# 🔥 Features

> [!NOTE]
> Most features have not yet been implemented, but the design is fully defined.

### 🛡️ Complete Memory Safety

**redy** uses an ownership system inspired by Rust. All heap memory is automatically freed when it goes out of scope, completely preventing memory leaks.

### 🚫 Zero-Cost Access Control

All potential illegal memory access is detected at compile time, eliminating runtime overhead. Say goodbye to segmentation faults!

### ☄ Blazing Fast

Runtime costs, such as boundary checks, are minimized through various compile-time analysis methods to achieve the **best possible performance**.

### 🍃 No Lifetime Annotation Required

The compiler uses **Control Flow Graph(CFG)** and **Static Single Assignment(SSA)** to do **Non-Lexical Constraint-Based Lifetime Checking**. This system is more flexible than Rust's while remaining safe. If the lifetime relationships are intuitive, the compiler can often infer them without requiring explicit annotations.

---

# 📦 Quick Start

## 🍭 Install redy

There are two ways to install redy:
  1. Install a pre-built binary (recommended).
  2. Build from source.

Detailed instructions are available in the [installation guide](docs/INSTALL.md).

## ✏ Coding First Program

Here is a basic example of **redy** code.

```redy
// inline comments use double slashes.
/* block comments are enclosed in slashes and asterisks. */

// function declaration
// return types are explicitly annotated for readability.
fn main() -> i32 {
  // prints a line
  println#("hello world!")       // no semicolons needed.

  // variable declaration with an explicit type annotation.
  my_favorite_number: i32 = 42

  // type can be inferred using `:=`.
  my_name := "redy"

  // `println#` supports fmtlib like formatting
  println#("My name is {} and my favorite number is {}.", my_name, my_favorite_number)
  // it will be formatted to like `My name is redy and my favorite number is 42.`

  for n: 0..<100 {
    if n % 15 == 0 {
      println#("Fizz Buzz")
    } else if n % 3 == 0 {
      println#("Fizz")
    } else if n % 5 == 0 {
      println#("Buzz")
    } else {
      println#("number: {}", n)
    }
  }

  mut i := 3
  // `loop` is a syntax sugar for `while(true)`.
  loop {
    if ++i > 50 {
      break
    } 
  }

  // `ret` stands for `return`, which returns a value and exits the function
  ret 0 
}
```

There is a more detailed [**document**](https://pugur523.github.io/redy_doc/), so please take a look if you are interested.

---

# 🚀 Roadmap to First Stable Release

<details close>
  <summary>
    <h3>
      Phase 1: Create Minimum Viable Product in C++
    </h3>
  </summary>

  - [x] **File Managing**
    - [x] UTF-8 file reading / validating
      - [x] Strict unicode sequence validation using latest UCD data
    - [x] Multiple files managing system
    - [x] UTF-8 file stream

  - [x] **Diagnostic**
    - [x] Code
    - [x] Severity
    - [x] Entry
      - [x] Header
      - [x] Label
        - [x] Body
        - [x] Annotation
    - [x] Diagnostic engine
      - [x] Formatters
        - [x] Header Formatter
        - [x] Label Formatter
          - [x] Render source line
        - [x] Annotation Formatter

  - [x] **Internationalization**
    - [x] i18n Code generator (from toml language files)
      - [x] Duplication saving for memory efficiency
    - [x] Translator
      - [x] Formatting support

  - [x] **base**
    - [x] Arena (useful for design data oriented structs)
    - [x] Token definition
    - [x] Keyword definition
    - [x] Operator definition
    - [x] Token stream

  - [x] **Lexer**
    - [x] Identifier
      - [x] Use UAX #31 - unicode identifiers rule
    - [x] Keyword
    - [x] Literal
      - [x] Numeric
      - [x] Character
      - [x] String
    - [x] Operator
    - [x] Delimiter

  - [ ] **AST**
    - [x] Context (data oriented structure)
    - [x] Nodes
      - [x] Expression
        - [x] Without block
          - [x] Literal
          - [x] Path
          - [x] Unary operator
          - [x] Binary operator
          - [x] Grouped
          - [x] Array
          - [x] Tuple
          - [x] Index
          - [x] Construct
          - [x] Function call
          - [x] Field access
          - [x] Closure
          - [x] Await
          - [x] Continue
          - [x] Break
          - [x] Range
          - [x] Return
        - [x] With block
          - [x] Block
          - [x] Unsafe
          - [x] Fast
          - [x] If
          - [x] Loop
          - [x] While
          - [x] For
          - [x] Match
      - [x] Statements
        - [x] Assign
        - [x] Const Assign
        - [x] Expression
        - [x] Module
        - [x] Attribute
        - [x] Function
        - [x] Struct
        - [x] Enumeration
        - [x] Union

  - [ ] **Parser**
    - [ ] Expression
      - [x] Without block
        - [x] Literal
        - [x] Path
        - [x] Unary operator
        - [x] Binary operator
        - [x] Grouped
        - [x] Array
        - [x] Tuple
        - [x] Index
        - [x] Construct
        - [x] Function call
        - [x] Field access
        - [x] Closure
        - [x] Await
        - [x] Continue
        - [x] Break
        - [x] Range
        - [x] Return
      - [x] With block
        - [x] Block
        - [x] Unsafe
        - [x] Fast
        - [x] If
        - [x] Loop
        - [x] While
        - [x] For
        - [x] Match
    - [ ] Statements
      - [x] Assign
      - [x] Expression
      - [x] Attribute
      - [x] Function Declaration
      - [x] Struct Declaration
      - [ ] Enumeration Declaration
      - [ ] Trait Declaration
      - [ ] Implementation Declaration
      - [ ] Union Declaration
      - [x] Module Declaration
      - [x] Redirect Declaration

  - [ ] **AST-Analyzer**
    - [ ] Symbol resolution
    - [ ] Type resolution
    - [ ] Desugar

  - [ ] **HIR**
    - [x] Context (data oriented structure)

  - [ ] **HIR-Analyzer**
    - [ ] Optimize HIR

  - [ ] **MIR**
    - [x] Context (data oriented structure)

  - [ ] **MIR-Analyzer**
    - [ ] Borrow Checker
    - [ ] Lifetime Checker
    - [ ] Optimize MIR

  - [ ] **Codegen**
    - [ ] Convert MIR to LLVM-IR

  - [ ] **Create first version of the standard libaray in redy**

  - [ ] **Release v0.1.0 (alpha, written in C++)**

</details>

<details close>
  <summary>
    <h3>
      Phase 2: Bootstrapping
    </h3>
  </summary>

- [ ] **Rewrite all over the compiler in redy**
- [ ] **Release v0.2.0 (still alpha, written in redy)**

</details>

---

# 🪪 License

**redy** is licensed under the [Apache 2.0 License](LICENSE).

# ❤️ Credits

  - **[femtolog](http://github.com/pugur523/femtolog)**: An extremely fast logging library.
  - **[zlib](https://github.com/madler/zlib)**: Used for compressing logs and build caches.
  - **[GoogleTest (gtest)](https://github.com/google/googletest)**: The primary unit testing framework.
  - **[Google Benchmark](https://github.com/google/benchmark)**: Used for micro-benchmarking.
  - **[toml11](https://github.com/ToruNiina/toml11)**: Used for parsing and dumping TOML configuration files.
