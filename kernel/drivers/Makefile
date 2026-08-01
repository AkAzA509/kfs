# kernel/drivers/Makefile

SRC				:= console.c \
				   framebuffer.c \
				   keyboard.c \
				   vga.c

MODULE_OBJDIR	:= $(OBJDIR)/drivers
OBJ				:= $(SRC:%.c=$(MODULE_OBJDIR)/%.o)

all: $(OBJ)

$(MODULE_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: all