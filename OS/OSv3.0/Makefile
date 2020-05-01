CFLAGS = -mcmodel=large -Iinclude -m64 -fno-builtin

vpath %.c ./src
cfiles := $(wildcard src/*.c)
objects := $(cfiles:.c=.o)

all: system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary system kernel.bin

system: $(objects) src/head.o src/entry.o
	ld -b elf64-x86-64 -o system src/head.o src/entry.o $(objects) -T ./Kernel.lds

$(objects):%.o:%.c
	gcc -c $(CFLAGS) $< -o $@

head.o:	./src/head.S
	gcc -E  ./src/head.S > head.s
	as --64 -o head.o head.s

entry.o: src/entry.S
	gcc -E  ./src/entry.S > entry.s
	as --64 -o entry.o entry.S

clean:
	-rm ./src/*.o *.o
