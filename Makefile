# Compiler and linker settings
CFLAGS = -m32 -g -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables \
		 -fno-pic -fno-omit-frame-pointer -fno-strict-aliasing \
		 -fno-common -fno-builtin -nostdlib -Wall -Wextra -Ikernel/include -fstack-protector
ASFLAGS = -f elf32
CC = x86_64-elf-gcc
AS = nasm
LD = x86_64-elf-ld

# Directories
SRC_DIR = src
BOOT_DIR = $(SRC_DIR)/boot
KERNEL_DIR = $(SRC_DIR)/kernel
LINKER_DIR = $(SRC_DIR)/linker
BUILD_DIR = build

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

.PHONY: all clean run debug

all: clean $(OS_IMAGE) run

# Automatic directory creation - creates directories as needed
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Bootloader (flat binary)
$(BOOT_BIN): $(BOOT_DIR)/boot.asm
	@mkdir -p $(dir $@)
	$(AS) -f bin $< -o $@

# Kernel ELF
$(KERNEL_ELF): $(KERNEL_OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-elf.ld -o $@ $^

# Kernel binary
$(KERNEL_BIN): $(KERNEL_OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-bin.ld --oformat binary -o $@ $^

# OS image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	@mkdir -p $(dir $@)
	cat $^ > $@
	dd if=/dev/zero bs=1K count=1440 >> $@

run: $(OS_IMAGE)
	qemu-system-i386 -drive file=$<,format=raw -serial stdio -smp 1 -m 512M

debug: $(OS_IMAGE)
	qemu-system-i386 -drive file=$<,format=raw -serial stdio -smp 1 -m 512M -s -S

clean:
	rm -rf $(BUILD_DIR)