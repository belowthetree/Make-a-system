CFLAGS = -mcmodel=large -fno-builtin -Iinclude

vpath %.c ./src
cfiles := $(wildcard kernel/*.c)
objects := $(cfiles:.c=.o)

all: system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary system kernel.bin

system: $(objects) kernel/head.o kernel/entry.o
	ld -b elf64-x86-64 -o system kernel/head.o kernel/entry.o $(objects) -T ./kernel/Kernel.lds

$(objects):%.o:%.c
	gcc -c $(CFLAGS) $< -o $@

head.o:	./kernel/head.S
	gcc -E  ./kernel/head.S > head.s
	as --64 -o head.o head.s

entry.o: kernel/entry.S
	gcc -E  ./kernel/entry.S > entry.s
	as --64 -o entry.o entry.s

clean:
	-rm ./kernel/*.o *.o
