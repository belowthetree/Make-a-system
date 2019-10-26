CFLAGS = -m32 -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I../include

vpath %.c ./src
cfiles := $(wildcard ./src/*.c)
objects := $(cfiles:.c=.o)

kernel.bin: $(objects)
	ld -s -m elf_i386 kernel.o $(objects) -o kernel.bin -e _start -Ttext 0x30400

$(objects):$(cfiles)
	gcc -c $(CFLAGS) $(cfiles) -o $(objects)

.PHONY:clean
clean:
	-rm ./src/*.o *.o kernel.bin