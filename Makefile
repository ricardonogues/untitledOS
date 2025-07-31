CFLAGS = -m32 -g\
		 -ffreestanding\
		 -fno-exceptions\
		 -fno-asynchronous-unwind-tables\
		 -fno-stack-protector\
		 -fno-pic\
		 -fno-omit-frame-pointer\
		 -fno-strict-aliasing\
		 -fno-common\
		 -fno-builtin\
		 -nostdlib\
		 -Wall\
		 -Wextra
ASFLAGS = -f bin
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-ld

kernel.elf: kernel.o
	$(LD) -m elf_i386 -T kernel-elf.ld -o $@ $<

kernel.bin: kernel.o
	$(LD) -m elf_i386 -T kernel-bin.ld --oformat binary -o $@ $<

boot.bin: boot.asm
	$(AS) $(ASFLAGS) $< -o $@

kernel.o: kernel.c
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: all clean

all: kernel.bin kernel.elf boot.bin
clean:
	rm -f *.o *.bin kernel.elf kernel.bin boot.bin

os-image.bin: boot.bin kernel.bin kernel.elf
	cat boot.bin kernel.bin > os-image.bin
	dd if=/dev/zero bs=1K count=1440 >> os-image.bin

run: os-image.bin
	qemu-system-i386 -drive file=os-image.bin,format=raw -serial stdio -smp 1 -m 512M

debug: os-image.bin
	qemu-system-i386 -drive file=os-image.bin,format=raw -serial stdio -smp 1 -m 512M -s -S