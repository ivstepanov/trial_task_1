#! /bin/bash

./riscv-qemu/riscv32-softmmu/qemu-system-riscv32 -machine sifive -kernel ./FreeRTOS/Demo/riscv-spike/riscv-spike.elf -serial stdio