make clean
nasm -o boot.bin boot.asm
nasm -o loader.bin loader.asm
nasm -o kernel.o ./src/kernel.asm -f elf

make
#ld -s -m elf_i386 -o kernel.bin kernel.o -Ttext 0x30400
