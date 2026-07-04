# Global part
BIN					:= bin
BUILD_DIR			:= iso
ISO_NAME			:= $(BIN)/kfs.iso
BIN_NAME			:= $(BIN)/kernel
DEBUG_NAME			:= $(BIN)/kernel_debug
DEBUG_BUILD_DIR		:= iso_debug
DEBUG_ISO_NAME		:= $(BIN)/kfs_debug.iso
UP_DIR				:= $(BIN)/boot/grub

OBJDIR				:= objs/
DEBUG_OBJDIR		:= objs_debug/

# C part
CXX					:= $(TARGET)-gcc
CXXFLAGS			:= -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Iinclude
DBGFLAGS			:= -DDEBUG=1

CSRC				:= kernel/vga_color.c kernel/kernel.c kernel/printer.c kernel/io.c kernel/display.c \
					   kernel/drivers/keyboard.c \
					   helpers/kprint/kprint.c helpers/kprint/utils.c helpers/kprint/convert_format.c \
					   helpers/memcpy.c helpers/memset.c helpers/memmove.c helpers/strlen.c helpers/kputchar.c helpers/kwrite.c

OBJC				:= $(CSRC:%.c=$(OBJDIR)%.o)
DEBUG_OBJC			:= $(CSRC:%.c=$(DEBUG_OBJDIR)%.o)

# ASM part
ASMXX				:= nasm
ASMFLGS				:= -f elf32 -g
LDXX				:= ld
LDFLGS				:= -m elf_i386

ASMSRC				:= bootloader/bootloader.s

OBJASM				:= $(ASMSRC:%.s=$(OBJDIR)%.o)
DEBUG_OBJASM		:= $(ASMSRC:%.s=$(DEBUG_OBJDIR)%.o)

CONFIG_H			:= includes/config.h
CONFIG_INC			:= $(OBJDIR)config.inc
DEBUG_CONFIG_INC	:= $(DEBUG_OBJDIR)config.inc

# Rules
# base rule, build the kernel binary
all: $(BIN_NAME)

# add the flag for the debug mode, call the iso rule and launch it
debug: CXXFLAGS += $(DBGFLAGS)
debug: $(DEBUG_ISO_NAME)
	@qemu-system-i386 -cdrom $(DEBUG_ISO_NAME) -serial stdio

# call the objects rules, link and check the multiboot config
$(BIN_NAME): $(OBJASM) $(OBJC) linker.ld
	@mkdir -p $(BIN)
	$(CXX) -T linker.ld -o $@ $(CXXFLAGS) $(OBJASM) $(OBJC)
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

# call the debug objects rules, link and check the multiboot config
$(DEBUG_NAME): $(DEBUG_OBJASM) $(DEBUG_OBJC) linker.ld
	@mkdir -p $(BIN)
	$(CXX) -T linker.ld -o $@ $(CXXFLAGS) $(DEBUG_OBJASM) $(DEBUG_OBJC)
	@if grub-file --is-x86-multiboot $@; then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

# generate config.inc (nasm syntax) from config.h (C syntax), single source of truth = config.h
$(CONFIG_INC): $(CONFIG_H)
	@mkdir -p $(dir $@)
	@grep -E '^\s*#define\s+(VIDEO_MODE|MODE_VGA|MODE_FRAMEBUFFER)\b' $(CONFIG_H) \
		| sed -E 's/^\s*#define\s+([A-Z_]+)\s+([A-Za-z0-9_]+).*$$/%define \1 \2/' > $@

$(DEBUG_CONFIG_INC): $(CONFIG_H)
	@mkdir -p $(dir $@)
	@grep -E '^\s*#define\s+(VIDEO_MODE|MODE_VGA|MODE_FRAMEBUFFER)\b' $(CONFIG_H) \
		| sed -E 's/^\s*#define\s+([A-Z_]+)\s+([A-Za-z0-9_]+).*$$/%define \1 \2/' > $@

# create the bin objects
$(OBJDIR)%.o: %.s $(CONFIG_INC)
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) -I$(OBJDIR) $< -o $@

$(OBJDIR)%.o: %.c $(CONFIG_H)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# create the debug objects
$(DEBUG_OBJDIR)%.o: %.s $(DEBUG_CONFIG_INC)
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) -I$(DEBUG_OBJDIR) $< -o $@

$(DEBUG_OBJDIR)%.o: %.c $(CONFIG_H)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# create the iso and is tree
$(ISO_NAME): $(BIN_NAME) grub.cfg
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/boot/grub
	@cp $(BIN_NAME) $(BUILD_DIR)/boot/kernel
	@cp grub.cfg $(BUILD_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $@ $(BUILD_DIR)

# create the debug iso and is tree
$(DEBUG_ISO_NAME): $(DEBUG_NAME) grub.cfg
	@rm -rf $(DEBUG_BUILD_DIR)
	@mkdir -p $(DEBUG_BUILD_DIR)/boot/grub
	@cp $(DEBUG_NAME) $(DEBUG_BUILD_DIR)/boot/kernel
	@cp grub.cfg $(DEBUG_BUILD_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $@ $(DEBUG_BUILD_DIR)

up: $(ISO_NAME)
	@qemu-system-i386 -cdrom $(ISO_NAME) -serial stdio

dev: $(BIN_NAME)
	@qemu-system-i386 -kernel $(BIN_NAME)

clean:
	rm -rf $(OBJDIR) $(DEBUG_OBJDIR)

fclean: clean
	rm -rf $(BIN) $(BUILD_DIR) $(DEBUG_BUILD_DIR)

re: fclean all

.PHONY: up dev clean fclean re debug all