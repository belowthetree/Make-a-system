nasm boot.asm -o boot.bin
nasm loader.asm -o loader.bin

dd if=boot.bin of=./img/boot.img bs=512 count=1 conv=notrunc
dd if=./img/kernel.img of=./img/boot.img skip=1 seek=1 bs=512 count=2879

sudo mount ./img/boot.img /media/ -t vfat -o loop
sudo cp loader.bin /media/
sudo cp kernel.bin /media/
sync
sudo umount /media/

bochs -f ./img/bochsrcq
