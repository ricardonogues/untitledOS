# Compiler and linker settings
CFLAGS = -m32 -g -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables \
		 -fno-pic -fno-omit-frame-pointer -fno-strict-aliasing \
		 -fno-common -fno-builtin -nostdlib -Wall -Wextra -Ikernel/include -fstack-protector
ASFLAGS = -f elf32
CC = gcc
AS = nasm
LD = ld

# Directories
SRC_DIR = src
BOOT_DIR = $(SRC_DIR)/boot
KERNEL_DIR = $(SRC_DIR)/kernel
LINKER_DIR = $(SRC_DIR)/linker
BUILD_DIR = build
CFLAGS += -I$(KERNEL_DIR)/libc/include

# Output files
BOOT_BIN = $(BUILD_DIR)/boot.bin
SECOND_STAGE_BIN = $(BUILD_DIR)/second_stage.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
OS_IMAGE = $(BUILD_DIR)/os-image.bin

# Libc and other dependencies
# LIBC_DIR = $(SRC_DIR)/kernel/libc
# LIBC_C_SOURCES = $(shell find $(LIBC_DIR) -name "*.c")
# LIBC_C_OBJECTS = $(LIBC_C_SOURCES:%.c=$(BUILD_DIR)/%.o)
# LIBC_ARCHIVE = $(BUILD_DIR)/libc.a

# Source files
# KERNEL_C_SOURCES = $(shell find $(KERNEL_DIR) -name "*.c" -not -path "$(LIBC_DIR)/*" $(if $(with-tests),, -not -path "*/tests/*"))
KERNEL_C_SOURCES = $(shell find $(KERNEL_DIR) -name "*.c" $(if $(with-tests),, -not -path "*/tests/*"))
KERNEL_ASM_SOURCES = $(shell find $(KERNEL_DIR) -name "*.asm" $(if $(with-tests),, -not -path "*/tests/*"))
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

# Kernel static library
# $(LIBC_ARCHIVE): $(LIBC_C_OBJECTS)
# 	@mkdir -p $(dir $@)
# 	ar rcs $@ $^

# Kernel ELF
# $(KERNEL_ELF): $(KERNEL_OBJECTS) $(LIBC_ARCHIVE)
$(KERNEL_ELF): $(KERNEL_OBJECTS)
	@mkdir -p $(dir $@)
# 	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-elf.ld -o $@ --whole-archive $(KERNEL_OBJECTS) $(LIBC_ARCHIVE) --no-whole-archive
	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-elf.ld -o $@ $^

# Kernel binary
# $(KERNEL_BIN): $(KERNEL_OBJECTS) $(LIBC_ARCHIVE)
$(KERNEL_BIN): $(KERNEL_OBJECTS)
	@mkdir -p $(dir $@)
# 	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-bin.ld --oformat binary -o $@ --whole-archive $(KERNEL_OBJECTS) $(LIBC_ARCHIVE) --no-whole-archive
	$(LD) -m elf_i386 -T $(LINKER_DIR)/kernel-bin.ld --oformat binary -o $@ $^

# OS image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN) $(KERNEL_ELF)
	@mkdir -p $(dir $@)
	cat $^ > $@
	dd if=/dev/zero bs=1K count=1440 >> $@

run: $(OS_IMAGE)
	qemu-system-x86_64 -machine pc -drive file=$<,format=raw -serial stdio -smp 1 -m 512M

debug: $(OS_IMAGE)
	qemu -machine pc -drive file=$<,format=raw -serial stdio -smp 1 -m 512M -s -S

clean:
	rm -rf $(BUILD_DIR)

show_sources:
	@echo "Kernel C Sources:"
	@for file in $(KERNEL_C_SOURCES); do echo $$file; done
	@echo "Kernel ASM Sources:"
	@for file in $(KERNEL_ASM_SOURCES); do echo $$file; done
	@echo "Kernel Object Files:"
	@for file in $(KERNEL_OBJECTS); do echo $$file; done

# debug_symbols:
# 	@echo "=== Symbols in libc.a ==="
# 	nm build/libc.a | grep -E "(strlen|abort)"
# 	@echo "=== Undefined symbols in kernel objects ==="
# 	nm -u build/src/kernel/drivers/tty/tty.o | grep strlen
# 	nm -u build/src/kernel/stack_protector.o | grep abort

# debug_linked:
# 	@echo "=== Symbols in final kernel.elf ==="
# 	nm $(KERNEL_ELF) | grep -E "(strlen|abort)"
# 	@echo "=== Sections in kernel.elf ==="
# 	objdump -h $(KERNEL_ELF)
