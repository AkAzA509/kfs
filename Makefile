# Global part
BIN			:= bin
BUILD_DIR	:= iso
ISO_NAME	:= $(BIN)/kfs.iso
BIN_NAME	:= $(BIN)/kernel
UP_DIR		:= $(BIN)/boot/grub

OBJDIR		:= objs/

# C part
CXX			:= $(TARGET)-gcc
CXXFLGS		:= -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs
DBGFLGS		:= -g3

CSRC		:= kernel/vga_color.c \
			   kernel/kernel.c \
			   kernel/kwrite.c \
			   kernel/io.c \
			   kernel/drivers/keyboard.c \
			   helpers/kprint/kprint.c \
			   helpers/kprint/utils.c \
			   helpers/kprint/convert_format.c \
			   helpers/memcpy.c \
			   helpers/memset.c \
			   helpers/strlen.c

OBJC		:= $(CSRC:%.c=$(OBJDIR)%.o)

# ASM part
ASMXX		:= nasm
ASMFLGS		:= -f elf32 -g
LDXX		:= ld
LDFLGS		:= -m elf_i386

ASMSRC		:= bootloader/bootloader.s

OBJASM		:= $(ASMSRC:%.s=$(OBJDIR)%.o)

# Rules
all: $(ISO_NAME)

$(ISO_NAME): $(OBJASM) $(OBJC)
	@mkdir -p $(BIN)
	$(CXX) -T linker.ld -o $(BIN_NAME) $(CXXFLGS) $(OBJASM) $(OBJC)
	@if grub-file --is-x86-multiboot $(BIN_NAME); then \
		echo "\033[92mmultiboot confirmed\033[0m"; \
	else \
		echo "\033[91mthe file is not multiboot\033[0m"; \
	fi

$(OBJDIR)%.o: %.s
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) $< -o $@

$(OBJDIR)%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLGS) -c $< -o $@

prepare_iso: all
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/boot/grub
	@cp $(BIN_NAME) $(BUILD_DIR)/boot/kernel
	@cp grub.cfg $(BUILD_DIR)/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_NAME) $(BUILD_DIR)

up: prepare_iso
	@qemu-system-i386 -cdrom $(ISO_NAME)
	

dev: prepare_iso
	@qemu-system-i386 -kernel $(BUILD_DIR)/boot/kernel

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(BIN) $(BUILD_DIR)

re: fclean all

.PHONY: all up clean fclean re