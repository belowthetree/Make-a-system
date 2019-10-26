dd if=../bootloader.o of=bootloader.img bs=512 count=1
dd if=../kernel.bin of=kernel.img  bs=512 count=1
dd if=kernel.img of=bootloader.img skip=0 seek=1 bs=512 count=1
dd if=floppy.img of=bootloader.img skip=2 seek=2 bs=512 count=2878


dd if=../bootloader.o of=bootloader.img bs=512 count=1
dd if=../start.o of=start.img bs=512 count=1
dd if=start.img of=bootloader.img skip=0 seek=1 bs=512 count=1
dd if=../kernel.bin of=kernel.img bs=512 count=1998
dd if=kernel.img of=bootloader.img skip=0 seek=2 bs=512 count=1998
dd if=floppy.img of=bootloader.img skip=2000 seek=2000 bs=512 count=880
