# Makefile

.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

TARGET			?= i386-elf
ASM				:= nasm
CC				:= $(TARGET)-gcc
AR				:= $(TARGET)-ar
RM				:= rm -rf

DEBUG			?= 0

BIN_DIR			:= bin
BUILD_DIR		:= $(if $(filter 1,$(DEBUG)),objs_debug,objs)
ISO_DIR			:= $(if $(filter 1,$(DEBUG)),iso_debug,iso)

BIN_NAME		:= $(BIN_DIR)/kernel$(if $(filter 1,$(DEBUG)),_debug,)
ISO_NAME		:= $(BIN_DIR)/kfs$(if $(filter 1,$(DEBUG)),_debug,).iso

KERNEL_A		:= $(BUILD_DIR)/kernel/kernel.a
LIBC_A			:= $(BUILD_DIR)/libc/libc.a

LINKER_SCRIPT	:= kernel/arch/i386/linker.ld
GRUB_CFG		:= grub.cfg

CPPFLAGS		:= -Ilibc/include -Ikernel/include -MMD -MP
CFLAGS			:= -std=gnu23 -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Werror #-O2
ASMFLAGS		:= -f elf32 -I.
LDFLAGS			:= -T $(LINKER_SCRIPT) -Wl,--start-group $(KERNEL_A) $(LIBC_A) -Wl,--end-group -lgcc

ifeq ($(DEBUG),1)
	CPPFLAGS	+= -DDEBUG=1
	CFLAGS		+= -g3 -O0
	ASMFLAGS	+= -g -F stabs
endif

MAKEFLAGS		+= --no-print-directory

# --- Sources & Objects ---
# LIBC_SRCS_C		:= $(shell find libc -type f -name '*.c' 2>/dev/null)
LIBC_SRCS_C		:= $(filter-out libc/stdlib/%, $(shell find libc -type f -name '*.c' 2>/dev/null)) \
				libc/stdlib/labs.c
KERNEL_SRCS_C	:= $(shell find kernel -type f -name '*.c' 2>/dev/null)
KERNEL_SRCS_S	:= $(shell find kernel -type f -name '*.s' 2>/dev/null)

LIBC_OBJS		:= $(patsubst libc/%.c,$(BUILD_DIR)/libc/%.o,$(LIBC_SRCS_C))
KERNEL_OBJS		:= $(patsubst kernel/%.c,$(BUILD_DIR)/kernel/%.o,$(KERNEL_SRCS_C)) \
				   $(patsubst kernel/%.s,$(BUILD_DIR)/kernel/%.o,$(KERNEL_SRCS_S))

ALL_OBJS		:= $(LIBC_OBJS) $(KERNEL_OBJS)
DEPS			:= $(ALL_OBJS:.o=.d)

# --- Targets ---
.PHONY: all libc kernel

all: $(BIN_NAME)

libc: $(LIBC_A)

kernel: $(KERNEL_A)

$(BUILD_DIR)/libc/%.o: libc/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: kernel/%.s
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(LIBC_A): $(LIBC_OBJS)
	@mkdir -p $(dir $@)
	@echo "\n\033[94mPacking libc sources\n"
	$(AR) rcs $@ $(LIBC_OBJS)
	@echo "\033[0m"

$(KERNEL_A): $(KERNEL_OBJS)
	@mkdir -p $(dir $@)
	@echo "\n\033[95mPacking kernel sources\n"
	$(AR) rcs $@ $(KERNEL_OBJS)
	@echo "\033[0m"

$(BIN_NAME): $(KERNEL_A) $(LIBC_A) $(LINKER_SCRIPT)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; exit 1; \
	fi

$(ISO_NAME): $(BIN_NAME) $(GRUB_CFG)
	@$(RM) $(ISO_DIR)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(BIN_NAME) $(ISO_DIR)/boot/kernel
	@cp $(GRUB_CFG) $(ISO_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $@ $(ISO_DIR)

# --- Execution & Utility ---
.PHONY: up dev compile_commands

compile_commands:
	bear -- $(MAKE) re

up: $(ISO_NAME)
	@qemu-system-i386 -cdrom $(ISO_NAME)

dev: $(BIN_NAME)
	@qemu-system-i386 -kernel $(BIN_NAME)

# --- Debug Commands ---
.PHONY: gdb debug debug-libc debug-kernel

debug:
	@$(MAKE) all DEBUG=1

debug-libc:
	@$(MAKE) libc DEBUG=1

debug-kernel:
	@$(MAKE) kernel DEBUG=1

debug-iso:
	@$(MAKE) $(BIN_DIR)/kfs_debug.iso DEBUG=1

debug-up: debug-iso
	@qemu-system-i386 -cdrom $(BIN_DIR)/kfs_debug.iso -serial stdio
gdb:
	@$(MAKE) $(ISO_NAME) DEBUG=1
	@qemu-system-i386 -cdrom $(BIN_DIR)/kfs_debug.iso -serial file:serial.log -s -S &
	gdb -x .gdbinit $(BIN_DIR)/kernel_debug

# --- Linting / Formatting ---
.PHONY: format lint

format:
	clang-format -i $(KERNEL_SRCS_C) $(LIBC_SRCS_C)

lint:
	clang-tidy $(KERNEL_SRCS_C) $(LIBC_SRCS_C) -- $(CPPFLAGS) $(CFLAGS)

# --- Tests ---
.PHONY: test

test:
	@if [ -d tests ]; then $(MAKE) -C tests; fi

# --- Cleaning ---
.PHONY: clean fclean re

clean:
	@if [ -d tests ]; then $(MAKE) -C tests clean; fi
	$(RM) objs objs_debug

fclean: clean
	$(RM) $(BIN_DIR) iso iso_debug

re: fclean all

# --- Automatic Dependencies Inclusion ---
ifeq (,$(filter clean fclean re,$(MAKECMDGOALS)))
-include $(DEPS)
endif
