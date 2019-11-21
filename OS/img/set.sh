dd if=../boot.bin of=boot.img bs=512 count=1
dd if=../loader.bin of=loader.img bs=512 count=1
dd if=../kernel.bin of=kernel.img bs=512 count=100 conv=notrunc

dd if=loader.img of=boot.img skip=0 seek=1 bs=512 count=1
dd if=kernel.img of=boot.img skip=0 seek=2 bs=512 count=100 conv=notrunc

dd if=floppy.img of=boot.img skip=14 seek=14 bs=512 count=2778
