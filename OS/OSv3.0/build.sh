nasm -o boot.bin ./asm/boot.asm
nasm -o loader.bin ./asm/loader.asm

dd if=boot.bin of=./img/boot.img bs=512 count=1 conv=notrunc
dd if=/dev/zero of=./img/kernel.img bs=512 count=2880 conv=notrunc
#dd if=loader.bin of=./img/boot.img bs=512 seek=1 count=1 conv=notrunc
dd if=./img/kernel.img of=./img/boot.img skip=1 seek=1 bs=512 count=2879

make

sudo mount ./img/boot.img /mnt/ -o loop -t vfat
sudo cp loader.bin /mnt/
# sudo cp ./img/loader.img /mnt/
sudo cp kernel.bin /mnt/
sync
sudo umount /mnt/

bochs -f ./img/bochsrc