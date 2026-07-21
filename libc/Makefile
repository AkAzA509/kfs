# Ce Makefile ne connaît que ses propres sources.
# CC, AR, CFLAGS, CPPFLAGS sont hérités (exportés) du Makefile racine.
# OBJDIR est passé par le Makefile racine (release ou debug).

OBJDIR		?= ../objs/libc

SRC			:= string/memcpy.c string/memmove.c string/memset.c string/strlen.c \
			   string/strcmp.c \
			   stdio/putchar.c stdio/vsnprintf.c stdio/vprintf.c stdio/vprint_core.c \
			   stdio/vfprintf.c stdio/vdprintf.c stdio/vsprintf.c \
			   stdio/printf.c stdio/sprintf.c stdio/dprintf.c stdio/snprintf.c stdio/fprintf.c

OBJ			:= $(SRC:%.c=$(OBJDIR)/%.o)

all: $(OBJDIR)/libc.a

$(OBJDIR)/libc.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean
