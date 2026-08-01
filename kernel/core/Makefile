# kernel/core/Makefile

SRC				:= init.c \
				   io.c \
				   kernel.c \
				   log.c \
				   shell.c \
				   tty.c

MODULE_OBJDIR	:= $(OBJDIR)/core
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all