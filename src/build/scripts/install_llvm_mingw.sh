#!/bin/bash

set -e

scripts_dir=$(cd "$(dirname "$0")" && pwd)

tag="20250709"
main_name="llvm-mingw-${tag}-ucrt-ubuntu-22.04-x86_64"
archive_name="${main_name}.tar.xz"
url="https://github.com/mstorsjo/llvm-mingw/releases/download/${tag}/${archive_name}"
install_dir="/opt/${main_name}"

download_path="${scripts_dir}/${archive_name}"

# echo "Downloading from: $url"
wget -q "$url" -O "$download_path"

# echo "Extracting: $download_path"
tar xf "$download_path" -C "$scripts_dir" && rm $download_path

# echo "Installing to: $install_dir"
sudo rm -rf "$install_dir"
sudo mv -f "$scripts_dir/${main_name}" "$install_dir"

export LLVM_MINGW_DIR=${install_dir}
echo $LLVM_MINGW_DIR

# echo "Installation complete!"
# echo "To use llvm-mingw, restart your terminal or run: source ~/.bashrc"
#
# echo "Done."
