# Ce Makefile ne connaît que ses propres sources.
# CC, CFLAGS, CPPFLAGS sont hérités (exportés) du Makefile racine.
# OBJDIR est passé par le Makefile racine (release ou debug).

OBJDIR		?= ../objs/kernel

SRC			:= kernel/kernel.c kernel/io.c kernel/init.c kernel/kwrite.c \
			   kernel/gdt.c \
			   arch/i386/tty.c arch/i386/vga.c arch/i386/keyboard.c arch/i386/framebuffer.c \
			   testing/test_printf.c

OBJ			:= $(SRC:%.c=$(OBJDIR)/%.o)

all: $(OBJDIR)/kernel.a

$(OBJDIR)/kernel.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
