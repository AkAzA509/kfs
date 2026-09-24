# tests/Makefile

LIBC_DIR		:= ../libc
LIBC_INC		:= $(LIBC_DIR)/include
BUILD_DIR		:= objs

STUB_SRCS		:= $(wildcard stubs/*.c)
STUB_OBJS		:= $(patsubst stubs/%.c,$(BUILD_DIR)/stubs/%.o,$(filter stubs/%.c,$(STUB_SRCS)))

LIBC_SRCS		:= $(wildcard $(LIBC_DIR)/stdio/*.c) \
				   $(wildcard $(LIBC_DIR)/string/*.c) \
				   $(wildcard $(LIBC_DIR)/stdlib/*.c) \
				   $(wildcard $(LIBC_DIR)/ctype/*.c) \
				   # $(wildcard $(LIBC_DIR)/unistd/*.c)

LIBC_OBJS		:= $(patsubst $(LIBC_DIR)/%.c,$(BUILD_DIR)/libc/%.o,$(LIBC_SRCS))

TEST_SRCS		:= tester.c test_ctype.c test_string.c test_stdlib.c test_stdio.c common.c
TEST_BIN		:= $(BUILD_DIR)/tester

CXX				:= gcc
CXXFLAGS		:= -std=gnu23 -Wall -Wextra -fno-builtin -I. -Iinclude -I$(LIBC_INC) -g3 -m32


# --- Rules ---
.PHONY: all test clean

all: test

test: $(TEST_BIN)
	@echo "=================================================="
	@echo " Starting KFS's unit testing suit..."
	@echo "=================================================="
	@$(TEST_BIN)

$(BUILD_DIR)/libc/%.o: $(LIBC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/stubs/%.o: stubs/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_SRCS) $(STUB_OBJS) $(LIBC_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) -r $(BUILD_DIR)