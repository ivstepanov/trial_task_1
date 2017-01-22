# Instruction for build and run
riscv-qemu prerequisites:
sudo apt-get install git gcc libc6-dev pkg-config bridge-utils uml-utilities zlib1g-dev libglib2.0-dev autoconf automake libtool libsdl1.2-dev
riscv-tools prerequisites:
sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev

####Step 1:
    $ git clone https://github.com/ivstepanov/trial_task_1.git
    $ cd trial_task_1
####Step 2: build riscv-qemu, riscv-tools, FreeRTOS:
    $ ./build.sh
Console output can be found in build.log
####Step 3: run qemu + FreeRTOS:
./run.sh

