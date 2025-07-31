# Compiler and linker settings
CFLAGS = -m32 -g -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables \
		 -fno-stack-protector -fno-pic -fno-omit-frame-pointer -fno-strict-aliasing \
		 -fno-common -fno-builtin -nostdlib -Wall -Wextra -Ikernel/include -fstack-protector
ASFLAGS = -f elf32
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-ld

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
LINKER_DIR = linker
BUILD_DIR = build
TESTS_DIR = ${KERNEL_DIR}/tests
LIB_DIR = $(KERNEL_DIR)/libc
ARCH_DIR = $(KERNEL_DIR)/arch/x86

# Output files
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
OS_IMAGE = $(BUILD_DIR)/os-image.bin

# Source files
KERNEL_C_SOURCES = $(shell find $(KERNEL_DIR) -name "*.c")
KERNEL_ASM_SOURCES = $(shell find $(KERNEL_DIR) -name "*.asm")
KERNEL_C_OBJECTS = $(KERNEL_C_SOURCES:%.c=$(BUILD_DIR)/%.o)
KERNEL_ASM_OBJECTS = $(KERNEL_ASM_SOURCES:%.asm=$(BUILD_DIR)/%.o)
KERNEL_OBJECTS = $(KERNEL_C_OBJECTS) $(KERNEL_ASM_OBJECTS)

.PHONY: all clean run debug directories

all: clean directories $(OS_IMAGE) run

directories:
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/drivers/vga
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)/drivers/tty
	@mkdir -p $(BUILD_DIR)/$(ARCH_DIR)
	@mkdir -p $(BUILD_DIR)/$(LIB_DIR)
	@mkdir -p $(BUILD_DIR)/$(TESTS_DIR)

$(BUILD_DIR)/%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(KERNEL_ELF): $(KERNEL_OBJECTS)
	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-elf.ld -o $@ $<

$(KERNEL_BIN): $(KERNEL_OBJECTS)
	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-bin.ld --oformat binary -o $@ $^

$(BOOT_BIN): $(BOOT_DIR)/boot.asm
	$(AS) -f bin $< -o $@

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(OS_IMAGE)
	dd if=/dev/zero bs=1K count=1440 >> $(OS_IMAGE)

run: $(OS_IMAGE)
	qemu-system-i386 -drive file=$(OS_IMAGE),format=raw -serial stdio -smp 1 -m 512M

debug: $(OS_IMAGE)
	qemu-system-i386 -drive file=$(OS_IMAGE),format=raw -serial stdio -smp 1 -m 512M -s -S

clean:
	rm -rf $(BUILD_DIR)