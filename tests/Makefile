# tests/Makefile

LIBC_DIR		:= ../libc
LIBC_INC		:= $(LIBC_DIR)/include
BUILD_DIR		:= objs

STUB_SRCS		:= $(wildcard stubs/*.c)
STUB_OBJS		:= $(patsubst stubs/%.c,$(BUILD_DIR)/stubs/%.o,$(STUB_SRCS))

LIBC_SRCS		:= $(wildcard $(LIBC_DIR)/stdio/*.c) \
				   $(wildcard $(LIBC_DIR)/string/*.c) \
				   # $(wildcard $(LIBC_DIR)/stdlib/*.c) \
				   # $(wildcard $(LIBC_DIR)/unistd/*.c)

LIBC_OBJS		:= $(patsubst $(LIBC_DIR)/%.c,$(BUILD_DIR)/libc/%.o,$(LIBC_SRCS))

TEST_SRCS		:= test_stdio.c
TEST_BINS		:= $(patsubst %.c,$(BUILD_DIR)/%,$(TEST_SRCS))

CXX				:= gcc

CXXFLAGS		:= -Wall -Wextra \
				   -fno-builtin \
				   -I. -Iinclude -I$(LIBC_INC) -g3


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

$(BUILD_DIR)/libc/%.o: $(LIBC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/stubs/%.o: stubs/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%: %.c $(STUB_OBJS) $(LIBC_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)	