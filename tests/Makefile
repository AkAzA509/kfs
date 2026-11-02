# tests/Makefile

TARGET					?= i386-elf
KERNEL_CC				?= $(TARGET)-gcc

CPPFLAGS				:= -I../libc/include -Iframework
CFLAGS					:= -m32 -std=gnu23 -Wall -Wextra -Werror -fno-builtin
BUILD_DIR				:= build

KERNEL_CPPFLAGS			:= -I../libc/include -I../kernel/include -Ikernel
KERNEL_CFLAGS			:= -std=gnu23 -ffreestanding -fno-builtin \
						   -fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Werror -O2
KERNEL_ASMFLAGS			:= -f elf32 -I..

ifeq ($(KERNEL_CC),clang)
	KERNEL_CFLAGS += --target=i686-elf
endif

KERNEL_TESTS			:= qemu gdt multiboot fpu stack layout vga tty framebuffer console vfs sys-write

TEST_SOURCE_qemu		:= test_kernel.c
TEST_SOURCE_gdt			:= test_gdt.c
TEST_SOURCE_multiboot	:= test_multiboot.c
TEST_SOURCE_fpu			:= test_fpu.c
TEST_SOURCE_stack		:= test_stack.c
TEST_SOURCE_layout		:= test_layout.c
TEST_SOURCE_vga			:= test_vga.c
TEST_SOURCE_tty			:= test_tty.c
TEST_SOURCE_framebuffer	:= test_framebuffer.c
TEST_SOURCE_console		:= test_console.c
TEST_SOURCE_vfs			:= test_vfs.c
TEST_SOURCE_sys-write	:= test_sys_write.c

TEST_EXTRA_qemu			:=
TEST_EXTRA_gdt			:= gdt.o gdt.s.o
TEST_EXTRA_multiboot	:=
TEST_EXTRA_fpu			:=
TEST_EXTRA_stack		:=
TEST_EXTRA_layout		:=
TEST_EXTRA_vga			:= vga.o memmove.o
TEST_EXTRA_tty			:= tty.o memmove.o
TEST_EXTRA_framebuffer	:= framebuffer.o memcpy.o memmove.o memset.o
TEST_EXTRA_console		:= console.o memmove.o memset.o labs.o
TEST_EXTRA_vfs			:= vfs.o
TEST_EXTRA_sys-write	:= vfs.o write.o

STRING_SOURCES			:= $(wildcard ../libc/string/*.c)
STRING_OBJECTS			:= $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(STRING_SOURCES))
CTYPE_SOURCES			:= $(wildcard ../libc/ctype/*.c)
CTYPE_OBJECTS			:= $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(CTYPE_SOURCES))
STDLIB_OBJECTS			:= $(BUILD_DIR)/libc/stdlib/labs.o
STDIO_SOURCES			:= $(wildcard ../libc/stdio/*.c)
STDIO_OBJECTS			:= $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(STDIO_SOURCES))
UNISTD_OBJECT			:= $(BUILD_DIR)/libc/unistd/write.o
FRAMEWORK_OBJECT		:= $(BUILD_DIR)/framework/test.o
TEST_OBJECTS			:= $(patsubst host/%.c,$(BUILD_DIR)/host/%.o,$(wildcard host/*.c))
TEST_BINARY				:= $(BUILD_DIR)/test-libc

.PHONY: all libc kernel $(addprefix test-,$(KERNEL_TESTS)) clean

all: libc kernel

libc: $(TEST_BINARY)
	@$(TEST_BINARY)

kernel: $(addprefix test-,$(KERNEL_TESTS))

define KERNEL_TEST_template
$(1)_BUILD_DIR := $(BUILD_DIR)/$(1)-test
$(1)_BINARY := $(BUILD_DIR)/$(1)-test/kernel
$(1)_ISO := $(BUILD_DIR)/$(1)-test/kfs-test.iso
$(1)_LOG := $(BUILD_DIR)/$(1)-test/serial.log
$(1)_EXTRA_OBJECTS := $(foreach object,$(TEST_EXTRA_$(1)),$(BUILD_DIR)/$(1)-test/$(object))
$(1)_OBJECTS := $(BUILD_DIR)/$(1)-test/bootloader.o \
	$(BUILD_DIR)/$(1)-test/fpu.o \
	$(BUILD_DIR)/$(1)-test/test_support.o \
	$$($(1)_EXTRA_OBJECTS) \
	$(BUILD_DIR)/$(1)-test/$(basename $(TEST_SOURCE_$(1))).o

test-$(1): $$($(1)_ISO)
	@qemu/run.sh $$($(1)_ISO) $$($(1)_LOG)

$$($(1)_BINARY): $$($(1)_OBJECTS)
	@mkdir -p $$(dir $$@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $$^ -o $$@

$$($(1)_ISO): $$($(1)_BINARY) kernel/grub.cfg
	@mkdir -p $$($(1)_BUILD_DIR)/iso/boot/grub
	cp $$< $$($(1)_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $$($(1)_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $$@ $$($(1)_BUILD_DIR)/iso

$(BUILD_DIR)/$(1)-test/$(basename $(TEST_SOURCE_$(1))).o: kernel/$(TEST_SOURCE_$(1))
	@mkdir -p $$(dir $$@)
	$$(KERNEL_CC) $$(KERNEL_CPPFLAGS) $$(KERNEL_CFLAGS) -c $$< -o $$@
endef

$(foreach test,$(KERNEL_TESTS),$(eval $(call KERNEL_TEST_template,$(test))))

$(BUILD_DIR)/%-test/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(BUILD_DIR)/%-test/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/%-test/test_support.o: kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/gdt.o: ../kernel/arch/i386/gdt.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/gdt.s.o: ../kernel/arch/i386/asm/gdt.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(BUILD_DIR)/%-test/vga.o: ../kernel/drivers/vga.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/tty.o: ../kernel/core/tty.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/framebuffer.o: ../kernel/drivers/framebuffer.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/console.o: ../kernel/drivers/console.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/vfs.o: ../kernel/fs/vfs.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/write.o: ../kernel/syscall/write.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/memcpy.o: ../libc/string/memcpy.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/memmove.o: ../libc/string/memmove.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/memset.o: ../libc/string/memset.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%-test/labs.o: ../libc/stdlib/labs.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(TEST_BINARY): $(FRAMEWORK_OBJECT) $(TEST_OBJECTS) $(STRING_OBJECTS) \
	$(CTYPE_OBJECTS) $(STDLIB_OBJECTS) $(STDIO_OBJECTS) $(UNISTD_OBJECT)
	$(CC) -m32 $^ -o $@

$(BUILD_DIR)/framework/%.o: framework/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/host/%.o: host/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/libc/%.o: ../libc/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

format:
	clang-format -i $(wildcard **/*.c)

clean:
	rm -rf $(BUILD_DIR)
