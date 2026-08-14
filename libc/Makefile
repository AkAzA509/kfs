# libc/Makefile

export CC		?= $(TARGET)-gcc
export AR		?= $(TARGET)-ar
export CPPFLAGS	?= -I$(abspath libc/include)
export CFLAGS	?= -fno-builtin -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Werror $(CPPFLAGS)
export OBJDIR	?= $(CURDIR)/../objs/libc

SUBDIRS			:= stdio string unistd stdlib
TARGET			:= $(OBJDIR)/libc.a

ALL_OBJ			:= $(shell find $(OBJDIR) -name "*.o" 2>/dev/null)

all: start_echo $(SUBDIRS)
	$(MAKE) $(TARGET)
	$(MAKE) reset_echo

start_echo:
	@echo "\n\033[94mStart compiling libc sources\n"

reset_echo:
	@echo "\033[0m"

$(SUBDIRS):
	@$(MAKE) -C $@

$(TARGET): $(ALL_OBJ)
	@mkdir -p $(OBJDIR)
	$(AR) rcs $@ $(shell find $(OBJDIR) -name "*.o")

clean:
	rm -rf $(OBJDIR)

.PHONY: all $(SUBDIRS) clean start_echo reset_echo
