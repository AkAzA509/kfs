# libc/stdlib/Makefile

SRC				:= labs.c \
				   # free.c \
				   # logger.c \
				   # malloc.c \
				   # realloc.c

MODULE_OBJDIR	:= $(OBJDIR)/stdlib
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all