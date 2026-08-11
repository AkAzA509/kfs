# ==============================================================================
# Libc test makefile for KFS (Kernel From Scratch)
#
# Compiles the libc kernel into freestanding, renames its global symbols
# (kfs_<name>) via objcopy to avoid collision with the host libc,
# then compile+link each test file (host gcc + libc system) against
# these renowned objects.
#
# ==============================================================================

LIBC_DIR		:= ../libc
LIBC_INC		:= $(LIBC_DIR)/include
BUILD_DIR		:= objs

STUB_SRCS		:= $(wildcard stubs/*.c)
STUB_OBJS		:= $(patsubst stubs/%.c,$(BUILD_DIR)/stubs/%.o,$(STUB_SRCS))

LIBC_SRCS		:= $(wildcard $(LIBC_DIR)/stdio/*.c)
LIBC_SRCS		+= $(wildcard $(LIBC_DIR)/string/*.c)
# LIBC_SRCS		:= $(shell find $(LIBC_DIR) -name '*.c' -not -path '*/testing/*')
LIBC_OBJS		:= $(patsubst $(LIBC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIBC_SRCS))
RENAMED_OBJS	:= $(patsubst $(LIBC_DIR)/%.c,$(BUILD_DIR)/%.renamed.o,$(LIBC_SRCS))

TEST_SRCS		:= test_stdio.c test_string.c
TEST_BINS		:= $(patsubst %.c,$(BUILD_DIR)/%,$(TEST_SRCS))

CC				:= gcc
OBJCOPY			:= objcopy

KFS_CFLAGS		:= -Wall -Wextra -Werror -ffreestanding -fno-builtin -nostdinc \
				   -I$(LIBC_INC) -O2

HOST_CFLAGS		:= -Wall -Wextra -g #-I$(LIBC_INC)

# --- Rules ---
.PHONY: all test clean

all: test

test: $(TEST_BINS)
	@echo "=================================================="
	@echo " Starting KFS's unit testing suit..."
	@echo "=================================================="
	@status=0; \
	for bin in $(TEST_BINS); do \
		echo "--- $$bin ---"; \
		./$$bin || status=1; \
	done; \
	exit $$status

$(BUILD_DIR)/%: %.c $(RENAMED_OBJS) $(STUB_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(HOST_CFLAGS) $< $(RENAMED_OBJS) $(STUB_OBJS) -o $@

$(BUILD_DIR)/%.o: $(LIBC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(KFS_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.renamed.o: $(BUILD_DIR)/%.o
	$(OBJCOPY) --prefix-symbols=kfs_ $< $@

$(BUILD_DIR)/stubs/%.o: stubs/%.c
	@mkdir -p $(dir $@)
	$(CC) $(HOST_CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)