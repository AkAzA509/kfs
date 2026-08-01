# kernel/arch/i386/asm/Makefile

SRC				:= bootloader.s \
				   fpu.s \
				   gdt.s

MODULE_OBJDIR	:= $(OBJDIR)/arch/i386/asm
OBJ				:= $(SRC:%.s=$(MODULE_OBJDIR)/%.o)

ASMXX		:= nasm
ASMFLGS		:= -f elf32 -g

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) $< -o $@

.PHONY: all