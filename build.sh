#! /bin/bash

export RISCV=$HOME/riscv
export PATH=$PATH:$RISCV/bin
git clone https://github.com/riscv/riscv-qemu.git
cd ./riscv-qemu
git reset --hard 168b87725a4295849a2870055f8f6459ec5b48f9
git submodule update --init --recursive
git apply ../patch/op_helper.patch
./configure --target-list=riscv32-softmmu
make
cd ..
git clone https://github.com/riscv/riscv-tools
cd riscv-tools
git submodule update --init --recursive
./build-rv32ima.sh
cd ..
cd ./FreeRTOS/Demo/riscv-spike
make

