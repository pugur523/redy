<p align="center">
  <img src="src/build/resources/assets/logo_1080.png" width="192" alt="redy">
</p>
<h1 align="center">redy</h1>

<p align="center">
  A new programming language that prioritizes performance and memory safety.
</p>

<p align="center">
  <a href="https://github.com/pugur523/redy/actions/workflows/ci.yml">
    <img src="https://github.com/pugur523/redy/actions/workflows/ci.yml/badge.svg" alt="CI">
  </a>
  <a href="https://github.com/pugur523/redy/issues">
    <img src="https://img.shields.io/github/issues/pugur523/redy.svg" alt="GitHub Issues">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-Apache%20License%20Version%202.0-yellow" alt="License">
  </a>
  <a href="https://llvm.org/docs/index.html">
    <img src="https://img.shields.io/badge/LLVM-20-green?logo=llvm" alt="LLVM Version">
  </a>
</p>
<p align="left">
  <a href="README.ja.md">
    <img src="https://img.shields.io/badge/日本語の説明はこちら-blue" alt="Japanese README">
  </a>
</p>

> [!NOTE]
> This project is a work in progress and cannot yet compile a **Hello World** program.

---

# ☄ Overview

**redy** is a new programming language to achieve both high performance and robust memory safety.

It emphasizes performance in all aspects, from reducing computation and memory usage to minimizing the size of compiled binaries, which is often overlooked in an era of abundant computing resources.

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

The compiler uses a **Non-Lexical Constraint-Based Lifetime Checker**(**NLCBL**). This system is more flexible than Rust's while remaining safe. If the lifetime relationships are intuitive, the compiler can often infer them without requiring explicit annotations.

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
  my_favourite_number :i32 = 42

  // type can be inferred using `:=`.
  my_name := "redy"

  // `println#` supports fmtlib like formatting
  println#("My name is {} and my favourite number is {}.", my_name, my_favourite_number)
  // it will be formatted to like `My name is redy and my favourite number is 42.`

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
  - [x] UTF-8 file cursor (provides peek, next, etc.)

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

- [x] **Base**
  - [x] Arena (useful for design data oriented structs)
  - [x] Token definition
  - [x] Keyword definition
  - [x] Operator definition
  - [x] Token stream (provides `peek`, `next`, etc.)

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
  - [x] Context (data oriented structure using `Base::Arena`)
  - [ ] Nodes
    - [ ] Expression
      - [ ] Without block
        - [ ] Literal
        - [ ] Path
        - [ ] Unary operator
        - [ ] Binary operator
        - [ ] Grouped
        - [ ] Array
        - [ ] Tuple
        - [ ] Index
        - [ ] Construct
        - [ ] Function call
        - [ ] Field access
        - [ ] Closure
        - [ ] Await
        - [ ] Continue
        - [ ] Break
        - [ ] Range
        - [ ] Return
      - [ ] With block
        - [ ] Block
        - [ ] Const block
        - [ ] Unsafe
        - [ ] Fast
        - [ ] If
        - [ ] Loop
        - [ ] While
        - [ ] For
        - [ ] Match
    - [ ] Statements
      - [ ] Assign
      - [ ] Const Assign
      - [ ] Expression
      - [ ] Module
      - [ ] Attribute
      - [ ] Function
      - [ ] Struct
      - [ ] Enumeration
      - [ ] Union

- [ ] **Parser**
  - [ ] Parse expressions
  - [ ] Parse statements

- [ ] **AST-Analyzer**
  - [ ] Symbol resolution
  - [ ] Type resolution
  - [ ] Desugar

- [ ] **HIR**
  - [ ] Context (data oriented structure using `Base::Arena`)

- [ ] **HIR-Analyzer**
  - [ ] Optimize HIR

- [ ] **MIR**
  - [ ] Context (data oriented structure using `Base::Arena`)

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
