# kernel/fs/Makefile

SRC				:= vfs.c

MODULE_OBJDIR	:= $(OBJDIR)/fs
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all