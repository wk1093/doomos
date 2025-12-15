# ===============================
# DoomOS GRUB Build System
# ===============================

CC      = i386-elf-gcc
LD      = i386-elf-ld
OBJCOPY = i386-elf-objcopy
AS      = nasm
CFLAGS  = -m32 -ffreestanding -Wall -Wextra -nostdlib -I src/drivers -I src/kernel -I src/libc \
 -fno-exceptions -fno-stack-protector -fomit-frame-pointer -fno-builtin -Wno-incompatible-pointer-types -Wno-unused-parameter
LDFLAGS = -T linker.ld

ifneq (,$(filter debug,$(MAKECMDGOALS)))
	CFLAGS += -g -DDEBUG -O0
else
	CFLAGS += -O2
endif

SRC     = $(wildcard src/kernel/*.c src/drivers/*.c src/libc/*.c src/kernel/**/*.c)
ASM     = $(wildcard src/kernel/*.asm src/kernel/**/*.asm)
OBJ     = $(patsubst src/%.c,$(BUILD)/%.o,$(SRC)) \
		  $(patsubst src/%.asm,$(BUILD)/%.o,$(ASM))
BUILD   = build
KERNEL_ELF = $(BUILD)/kernel.elf
ISO     = $(BUILD)/doomos.iso

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

# Compile kernel

$(BUILD)/%.o: src/%.c | $(BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.asm | $(BUILD)
	mkdir -p $(dir $@)
	$(AS) -f elf32 $< -o $@

$(KERNEL_ELF): $(OBJ) | $(BUILD)
	$(LD) -m elf_i386 $(LDFLAGS) -o $@ $(OBJ)

# Create GRUB ISO
$(ISO): $(KERNEL_ELF)
	mkdir -p $(BUILD)/iso/boot/grub
	cp $< $(BUILD)/iso/boot/kernel.elf
	cp boot/grub/grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(BUILD)/iso

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

kvm: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -enable-kvm

debug: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -S -gdb tcp::1234

debugkvm: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -enable-kvm -S -gdb tcp::1234

debug_cmd: $(ISO)
	gdb $(KERNEL_ELF) -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_ELF)"

clean:
	rm -rf $(BUILD)
