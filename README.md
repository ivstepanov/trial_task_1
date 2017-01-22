#  Задание 1
Собрать компилятором gcc и запустить под FreeRTOS для RISC-V (RV32G) бенчмарки на
платформе QEMU sifive_board (git@github. com: riscv/riscv-qemu. git).
Результат представить в виде репозитария на github. com с описанием пререквизитов
и последовательности сборки и запуска (с консольным логом запуска) в README. md.
Репозиторий должен также содержать все необходимые для сборки и запуска скрипты.
Бенчмарк должен запускаться как задача FreeRTOS, которая будет перезапускаться
через 5 секунд после завершения.
trial task

# Инструкция
Для сборки riscv-qemu потребуется:
sudo apt-get install git gcc libc6-dev pkg-config bridge-utils uml-utilities zlib1g-dev libglib2.0-dev autoconf automake libtool libsdl1.2-dev
Для сборки riscv-tools:
sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev

После того как установлены необходимые пакеты, получаем содержимое репозитария:
git clone https://github.com/ivstepanov/trial_task_1.git
Переходим в папку trial_task_1:
cd trial_task_1
Выполним сборку riscv-qemu, riscv-tools, FreeRTOS:
./build.sh
Консольный лог работы сборочного скрипта сохранен в файле build.log
Запуск:
./run.sh

