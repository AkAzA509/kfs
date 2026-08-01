# libc/stdio/Makefile

SRC				:= dprintf.c \
				   fflush.c \
				   fprintf.c \
				   fputc.c \
				   printf.c \
				   putchar.c \
				   snprintf.c \
				   sprintf.c \
				   stdio.c \
				   vdprintf.c \
				   vfprintf.c \
				   vprint_core.c \
				   vprintf.c \
				   vsnprintf.c \
				   vsprintf.c

MODULE_OBJDIR	:= $(OBJDIR)/stdio
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all