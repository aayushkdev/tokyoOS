# Directories
SRC_DIR := src
BUILD_DIR := bin
DIST_DIR := dist

# Source files
C_SRC := $(shell find $(SRC_DIR)/kernel $(SRC_DIR)/lib -name '*.c')
ASM_SRC := $(SRC_DIR)/kernel/cpu/isr_codes.asm

# Object files
OBJ := $(ASM_SRC:$(SRC_DIR)/%.asm=$(BUILD_DIR)/%.o) \
       $(C_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Dependency files
DEP := $(OBJ:.o=.d)

# Target
TARGET := $(DIST_DIR)/kernel.bin
ISO := $(DIST_DIR)/tokyo.iso

# Compiler and flags
CC := $(HOME)/opt/cross/bin/x86_64-elf-gcc
LD := $(HOME)/opt/cross/bin/x86_64-elf-ld
AS := nasm
CFLAGS := -I$(SRC_DIR)/include -ffreestanding -Wall -Wextra -nostdlib -m64 -mcmodel=kernel
LDFLAGS := -m elf_x86_64 -T $(SRC_DIR)/kernel/linker.ld -z max-page-size=0x1000 -static -nostdlib
ASFLAGS := -f elf64

# Rules
all: $(ISO)

LIMINE_DIR := $(SRC_DIR)/boot/limine

# Rule to create the ISO image
$(ISO): $(TARGET)
	@mkdir -p isoDir/boot/kernel
	cp $(TARGET) isoDir/boot/kernel/kernel.bin

	@mkdir -p isoDir/boot/limine
	@mkdir -p isoDir/EFI/BOOT

	cp $(LIMINE_DIR)/limine-bios-cd.bin isoDir/boot/limine/
	cp $(LIMINE_DIR)/limine-uefi-cd.bin isoDir/boot/limine/
	cp $(LIMINE_DIR)/limine-bios.sys isoDir/boot/limine/
	cp $(LIMINE_DIR)/limine.conf isoDir/boot/limine/
	cp $(LIMINE_DIR)/BOOTX64.EFI isoDir/EFI/BOOT/

	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		isoDir -o $(DIST_DIR)/tokyo.iso
	limine bios-install $(DIST_DIR)/tokyo.iso

# Rule to create the kernel binary
$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) -o $@ $^

# Rule to assemble assembly source files
$(BUILD_DIR)/kernel/cpu/%.o: $(SRC_DIR)/kernel/cpu/%.asm
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

# Rule to compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include dependency files for C sources
-include $(DEP)

# Debug target
.PHONY: debug
debug: CFLAGS += -g
debug: all

# Run target (using QEMU)
.PHONY: run
run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO)

# Clean target
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR) isoDir
