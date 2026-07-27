# Ce Makefile ne connaît que ses propres sources.
# CC, CFLAGS, CPPFLAGS sont hérités (exportés) du Makefile racine.
# OBJDIR est passé par le Makefile racine (release ou debug).

OBJDIR		?= ../objs/kernel

SRC			:= kernel/kernel.c kernel/io.c kernel/init.c kernel/kwrite.c kernel/tty.c \
			   kernel/gdt.c kernel/log.c kernel/shell.c \
			   arch/i386/console.c arch/i386/vga.c arch/i386/keyboard.c arch/i386/framebuffer.c \

ifdef DEBUG
SRC			+= testing/test_printf.c testing/test_screen.c
endif

OBJ			:= $(SRC:%.c=$(OBJDIR)/%.o)

all: echo $(OBJDIR)/kernel.a reset

echo:
	@echo "\n\033[95mStart compiling kernel sources\n"

reset:
	@echo "\033[0m"

$(OBJDIR)/kernel.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
