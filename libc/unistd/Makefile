# libc/unistd/Makefile

SRC				:= write.c

MODULE_OBJDIR	:= $(OBJDIR)/unistd
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all