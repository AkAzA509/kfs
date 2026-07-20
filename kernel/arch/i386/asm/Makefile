OBJDIR		?= ../../../../objs/asm

ASMXX		:= nasm
ASMFLGS		:= -f elf32 -g

SRC			:= bootloader.s #gdt.s

OBJ			:= $(SRC:%.s=$(OBJDIR)/%.o)

all: $(OBJDIR)/asm.a

$(OBJDIR)/asm.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

$(OBJDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(ASMXX) $(ASMFLGS) $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean