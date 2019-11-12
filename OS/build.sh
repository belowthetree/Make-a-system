make clean
nasm -o boot.bin boot.asm
nasm -o loader.bin loader.asm
nasm -o kernel.o ./src/kernel.asm -f elf

make

dd if=boot.bin of=img/boot.img bs=512 count=1
dd if=loader.bin of=img/loader.img bs=512 count=1
dd if=kernel.bin of=img/kernel.img bs=512 count=10

dd if=img/loader.img of=img/boot.img skip=0 seek=1 bs=512 count=1
dd if=img/kernel.img of=img/boot.img skip=0 seek=2 bs=512 count=12

dd if=img/floppy.img of=img/boot.img skip=14 seek=14 bs=512 count=2866
