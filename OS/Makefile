CFLAGS = -m32 -ggdb -gstabs -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include

vpath %.c ./src
cfiles := $(wildcard ./src/*.c)
objects := $(cfiles:.c=.o)

kernel.bin: $(objects)
	ld -m elf_i386 kernel.o $(objects) -o kernel.bin -e _start -Ttext 0x30400

$(objects):%.o:%.c
	gcc -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-rm ./src/*.o *.o kernel.bin