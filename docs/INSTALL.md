# Installation

This guide will walk you through the process of installing the **redy**.

---

## 1. Install Pre-built Binary (Recommended)

TBW...

### Manual Installer Archive

- [Linux](https://github.com/pugur523/redy/actions/runs/16851866442/artifacts/3726480006)
- [Windows](https://github.com/pugur523/redy/actions/runs/16851866442/artifacts/3726486384)
- [macOS](https://github.com/pugur523/redy/actions/runs/16851866442/artifacts/3726472420)

---

## 2. Building from Source

To build **redy** from its source code, follow these steps.

### Step 1: Clone the Repository

Clone the repository and its submodules, then navigate into the directory.

```bash
git clone --depth 1 --recursive [https://github.com/pugur523/redy.git](https://github.com/pugur523/redy.git)
cd redy
```

### Step 2: Set up Dependencies

**redy** requires both Python and system-level dependencies.

#### Python Dependencies

Install the required Python packages and set up a virtual environment.

<details>
<summary>
Installation Details
</summary>

1. Install Python(>= 3.13) from the <a href="https://www.python.org/downloads/">official website</a>.<br/>

2. Install uv:<br/>
```bash
pip install uv.
```

3. Create and activate a virtual environment:<br/>
```bash
uv venv
source .venv/bin/activate
```

4. Install the build dependencies:  
```bash
uv sync
```

</details>

#### System Dependencies

The required system dependencies vary by operating system.

<details>
<summary>
Installation Details
</summary>

### Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y wget curl ninja-build nasm nsis wine python3

# Install CMake
CMAKE_VERSION="4.1.1"
cmake_url="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh"
wget -nv "${cmake_url}" -O cmake-installer.sh
chmod +x cmake-installer.sh
sudo ./cmake-installer.sh --skip-license --prefix=/usr/local
rm cmake-installer.sh

# Install LLVM
LLVM_VERSION="21"
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

1. Install [**Chocolatey**](https://chocolatey.org/install#individual).
2. Run the following command in an administrator PowerShell terminal:
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

**Verification**

Ensure all required tools are correctly installed and accessible from the command line by running the following commands.

```bash
nasm --version
# Expected output: NASM version 2.16.03 compiled on May 13 2025

cmake --version
# Expected output: cmake version 4.1.1-dirty
# CMake suite maintained and supported by Kitware ([kitware.com/cmake](https://kitware.com/cmake)).

ninja --version
# Expected output: 1.12.1

clang --version
# Expected output: clang version 20.1.8
# Target: x86_64-pc-linux-gnu
# Thread model: posix
# InstalledDir: /usr/bin

# For Linux only
echo $LLVM_MINGW_DIR
# Expected output: /opt/llvm-mingw-20250826-ucrt-ubuntu-22.04-x86_64
```

</details>

### Step 3: Build and Install

Execute the build script. This process may take 5 to 10 minutes, it's time for tea :)

```bash
./cc build --release --install
```

Upon successful completion, the compiled artifacts will be located in the `./out/install/{os}/{arch}/release/bin/` directory.

If you encounter any compilation errors, please double-check that all required tools and dependencies are installed and properly configured.
