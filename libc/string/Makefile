# libc/string/Makefile

SRC				:= memcmp.c \
				   memcpy.c \
				   memmove.c \
				   memset.c \
				   strcmp.c \
				   strlen.c \
				   strncmp.c

MODULE_OBJDIR	:= $(OBJDIR)/string
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all