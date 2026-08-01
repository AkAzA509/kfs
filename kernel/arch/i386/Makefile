# kernel/arch/i386/Makefile

SRC				:= gdt.c

MODULE_OBJDIR	:= $(OBJDIR)/arch/i386
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all