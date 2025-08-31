# インストール

**redy**のインストール手順を説明します

---

## 1. ビルド済みバイナリのインストール (推奨)

TBW...

### 手動インストーラーアーカイブ

- [Linux](https://github.com/pugur523/redy/actions/runs/16851866442/artifacts/3726480006)
- [Windows](https://github.com/pugur523/redy/actions/runs/16851866442/artifacts/3726486384)
- [macOS](https://github.com/pugur523/redy/actions/runs/16851866442/artifacts/3726472420)

---

## 2. ソースコードからビルド

ソースコードからビルドするには、以下の手順に従ってください

### ステップ1: リポジトリをクローン

リポジトリとサブモジュールをクローンし、ディレクトリに移動します

```bash
git clone --depth 1 --recursive [https://github.com/pugur523/redy.git](https://github.com/pugur523/redy.git)
cd redy
```

### ステップ2: 依存関係のセットアップ

**redy**にはPythonなどのいくつかの依存関係が必要です

#### Pythonの依存関係

必要なPythonパッケージをインストールし、仮想環境をセットアップします

<details>
<summary>
インストール詳細
</summary>

  * [**Python >= 3.13**](https://www.python.org/downloads/)をインストールします
  * `uv`パッケージマネージャーをインストールします: `pip install uv`
  * 仮想環境を作成し、アクティベートします:
    ```bash
    uv venv
    source .venv/bin/activate
    ```
  * ビルドに必要なPythonパッケージをインストールします:
    ```bash
    uv sync
    ```

</details>

#### システムの依存関係

必要なシステムの依存関係はOSごとに異なります

<details>
<summary>
インストール詳細
</summary>

### Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y wget curl ninja-build nasm nsis wine python3

# CMakeのインストール
CMAKE_VERSION="4.0.3"
cmake_url="[https://github.com/Kitware/CMake/releases/download/v$](https://github.com/Kitware/CMake/releases/download/v$){CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh"
wget -nv "${cmake_url}" -O cmake-installer.sh
chmod +x cmake-installer.sh
sudo ./cmake-installer.sh --skip-license --prefix=/usr/local
rm cmake-installer.sh

# LLVMのインストール
LLVM_VERSION="20"
wget -qOllvm.sh [https://apt.llvm.org/llvm.sh](https://apt.llvm.org/llvm.sh) && chmod +x llvm.sh && sudo ./llvm.sh $LLVM_VERSION all && rm ./llvm.sh

source ./src/build/scripts/install_llvm_mingw.sh
echo "export LLVM_MINGW_DIR=${LLVM_MINGW_DIR}" >> ~/.bashrc
echo "export LLVM_MINGW_DIR=${LLVM_MINGW_DIR}" >> ~/.zshrc
```

### Arch Linux

```bash
pacman -S --noconfirm \
        base-devel \
        git \
        wget \
        curl \
        gnupg \
        python \
        python-pip \
        ninja \
        nasm \
        yasm \
        pkgconf \
        openssl \
        llvm \
        clang \
        clang-tools-extra \
        lld \
        llvm-libs \
        cmake
yay -S --noconfirm libc++-with-libunwind
```

### Windows

1.  [**Chocolatey**](https://chocolatey.org/install#individual)をインストールします
2.  管理者権限のPowerShellターミナルで、次のコマンドを実行します
    ```powershell
    choco install -y nsis ninja nasm cmake llvm
    ```

### macOS

```bash
brew update
brew install ninja nasm cmake llvm@20 lld@20
echo 'export PATH="$(brew --prefix llvm@20)/bin:$PATH"' >> ~/.bash_profile
echo 'export PATH="$(brew --prefix lld@20)/bin:$PATH"' >> ~/.bash_profile
```

**確認**

すべての必要なツールが正しくインストールされ、コマンドラインからアクセスできることを確認するために、以下のコマンドを実行します

```bash
nasm --version
# 期待される出力: NASM version 2.16.03 compiled on May 13 2025

cmake --version
# 期待される出力: cmake version 4.0.3-dirty
# CMake suite maintained and supported by Kitware ([kitware.com/cmake](https://kitware.com/cmake)).

ninja --version
# 期待される出力: 1.12.1

clang --version
# 期待される出力: clang version 20.1.8
# Target: x86_64-pc-linux-gnu
# Thread model: posix
# InstalledDir: /usr/bin

# Linuxのみ
echo $LLVM_MINGW_DIR
# 期待される出力: /opt/llvm-mingw-20250826-ucrt-ubuntu-22.04-x86_64
```

</details>

### ステップ3: ビルドとインストール

ビルドスクリプトを実行します  
このプロセスには5〜10分かかる場合があります  
座して待て

```bash
./cc build --release --install
```

正常に完了すると、コンパイルされた成果物は`./out/install/{os}/{arch}/release/bin/`ディレクトリに配置されます

コンパイルエラーが発生した場合は、必要なツールと依存関係がすべて正しくインストールされ、適切に設定されていることを再度確認してください
