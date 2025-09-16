<div align="center">
  <img src="src/build/resources/assets/logo_dark_r.svg#gh-dark-mode-only" width="192" alt="redy">
  <img src="src/build/resources/assets/logo_light_r.svg#gh-light-mode-only" width="192" alt="redy">

  <h1>redy</h1>

  安全性を保ちながら最大限のパフォーマンス発揮する新しいプログラミング言語

  [![CI](https://github.com/pugur523/redy/actions/workflows/ci.yml/badge.svg)](https://github.com/pugur523/redy/actions/workflows/ci.yml)
  [![Issues](https://img.shields.io/github/issues/pugur523/redy.svg)](https://github.com/pugur523/redy/issues)
  [![License](https://img.shields.io/badge/License-Apache%20License%20Version%202.0-red)](LICENSE)<br/>
  [![C](https://img.shields.io/badge/C-blue?logo=c)](https://www.c-language.org/)
  [![C++](https://img.shields.io/badge/C++-blue?logo=cplusplus)](https://isocpp.org/)
  [![LLVM](https://img.shields.io/badge/LLVM-20-emerald?logo=llvm)](https://llvm.org/docs/index.html)
  [![ドキュメント](https://img.shields.io/badge/ドキュメント-purple)](https://pugur523.github.io/redy_doc/ja/)
  [![English](https://img.shields.io/badge/English-blue)](README.md)
</div>

> [!NOTE]
> このプロジェクトは鋭意開発中であり、現状では**Hello World**のコードすらコンパイルできません
> また、現在受験勉強に集中するため一時的に休止してます。3月になっても更新がなかったら多分そういうことなのでもう一年遅れます、そうなるつもりは毛頭ありませんけどね

---

# ☄ 概要

**redy** は、高いパフォーマンスと堅牢な安全性の両立を目指す新しいプログラミング言語です

豊富な計算資源がある現代では軽視されがちな計算量の削減、メモリ効率の向上、キャッシュヒット率の向上、コンパイル後バイナリサイズの最小化など、**真のパフォーマンス最大化**を重視していて、安全性と引き換えに実行速度の面で妥協することを許さないという思想です

---

# 📖 目次

- [☄ 概要](#-概要)
- [📖 目次](#-目次)
- [🔥 特長](#-特長)
    - [🛡️ メモリ安全性](#️-メモリ安全性)
    - [🚫 ゼロコストのアクセス制御](#-ゼロコストのアクセス制御)
    - [☄ 実行速度](#-実行速度)
    - [🍃 ライフタイムの高度な自動推論](#-ライフタイムの高度な自動推論)
- [📦 クイックスタート](#-クイックスタート)
  - [🍭 redyのインストール](#-redyのインストール)
  - [✏ redyでプログラムを書く](#-redyでプログラムを書く)
- [🚀 最初の安定版リリースまでのロードマップ](#-最初の安定版リリースまでのロードマップ)
- [🪪 ライセンス](#-ライセンス)
- [❤️ クレジット](#️-クレジット)

---

# 🔥 特長

> [!NOTE]
> ほとんどの機能はまだ実装されていませんが、ロジックの設計自体はすべて完了しています

### 🛡️ メモリ安全性

**redy**は、Rustにインスパイアされた**所有権システム**を採用しています  
すべてのヒープメモリはスコープを抜けると自動的に解放され、メモリリークを完全に防ぎます

### 🚫 ゼロコストのアクセス制御

潜在的な不正メモリアクセスは、純粋関数であればすべて**コンパイル時**に検出されるため、ランタイムの境界チェックなどのオーバーヘッドを極力なしにしつつセグフォを防ぎます

### ☄ 実行速度

境界チェックなどのランタイムコストは、さまざまなコンパイル時解析手法によって最小限に抑えられ、**最高のパフォーマンス**を実現します

### 🍃 ライフタイムの高度な自動推論

コンパイラは、Rustよりも柔軟でありながら安全性を保つ、**制御フローグラフ(CFG)**と**静的単一代入(SSA)**による**非語彙依存制約ベースライフタイムチェック**を行います  
変数のライフタイムの関係が人間にとって直観的なとき、そのほとんどの場合でコンパイラも明示的なライフタイム注釈を必要とせず推論することができます

---

# 📦 クイックスタート

## 🍭 redyのインストール

**redy**をインストールする方法は以下の2つです
  1. ビルド済みバイナリをインストールする(推奨)
  2. ソースコードからビルドする

詳細な手順については、[インストールガイド](docs/INSTALL.ja.md)を用意しているのでこれを参照してください

## ✏ redyでプログラムを書く

以下は、**redy**の基本的なコード例です

```redy
// インラインコメントはダブルスラッシュのあとに記述します
/* ブロックコメントはスラッシュとアスタリスクで囲みます */

// 関数を定義
// 戻り値の型は、コードの可読性のために明示的に注釈を付けなければいけません
fn main() -> i32 {
  // 文字列を出力して改行する
  println#("hello world!") // セミコロンは必要なし

  // 型を明示的に指定した変数宣言
  my_favorite_number: i32 = 42

  // 型は `:=` を使用してコンパイラに推論させることもできます
  my_name := "redy"

  // `println#`はfmtlibのようなフォーマットつきprintをサポートしています
  println#("My name is {} and my favorite number is {}.", my_name, my_favorite_number)
  // これは`My name is redy and my favorite number is 42.`のようにフォーマットされます

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

  // `ret`は`return`の略であり、値を返して関数を終了するために使用されます
  ret 0
}
```

もっと詳しい[**ドキュメントサイト**](https://pugur523.github.io/redy_doc/ja/)も用意しています  
現在日本語と英語の2言語に対応しているので、興味があれば見てみてください

---

# 🚀 最初の安定版リリースまでのロードマップ

<details close>
<summary>
  <h3>
    フェーズ1: C++で最低限動作するバージョンの作成
  </h3>
</summary>

  - [x] **ファイル管理**
    - [x] UTF-8ファイルの読み込みと検証
      - [x] 最新のUCDデータを使用した厳密なユニコードシーケンスの検証
    - [x] 複数ファイルの管理システム
    - [x] UTF-8ファイルカーソル

  - [x] **Diagnostic - エラー診断**
    - [x] コード
    - [x] 重大度
    - [x] エントリ
      - [x] ヘッダー
      - [x] ラベル
        - [x] ボディ
        - [x] アノテーション
    - [x] エラー診断エンジン
      - [x] フォーマッタ
        - [x] ヘッダーフォーマッタ
        - [x] ラベルフォーマッタ
          - [x] ソース行のレンダリング
        - [x] アノテーションフォーマッタ

  - [x] **Internationalization - 多言語サポート**
    - [x] i18nコード生成器(tomlの言語ファイルから)
      - [x] メモリ効率のための重複削減
    - [x] トランスレータ
      - [x] フォーマットサポート

  - [x] **base - 全体で共有されるモジュール**
    - [x] アリーナ(データ指向な構造の設計に便利)
    - [x] 文字列アリーナ
    - [x] 文字列重複排除
    - [x] トークン定義
    - [x] キーワード定義
    - [x] 演算子定義
    - [x] トークンストリーム

  - [x] **Lexer - 字句解析**
    - [x] 識別子
      - [x] UAX #31 - ユニコード識別子のルールを使用
    - [x] キーワード
    - [x] リテラル
      - [x] 数値
      - [x] 文字
      - [x] 文字列
    - [x] 演算子
    - [x] 区切り文字

  - [x] **AST - 抽象構文木**
    - [x] コンテキスト(データ指向構造)
    - [x] ノード
      - [x] 式
        - [x] ブロックなし
          - [x] リテラル
          - [x] パス
          - [x] 単項演算子
          - [x] 二項演算子
          - [x] グループ化
          - [x] 配列
          - [x] タプル
          - [x] インデックス
          - [x] コンストラクタ
          - [x] 関数呼び出し
          - [x] フィールドアクセス
          - [x] クロージャ
          - [x] Await
          - [x] Continue
          - [x] Break
          - [x] Range
          - [x] Return
        - [x] ブロックあり
          - [x] ブロック
          - [x] Unsafe
          - [x] Fast
          - [x] If
          - [x] Loop
          - [x] While
          - [x] For
          - [x] Match
      - [x] 文
        - [x] 代入
        - [x] Const代入
        - [x] 文として扱う式
        - [x] モジュール
        - [x] 属性
        - [x] 関数
        - [x] 構造体
        - [x] 列挙型
        - [x] 共用体

  - [x] **Parser - 構文解析**
    - [x] 式
      - [x] ブロックなし
        - [x] リテラル
        - [x] パス
        - [x] 単項演算子
        - [x] 二項演算子
        - [x] グループ化
        - [x] 配列
        - [x] タプル
        - [x] インデックス
        - [x] コンストラクタ
        - [x] 関数呼び出し
        - [x] フィールドアクセス
        - [x] クロージャ
        - [x] Await
        - [x] Continue
        - [x] Break
        - [x] Range
        - [x] Return
      - [x] ブロックあり
        - [x] ブロック
        - [x] If
        - [x] Loop
        - [x] While
        - [x] For
        - [x] Match
    - [x] 文
      - [x] 代入
      - [x] 文として扱う式
      - [x] 属性
      - [x] 使用文
      - [x] 関数宣言
      - [x] 構造体宣言
      - [x] 列挙型宣言
      - [x] トレイト宣言
      - [x] 実装宣言
      - [x] 共用体宣言
      - [x] モジュール宣言
      - [x] リダイレクト宣言

  - [ ] **Resolver - AST解析**
    - [ ] シンボル解決
      - [ ] シンボルテーブル
    - [ ] 型解決
    - [ ] Desugar

  - [ ] **HIR - 抽象度高めの中間表現**
    - [ ] コンテキスト(データ指向構造)

  - [ ] **HIR-Analyzer - HIR解析**
    - [ ] HIRの最適化

  - [ ] **MIR - 抽象度少し低めの中間表現**
    - [ ] コンテキスト(データ指向構造)

  - [ ] **MIR-Analyzer - MIR解析**
    - [ ] 借用チェッカー (Borrow Checker)
    - [ ] ライフタイムチェッカー
    - [ ] MIRの最適化

  - [ ] **Codegen - LLVMに渡すコードの生成**
    - [ ] MIRをLLVM-IRに変換

  - [ ] **redyで標準ライブラリの最初のバージョンを作成**

  - [ ] **v0.1.0 (C++で書かれたα版)をリリース**

</details>

<details close>
<summary>
<h3>
フェーズ2: ブートストラップ化
</h3>
</summary>

  - [ ] **コンパイラ全体をredyで書き直す**

  - [ ] **v0.2.0(redyで書かれたα版)をリリース**

</details>

---

# 🪪 ライセンス

**redy**は[Apache 2.0 License](LICENSE)のもとでライセンスされています

# ❤️ クレジット

  - **[femtolog](http://github.com/pugur523/femtolog)**: 超高速なロギングライブラリ
  - **[zlib](https://github.com/madler/zlib)**: ログとビルドキャッシュの圧縮に使用
  - **[GoogleTest (gtest)](https://github.com/google/googletest)**: プロジェクト全体で使用しているユニットテストフレームワーク
  - **[Google Benchmark](https://github.com/google/benchmark)**: マイクロベンチマークフレームワークとして使用
  - **[toml11](https://github.com/ToruNiina/toml11)**: TOML形式の設定ファイルの解析と出力に使用
