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

DBGFLAGS			:= -DDEBUG=1

LIBC_INCLUDES		:= -I$(abspath libc/include)
KERNEL_INCLUDES		:= -I$(abspath kernel/include) $(LIBC_INCLUDES)

export CFLAGS		:= -std=gnu11 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Werror


# --- Release ---
.PHONY: all libc kernel up dev compile_commands

all: kernel $(BIN_NAME)

libc:
	@$(MAKE) -C libc OBJDIR=$(OBJDIR)/libc CPPFLAGS="$(LIBC_INCLUDES)"

kernel: libc
	@$(MAKE) -C kernel OBJDIR=$(OBJDIR)/kernel CPPFLAGS="$(KERNEL_INCLUDES)"

$(BIN_NAME): $(OBJDIR)/kernel/kernel.a $(OBJDIR)/libc/libc.a $(LINKER_SCRIPT)
	@mkdir -p $(BIN)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) $(CPPFLAGS) \
		-Wl,--start-group $(OBJDIR)/kernel/kernel.a \
						  $(OBJDIR)/libc/libc.a -Wl,--end-group -lgcc
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

$(ISO_NAME): $(BIN_NAME) $(GRUB_CFG)
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/boot/grub
	@cp $(BIN_NAME) $(BUILD_DIR)/boot/kernel
	@cp $(GRUB_CFG) $(BUILD_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $@ $(BUILD_DIR)

compile_commands:
	bear -- $(MAKE) re

# --- Debug ---
.PHONY: debug debug-libc debug-kernel

debug: debug-kernel $(DEBUG_ISO_NAME)
	@qemu-system-i386 -cdrom $(DEBUG_ISO_NAME) -serial stdio

debug-libc:
	@$(MAKE) -C libc OBJDIR="$(DEBUG_OBJDIR)/libc" CFLAGS="$(CFLAGS) $(DBGFLAGS)" CPPFLAGS="$(LIBC_INCLUDES)"

debug-kernel: debug-libc
	@$(MAKE) -C kernel OBJDIR="$(DEBUG_OBJDIR)/kernel" CFLAGS="$(CFLAGS) $(DBGFLAGS)" CPPFLAGS="$(KERNEL_INCLUDES)"

$(DEBUG_NAME): $(DEBUG_OBJDIR)/kernel/kernel.a $(DEBUG_OBJDIR)/libc/libc.a $(LINKER_SCRIPT)
	@mkdir -p $(BIN)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) $(CPPFLAGS) $(DBGFLAGS) \
		-Wl,--start-group $(DEBUG_OBJDIR)/kernel/kernel.a \
		                  $(DEBUG_OBJDIR)/libc/libc.a -Wl,--end-group -lgcc
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

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

# --- Test ---
.PHONY: test

test:
	@$(MAKE) -C tests

# --- Clean ---
.PHONY: clean fclean re

clean:
	@$(MAKE) -C tests clean
	rm -rf $(OBJDIR) $(DEBUG_OBJDIR)

fclean: clean
	rm -rf $(BIN) $(BUILD_DIR) $(DEBUG_BUILD_DIR)

re: fclean all