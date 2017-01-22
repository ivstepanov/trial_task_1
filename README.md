#  ������� 1
������� ������������ gcc � ��������� ��� FreeRTOS ��� RISC-V (RV32G) ��������� ��
��������� QEMU sifive_board (git@github. com: riscv/riscv-qemu. git).
��������� ����������� � ���� ����������� �� github. com � ��������� �������������
� ������������������ ������ � ������� (� ���������� ����� �������) � README. md.
����������� ������ ����� ��������� ��� ����������� ��� ������ � ������� �������.
�������� ������ ����������� ��� ������ FreeRTOS, ������� ����� ���������������
����� 5 ������ ����� ����������.
trial task

# ����������
��� ������ riscv-qemu �����������:
sudo apt-get install git gcc libc6-dev pkg-config bridge-utils uml-utilities zlib1g-dev libglib2.0-dev autoconf automake libtool libsdl1.2-dev
��� ������ riscv-tools:
sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev

����� ���� ��� ����������� ����������� ������, �������� ���������� �����������:
git clone https://github.com/ivstepanov/trial_task_1.git
��������� � ����� trial_task_1:
cd trial_task_1
�������� ������ riscv-qemu, riscv-tools, FreeRTOS:
./build.sh
���������� ��� ������ ���������� ������� �������� � ����� build.log
������:
./run.sh

