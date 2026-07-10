# Compilation
export CC			:= $(TARGET)-gcc
export AR			:= $(TARGET)-ar
ASMXX				:= nasm
ASMFLGS				:= -f elf32 -g

# Global part
BIN					:= bin
BUILD_DIR			:= iso
DEBUG_BUILD_DIR		:= iso_debug
ISO_NAME			:= $(BIN)/kfs.iso
DEBUG_ISO_NAME		:= $(BIN)/kfs_debug.iso
BIN_NAME			:= $(BIN)/kernel
DEBUG_NAME			:= $(BIN)/kernel_debug

OBJDIR				:= $(abspath objs)
DEBUG_OBJDIR		:= $(abspath objs_debug)

LINKER_SCRIPT		:= kernel/arch/i386/linker.ld
GRUB_CFG			:= grub.cfg

# Shared flags
export CPPFLAGS		:= -I$(abspath libc/include) -I$(abspath kernel/include)
export CFLAGS		:= -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Werror $(CPPFLAGS)
DBGFLAGS			:= -DDEBUG=1 -g

# ASM part
ASMSRC				:= kernel/arch/i386/bootloader.s
OBJASM				:= $(OBJDIR)/boot/bootloader.o
DEBUG_OBJASM		:= $(DEBUG_OBJDIR)/boot/bootloader.o

# Rules
.PHONY: all debug libc kernel debug-libc debug-kernel clean fclean re up dev compile_commands

all: $(BIN_NAME)

debug: DBGFLAGS := $(DBGFLAGS)
debug: $(DEBUG_ISO_NAME)
	@qemu-system-i386 -cdrom $(DEBUG_ISO_NAME) -serial stdio

# --- release ---
libc:
	@$(MAKE) -C libc OBJDIR=$(OBJDIR)/libc

kernel: libc
	@$(MAKE) -C kernel OBJDIR=$(OBJDIR)/kernel

# --- debug ---
debug-libc:
	@$(MAKE) -C libc OBJDIR=$(DEBUG_OBJDIR)/libc CFLAGS="$(CFLAGS) $(DBGFLAGS)"

debug-kernel: debug-libc
	@$(MAKE) -C kernel OBJDIR=$(DEBUG_OBJDIR)/kernel CFLAGS="$(CFLAGS) $(DBGFLAGS)"

# --- asm objet ---
$(OBJDIR)/boot/bootloader.o: $(ASMSRC)
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) $< -o $@

$(DEBUG_OBJDIR)/boot/bootloader.o: $(ASMSRC)
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) $< -o $@

# --- final link ---
$(BIN_NAME): kernel $(OBJASM) $(LINKER_SCRIPT)
	@mkdir -p $(BIN)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) $(OBJASM) \
		-Wl,--start-group $(OBJDIR)/kernel/kernel.a $(OBJDIR)/libc/libc.a -Wl,--end-group
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

$(DEBUG_NAME): debug-kernel $(DEBUG_OBJASM) $(LINKER_SCRIPT)
	@mkdir -p $(BIN)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) $(DBGFLAGS) $(DEBUG_OBJASM) \
		-Wl,--start-group $(DEBUG_OBJDIR)/kernel/kernel.a $(DEBUG_OBJDIR)/libc/libc.a -Wl,--end-group
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

# --- ISO ---
$(ISO_NAME): $(BIN_NAME) $(GRUB_CFG)
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/boot/grub
	@cp $(BIN_NAME) $(BUILD_DIR)/boot/kernel
	@cp $(GRUB_CFG) $(BUILD_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $@ $(BUILD_DIR)

$(DEBUG_ISO_NAME): $(DEBUG_NAME) $(GRUB_CFG)
	@rm -rf $(DEBUG_BUILD_DIR)
	@mkdir -p $(DEBUG_BUILD_DIR)/boot/grub
	@cp $(DEBUG_NAME) $(DEBUG_BUILD_DIR)/boot/kernel
	@cp $(GRUB_CFG) $(DEBUG_BUILD_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $@ $(DEBUG_BUILD_DIR)

up: $(ISO_NAME)
	@qemu-system-i386 -cdrom $(ISO_NAME)

dev: $(BIN_NAME)
	@qemu-system-i386 -kernel $(BIN_NAME)

compile_commands:
	bear -- $(MAKE) re

clean:
	@$(MAKE) -C libc OBJDIR=$(OBJDIR)/libc clean
	@$(MAKE) -C libc OBJDIR=$(DEBUG_OBJDIR)/libc clean
	@$(MAKE) -C kernel OBJDIR=$(OBJDIR)/kernel clean
	@$(MAKE) -C kernel OBJDIR=$(DEBUG_OBJDIR)/kernel clean
	rm -rf $(OBJDIR) $(DEBUG_OBJDIR)

fclean: clean
	rm -rf $(BIN) $(BUILD_DIR) $(DEBUG_BUILD_DIR)

re: fclean all