# Global part
ISO_NAME	:= kfs.iso
BIN_NAME	:= bin/kernel
BIN			:= bin/

OBJDIR		:= objs/

# C part
CXX			:= $(TARGET)-gcc
CXXFLGS		:= -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs
DBGFLGS		:= -g3

CSRC		:= sources/kernel.c

OBJC		:= $(CSRC:%.c=$(OBJDIR)%.o)

# ASM part
ASMXX		:= nasm
ASMFLGS		:= -f elf32 -g
LDXX		:= ld
LDFLGS		:= -m elf_i386 -T linker.ld

ASMSRC		:= bootloader/bootloader.s

OBJASM		:= $(ASMSRC:%.s=$(OBJDIR)%.o)

# Rules
all: $(ISO_NAME)

$(ISO_NAME): $(OBJASM) $(OBJC)
	mkdir -p $(BIN)
# 	$(LDXX) $(LDFLGS) $(OBJASM) -o $(ASM_NAME) segfault car appeler directement en tant que runtime, link avec gcc = lancer depuis un runtime et ret ne segfault pas
	$(CXX) $(CXXFLGS) $(OBJASM) $(OBJC) -o $(BIN_NAME)

$(OBJDIR)%.o: %.s
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) $< -o $@

$(OBJDIR)%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX) $(DBGFLGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(BIN)

re: fclean all

.PHONY: all clean fclean re