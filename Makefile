# Global part
ISO_NAME	:= kfs.iso
BIN_NAME	:= bin/kernel
ASM_NAME	:= bin/bootload
BIN			:= bin/

OBJDIR		:= objs/

# C part
CXX			:= gcc
CXXFLGS		:= -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs
DBGFLGS		:= -g3 -fsanitize=address,leak

CSRC		:= sources/test_c.c

OBJC		:= $(CSRC:%.c=$(OBJDIR)%.o)

# ASM part
ASMXX		:= nasm
ASMFLGS		:= -f elf64 -g
LDXX		:= ld
LDFLGS		:= -m elf_x86_64

ASMSRC		:= bootloader/bootloader.s

OBJASM		:= $(ASMSRC:%.s=$(OBJDIR)%.o)

# Rules
all: $(ISO_NAME)

$(ISO_NAME): $(OBJASM) $(OBJC)
	mkdir -p $(BIN)
# 	$(LDXX) $(LDFLGS) $(OBJASM) -o $(ASM_NAME) segfault car appeler directement en tant que runtime, link avec gcc = lancer depuis un runtime et ret ne segfault pas
	$(CXX) $(OBJASM) -o $(ASM_NAME)
	$(CXX) $(DBGFLGS) $(OBJC) -o $(BIN_NAME)

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