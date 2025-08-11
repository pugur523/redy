<p align="center">
  <img src="src/build/resources/assets/logo_1080.png" width="192" alt="redy">
</p>
<h1 align="center">redy</h1>

<p align="center">
  パフォーマンスとメモリ安全性を両立する新しいプログラミング言語
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
    <img src="https://img.shields.io/badge/LLVM-20-green?logo=llvm" alt="LLVM">
  </a>
</p>
<p align="left">
  <a href="README.md">
    <img src="https://img.shields.io/badge/English_description_here-blue" alt="English README">
  </a>
</p>

> [!NOTE]
> このプロジェクトは鋭意開発中であり、現状では**Hello World**のコードすらコンパイルできません。

---

# ☄ 概要

**redy** は、高いパフォーマンスと堅牢なメモリ安全性の両立を目指す新しいプログラミング言語です。

豊富な計算資源がある時代に忘れられがちな、計算量の削減、メモリ効率の向上、コンパイル後のバイナリサイズの最小化など、**真のパフォーマンス**を重視しています。

---

# 📖 目次

- [☄ 概要](#-概要)
- [📖 目次](#-目次)
- [🔥 特長](#-特長)
    - [🛡️ 完全なメモリ安全性](#️-完全なメモリ安全性)
    - [🚫 ゼロコストのアクセス制御](#-ゼロコストのアクセス制御)
    - [☄ 超高速](#-超高速)
    - [🍃 ライフタイムアノテーションは不要](#-ライフタイムアノテーションは不要)
- [📦 クイックスタート](#-クイックスタート)
  - [🍭 redyのインストール](#-redyのインストール)
  - [✏ redyではじめてのプログラム](#-redyではじめてのプログラム)
- [🚀 最初の安定版リリースまでのロードマップ](#-最初の安定版リリースまでのロードマップ)
- [🪪 ライセンス](#-ライセンス)
- [❤️ クレジット](#️-クレジット)

---

# 🔥 特長

> [!NOTE]
> ほとんどの機能はまだ実装されていませんが、ロジックの設計自体はすべて完了しています。

### 🛡️ 完全なメモリ安全性

**redy**は、Rustにインスパイアされた**所有権システム**を採用しています。すべてのヒープメモリはスコープを抜けると自動的に解放され、メモリリークを完全に防ぎます。

### 🚫 ゼロコストのアクセス制御

潜在的な不正なメモリアクセスはすべて**コンパイル時**に検出されるため、ランタイムのオーバーヘッドはありません。セグメンテーション違反とはもうお別れです！

### ☄ 超高速

境界チェックなどのランタイムコストは、さまざまなコンパイル時解析手法によって最小限に抑えられ、**最高のパフォーマンス**を実現します。

### 🍃 ライフタイムアノテーションは不要

コンパイラは、Rustよりも柔軟でありながら安全性を保つ **非語彙的制約ベースライフタイムチェッカー**(**NLCBL**)を使用します。変数のライフタイムの関係が直感的な場合、コンパイラはほとんどの場合、明示的なアノテーションを必要とせずに推論できます。

---

# 📦 クイックスタート

## 🍭 redyのインストール

**redy**をインストールする方法は2つあります。
  1. ビルド済みのバイナリをインストールする（推奨）。
  2. ソースコードからビルドする。

詳細な手順については、[インストールガイド](docs/INSTALL.ja.md)を参照してください。

## ✏ redyではじめてのプログラム

以下は、**redy**の基本的なコード例です。

```redy
// インラインコメントはダブルスラッシュのあとに記述します。
/* ブロックコメントはスラッシュとアスタリスクで囲みます。 */

// 関数を定義
// 戻り値の型は、コードの可読性のために明示的に注釈を付けなければいけません
fn main() -> i32 {
  // 文字列を出力して改行する
  println#("hello world!") // セミコロンは必要なし

  // 型を明示的に指定した変数宣言
  my_favourite_number :i32 = 42

  // 型は `:=` を使用してコンパイラに推論させることもできます
  my_name := "redy"

  // `println#`はfmtlibのようなフォーマットつきprintをサポートしています
  println#("My name is {} and my favourite number is {}.", my_name, my_favourite_number)
  // これは`My name is redy and my favourite number is 42.`のようにフォーマットされます

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
  // `loop`は`while(true)`の糖衣構文です(どちらを使っても同じ意味になる)
  loop {
    if ++i > 50 {
      break
    }
  }

  // `ret`は`return`の略で、値を返して関数を終了するために使用されます
  ret 0
}
```

もっと詳しい[**ドキュメント**](https://pugur523.github.io/redy_doc/)も用意しています。日本語にも対応しているので、興味があれば見てみてください！

---

# 🚀 最初の安定版リリースまでのロードマップ

<details close>
<summary>
  <h3>
    フェーズ1: C++で最低限動作するバージョン(Minimum Viable Product/MVPって呼ぶらしい)を作成
  </h3>
</summary>

  - [x] **ファイル管理**
      - [x] UTF-8ファイルの読み込みと検証
          - [x] 最新のUCDデータを使用した厳密なユニコードシーケンスの検証
      - [x] 複数ファイルの管理システム
      - [x] UTF-8ファイルカーソル（peek、nextなどを提供）

  - [x] **診断機能**
      - [x] コード
      - [x] 重大度
      - [x] エントリ
          - [x] ヘッダー
          - [x] ラベル
              - [x] ボディ
              - [x] アノテーション
      - [x] 診断エンジン
          - [x] フォーマッタ
              - [x] ヘッダーフォーマッタ
              - [x] ラベルフォーマッタ
                  - [x] ソース行のレンダリング
              - [x] アノテーションフォーマッタ

  - [x] **国際化**
      - [x] i18nコード生成器（tomlの言語ファイルから）
          - [x] メモリ効率のための重複削減
      - [x] トランスレータ
          - [x] フォーマットサポート

  - [x] **基盤**
      - [x] アリーナ（データ指向な構造の設計に便利）
      - [x] トークン定義
      - [x] キーワード定義
      - [x] 演算子定義
      - [x] トークンストリーム（`peek`、`next`などを提供）

  - [x] **字句解析器 (Lexer)**
      - [x] 識別子
          - [x] UAX #31 - ユニコード識別子のルールを使用
      - [x] キーワード
      - [x] リテラル
          - [x] 数値
          - [x] 文字
          - [x] 文字列
      - [x] 演算子
      - [x] 区切り文字

  - [ ] **抽象構文木 (AST)**
      - [x] コンテキスト（`Base::Arena`を使用するデータ指向な構造）
      - [ ] ノード
          - [ ] 式
              - [ ] ブロックなし
                  - [ ] リテラル
                  - [ ] パス
                  - [ ] 単項演算子
                  - [ ] 二項演算子
                  - [ ] グループ化
                  - [ ] 配列
                  - [ ] タプル
                  - [ ] インデックス
                  - [ ] コンストラクタ
                  - [ ] 関数呼び出し
                  - [ ] フィールドアクセス
                  - [ ] クロージャ
                  - [ ] Await
                  - [ ] Continue
                  - [ ] Break
                  - [ ] Range
                  - [ ] Return
              - [ ] ブロックあり
                  - [ ] ブロック
                  - [ ] Constブロック
                  - [ ] Unsafe
                  - [ ] Fast
                  - [ ] If
                  - [ ] Loop
                  - [ ] While
                  - [ ] For
                  - [ ] Match
          - [ ] 文
              - [ ] 代入
              - [ ] Const代入
              - [ ] 文として扱う式
              - [ ] モジュール
              - [ ] 属性
              - [ ] 関数
              - [ ] 構造体
              - [ ] 列挙型
              - [ ] 共用体

  - [ ] **構文解析器 (Parser)**
      - [ ] 式の解析
      - [ ] ステートメントの解析

  - [ ] **AST解析器 (AST-Analyzer)**
      - [ ] シンボル解決
      - [ ] 型解決
      - [ ] Desugar

  - [ ] **HIR (High-level Intermediate Representation)**
      - [ ] コンテキスト（`Base::Arena`を使用するデータ指向な構造）

  - [ ] **HIR解析器 (HIR-Analyzer)**
      - [ ] HIRの最適化

  - [ ] **MIR (Mid-level Intermediate Representation)**
      - [ ] コンテキスト（`Base::Arena`を使用するデータ指向な構造）

  - [ ] **MIR解析器 (MIR-Analyzer)**
      - [ ] 借用チェッカー (Borrow Checker)
      - [ ] ライフタイムチェッカー
      - [ ] MIRの最適化

  - [ ] **コード生成 (Codegen)**
      - [ ] MIRをLLVM-IRに変換

  - [ ] **redyで標準ライブラリの最初のバージョンを作成**

  - [ ] **v0.1.0（C++で書かれたα版）をリリース**

</details>

<details close>
<summary>
<h3>
フェーズ2: ブートストラップ
</h3>
</summary>

  - [ ] **コンパイラ全体をredyで書き直す**

  - [ ] **v0.2.0（redyで書かれたα版）をリリース**

</details>

---

# 🪪 ライセンス

**redy**は[Apache 2.0 License](LICENSE)のもとでライセンスされています

# ❤️ クレジット

  - **[femtolog](http://github.com/pugur523/femtolog)**: 超高速なロギングライブラリ。
  - **[zlib](https://github.com/madler/zlib)**: ログとビルドキャッシュの圧縮に使用。
  - **[GoogleTest (gtest)](https://github.com/google/googletest)**: プロジェクト全体の主要な単体テストフレームワーク。
  - **[Google Benchmark](https://github.com/google/benchmark)**: マイクロベンチマークフレームワークとして使用。
  - **[toml11](https://github.com/ToruNiina/toml11)**: TOML形式の設定ファイルの解析と出力に使用。
