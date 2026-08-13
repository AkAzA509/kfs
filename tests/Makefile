# tests/Makefile

LIBC_DIR		:= ../libc
LIBC_INC		:= $(LIBC_DIR)/include
BUILD_DIR		:= objs

STUB_SRCS		:= $(wildcard stubs/*.c)
STUB_OBJS		:= $(patsubst stubs/%.c,$(BUILD_DIR)/stubs/%.o,$(STUB_SRCS))

LIBC_SRCS		:= $(wildcard $(LIBC_DIR)/stdio/*.c)
LIBC_SRCS		+= $(wildcard $(LIBC_DIR)/string/*.c)
# LIBC_SRCS		:= $(shell find $(LIBC_DIR) -name '*.c' -not -path '*/testing/*')
LIBC_OBJS		:= $(patsubst $(LIBC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIBC_SRCS))

TEST_SRCS		:= test_stdio.c test_string.c
TEST_BINS		:= $(patsubst %.c,$(BUILD_DIR)/%,$(TEST_SRCS))

CXX				:= gcc

CXXFLAGS		:= -Wall -Wextra -Werror -ffreestanding -fno-builtin -nostdinc \
				   -I$(LIBC_INC) -O2


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

$(BUILD_DIR)/%: %.c $(STUB_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CXXFLAGS) $< $(STUB_OBJS) -o $@

$(BUILD_DIR)/%.o: $(LIBC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(KFS_CFLAGS) -c $< -o $@

$(BUILD_DIR)/stubs/%.o: stubs/%.c
	@mkdir -p $(dir $@)
	$(CCX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)