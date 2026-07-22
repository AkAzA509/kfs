# Compilation
export CC			:= $(TARGET)-gcc
export AR			:= $(TARGET)-ar

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

# Rules
.PHONY: all debug libc asm kernel debug-libc debug-asm debug-kernel clean fclean re up dev compile_commands

all: kernel $(BIN_NAME)

# debug: DBGFLAGS := $(DBGFLAGS)
debug: debug-kernel $(DEBUG_ISO_NAME)
	@qemu-system-i386 -cdrom $(DEBUG_ISO_NAME) -serial stdio

# --- release ---
libc:
	@$(MAKE) -C libc OBJDIR=$(OBJDIR)/libc

asm:
	@$(MAKE) -C kernel/arch/i386/asm OBJDIR=$(OBJDIR)/asm
	
kernel: asm libc
	@$(MAKE) -C kernel OBJDIR=$(OBJDIR)/kernel

# --- debug ---
debug-libc:
	@$(MAKE) -C libc OBJDIR=$(DEBUG_OBJDIR)/libc CFLAGS="$(CFLAGS) $(DBGFLAGS)"

debug-asm:
	@$(MAKE) -C kernel/arch/i386/asm OBJDIR="$(DEBUG_OBJDIR)/asm"
	
debug-kernel: debug-asm debug-libc
	@$(MAKE) -C kernel OBJDIR=$(DEBUG_OBJDIR)/kernel CFLAGS="$(CFLAGS) $(DBGFLAGS)" DEBUG=1

# --- final link ---
$(BIN_NAME): kernel $(OBJDIR)/asm/asm.a $(OBJDIR)/kernel/kernel.a $(OBJDIR)/libc/libc.a $(LINKER_SCRIPT)
	@mkdir -p $(BIN)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) \
		-Wl,--start-group $(OBJDIR)/asm/asm.a \
						  $(OBJDIR)/kernel/kernel.a \
						  $(OBJDIR)/libc/libc.a -Wl,--end-group -lgcc
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

$(DEBUG_NAME): $(DEBUG_OBJDIR)/asm/asm.a $(DEBUG_OBJDIR)/kernel/kernel.a $(DEBUG_OBJDIR)/libc/libc.a $(LINKER_SCRIPT)
	@mkdir -p $(BIN)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) $(DBGFLAGS) \
		-Wl,--start-group $(DEBUG_OBJDIR)/asm/asm.a $(DEBUG_OBJDIR)/kernel/kernel.a $(DEBUG_OBJDIR)/libc/libc.a -Wl,--end-group -lgcc
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
	@$(MAKE) -C kernel/arch/i386/asm OBJDIR=$(OBJDIR)/asm clean
	@$(MAKE) -C kernel/arch/i386/asm OBJDIR=$(DEBUG_OBJDIR)/asm clean
	rm -rf $(OBJDIR) $(DEBUG_OBJDIR)

fclean: clean
	rm -rf $(BIN) $(BUILD_DIR) $(DEBUG_BUILD_DIR)

re: fclean all