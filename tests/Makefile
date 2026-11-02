TARGET ?= i386-elf
KERNEL_CC ?= $(TARGET)-gcc

CPPFLAGS := -I../libc/include -Iframework
CFLAGS := -m32 -std=gnu23 -Wall -Wextra -Werror -fno-builtin
BUILD_DIR := build
KERNEL_BUILD_DIR := $(BUILD_DIR)/kernel-test
KERNEL_BINARY := $(KERNEL_BUILD_DIR)/kernel
KERNEL_ISO := $(KERNEL_BUILD_DIR)/kfs-test.iso
KERNEL_LOG := $(KERNEL_BUILD_DIR)/serial.log
KERNEL_SUPPORT_OBJECT := $(KERNEL_BUILD_DIR)/test_support.o
GDT_BUILD_DIR := $(BUILD_DIR)/gdt-test
GDT_BINARY := $(GDT_BUILD_DIR)/kernel
GDT_ISO := $(GDT_BUILD_DIR)/kfs-test.iso
GDT_LOG := $(GDT_BUILD_DIR)/serial.log
GDT_SUPPORT_OBJECT := $(GDT_BUILD_DIR)/test_support.o
MULTIBOOT_BUILD_DIR := $(BUILD_DIR)/multiboot-test
MULTIBOOT_BINARY := $(MULTIBOOT_BUILD_DIR)/kernel
MULTIBOOT_ISO := $(MULTIBOOT_BUILD_DIR)/kfs-test.iso
MULTIBOOT_LOG := $(MULTIBOOT_BUILD_DIR)/serial.log
MULTIBOOT_SUPPORT_OBJECT := $(MULTIBOOT_BUILD_DIR)/test_support.o
FPU_BUILD_DIR := $(BUILD_DIR)/fpu-test
FPU_BINARY := $(FPU_BUILD_DIR)/kernel
FPU_ISO := $(FPU_BUILD_DIR)/kfs-test.iso
FPU_LOG := $(FPU_BUILD_DIR)/serial.log
FPU_SUPPORT_OBJECT := $(FPU_BUILD_DIR)/test_support.o
STACK_BUILD_DIR := $(BUILD_DIR)/stack-test
STACK_BINARY := $(STACK_BUILD_DIR)/kernel
STACK_ISO := $(STACK_BUILD_DIR)/kfs-test.iso
STACK_LOG := $(STACK_BUILD_DIR)/serial.log
STACK_SUPPORT_OBJECT := $(STACK_BUILD_DIR)/test_support.o
LAYOUT_BUILD_DIR := $(BUILD_DIR)/layout-test
LAYOUT_BINARY := $(LAYOUT_BUILD_DIR)/kernel
LAYOUT_ISO := $(LAYOUT_BUILD_DIR)/kfs-test.iso
LAYOUT_LOG := $(LAYOUT_BUILD_DIR)/serial.log
LAYOUT_SUPPORT_OBJECT := $(LAYOUT_BUILD_DIR)/test_support.o
VGA_BUILD_DIR := $(BUILD_DIR)/vga-test
VGA_BINARY := $(VGA_BUILD_DIR)/kernel
VGA_ISO := $(VGA_BUILD_DIR)/kfs-test.iso
VGA_LOG := $(VGA_BUILD_DIR)/serial.log
VGA_SUPPORT_OBJECT := $(VGA_BUILD_DIR)/test_support.o
TTY_BUILD_DIR := $(BUILD_DIR)/tty-test
TTY_BINARY := $(TTY_BUILD_DIR)/kernel
TTY_ISO := $(TTY_BUILD_DIR)/kfs-test.iso
TTY_LOG := $(TTY_BUILD_DIR)/serial.log
TTY_SUPPORT_OBJECT := $(TTY_BUILD_DIR)/test_support.o
FRAMEBUFFER_BUILD_DIR := $(BUILD_DIR)/framebuffer-test
FRAMEBUFFER_BINARY := $(FRAMEBUFFER_BUILD_DIR)/kernel
FRAMEBUFFER_ISO := $(FRAMEBUFFER_BUILD_DIR)/kfs-test.iso
FRAMEBUFFER_LOG := $(FRAMEBUFFER_BUILD_DIR)/serial.log
FRAMEBUFFER_SUPPORT_OBJECT := $(FRAMEBUFFER_BUILD_DIR)/test_support.o
VFS_BUILD_DIR := $(BUILD_DIR)/vfs-test
VFS_BINARY := $(VFS_BUILD_DIR)/kernel
VFS_ISO := $(VFS_BUILD_DIR)/kfs-test.iso
VFS_LOG := $(VFS_BUILD_DIR)/serial.log
VFS_SUPPORT_OBJECT := $(VFS_BUILD_DIR)/test_support.o
SYS_WRITE_BUILD_DIR := $(BUILD_DIR)/sys-write-test
SYS_WRITE_BINARY := $(SYS_WRITE_BUILD_DIR)/kernel
SYS_WRITE_ISO := $(SYS_WRITE_BUILD_DIR)/kfs-test.iso
SYS_WRITE_LOG := $(SYS_WRITE_BUILD_DIR)/serial.log
SYS_WRITE_SUPPORT_OBJECT := $(SYS_WRITE_BUILD_DIR)/test_support.o
KERNEL_CPPFLAGS := -I../libc/include -I../kernel/include -Ikernel
KERNEL_CFLAGS := -std=gnu23 -ffreestanding -fno-builtin \
	-fno-stack-protector -nostdlib -nodefaultlibs -Wall -Wextra -Werror -O2
KERNEL_ASMFLAGS := -f elf32 -I..

ifeq ($(KERNEL_CC),clang)
KERNEL_CFLAGS += --target=i686-elf
endif

STRING_SOURCES := $(wildcard ../libc/string/*.c)
STRING_OBJECTS := $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(STRING_SOURCES))
CTYPE_SOURCES := $(wildcard ../libc/ctype/*.c)
CTYPE_OBJECTS := $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(CTYPE_SOURCES))
STDLIB_SOURCES := ../libc/stdlib/labs.c
STDLIB_OBJECTS := $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(STDLIB_SOURCES))
STDIO_SOURCES := $(wildcard ../libc/stdio/*.c)
STDIO_OBJECTS := $(patsubst ../libc/%.c,$(BUILD_DIR)/libc/%.o,$(STDIO_SOURCES))
UNISTD_OBJECT := $(BUILD_DIR)/libc/unistd/write.o
FRAMEWORK_OBJECT := $(BUILD_DIR)/framework/test.o
TEST_OBJECTS := $(patsubst host/%.c,$(BUILD_DIR)/host/%.o,$(wildcard host/*.c))
TEST_BINARY := $(BUILD_DIR)/test-libc

.PHONY: all libc kernel test-kernel test-qemu test-gdt test-multiboot \
		test-fpu test-stack test-layout test-vga test-tty test-framebuffer \
		test-vfs test-sys-write clean

all: libc

libc: $(TEST_BINARY)
	@$(TEST_BINARY)

kernel: $(KERNEL_BINARY)

test-kernel: test-qemu test-gdt test-multiboot test-fpu test-stack test-layout \
	test-vga test-tty test-framebuffer test-vfs test-sys-write

test-qemu: $(KERNEL_ISO)
	@qemu/run.sh $(KERNEL_ISO) $(KERNEL_LOG)

test-gdt: $(GDT_ISO)
	@qemu/run.sh $(GDT_ISO) $(GDT_LOG)

test-multiboot: $(MULTIBOOT_ISO)
	@qemu/run.sh $(MULTIBOOT_ISO) $(MULTIBOOT_LOG)

test-fpu: $(FPU_ISO)
	@qemu/run.sh $(FPU_ISO) $(FPU_LOG)

test-stack: $(STACK_ISO)
	@qemu/run.sh $(STACK_ISO) $(STACK_LOG)

test-layout: $(LAYOUT_ISO)
	@qemu/run.sh $(LAYOUT_ISO) $(LAYOUT_LOG)

test-vga: $(VGA_ISO)
	@qemu/run.sh $(VGA_ISO) $(VGA_LOG)

test-tty: $(TTY_ISO)
	@qemu/run.sh $(TTY_ISO) $(TTY_LOG)

test-framebuffer: $(FRAMEBUFFER_ISO)
	@qemu/run.sh $(FRAMEBUFFER_ISO) $(FRAMEBUFFER_LOG)

test-vfs: $(VFS_ISO)
	@qemu/run.sh $(VFS_ISO) $(VFS_LOG)

test-sys-write: $(SYS_WRITE_ISO)
	@qemu/run.sh $(SYS_WRITE_ISO) $(SYS_WRITE_LOG)

$(KERNEL_BINARY): $(KERNEL_BUILD_DIR)/bootloader.o \
	$(KERNEL_BUILD_DIR)/fpu.o $(KERNEL_SUPPORT_OBJECT) \
	$(KERNEL_BUILD_DIR)/test_kernel.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(KERNEL_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(KERNEL_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(KERNEL_BUILD_DIR)/test_kernel.o: kernel/test_kernel.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(KERNEL_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(GDT_BINARY): $(GDT_BUILD_DIR)/bootloader.o $(GDT_BUILD_DIR)/fpu.o \
	$(GDT_BUILD_DIR)/gdt.o $(GDT_BUILD_DIR)/gdt.s.o \
	$(GDT_SUPPORT_OBJECT) $(GDT_BUILD_DIR)/test_gdt.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(GDT_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(GDT_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(GDT_BUILD_DIR)/gdt.o: ../kernel/arch/i386/gdt.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(GDT_BUILD_DIR)/gdt.s.o: ../kernel/arch/i386/asm/gdt.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(GDT_BUILD_DIR)/test_gdt.o: kernel/test_gdt.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(GDT_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(MULTIBOOT_BINARY): $(MULTIBOOT_BUILD_DIR)/bootloader.o \
	$(MULTIBOOT_BUILD_DIR)/fpu.o $(MULTIBOOT_SUPPORT_OBJECT) \
	$(MULTIBOOT_BUILD_DIR)/test_multiboot.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(MULTIBOOT_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(MULTIBOOT_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(MULTIBOOT_BUILD_DIR)/test_multiboot.o: kernel/test_multiboot.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(MULTIBOOT_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FPU_BINARY): $(FPU_BUILD_DIR)/bootloader.o \
	$(FPU_BUILD_DIR)/fpu.o $(FPU_SUPPORT_OBJECT) \
	$(FPU_BUILD_DIR)/test_fpu.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(FPU_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(FPU_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(FPU_BUILD_DIR)/test_fpu.o: kernel/test_fpu.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FPU_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(STACK_BINARY): $(STACK_BUILD_DIR)/bootloader.o \
	$(STACK_BUILD_DIR)/fpu.o $(STACK_SUPPORT_OBJECT) \
	$(STACK_BUILD_DIR)/test_stack.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(STACK_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(STACK_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(STACK_BUILD_DIR)/test_stack.o: kernel/test_stack.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(STACK_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(LAYOUT_BINARY): $(LAYOUT_BUILD_DIR)/bootloader.o \
	$(LAYOUT_BUILD_DIR)/fpu.o $(LAYOUT_SUPPORT_OBJECT) \
	$(LAYOUT_BUILD_DIR)/test_layout.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(LAYOUT_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(LAYOUT_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(LAYOUT_BUILD_DIR)/test_layout.o: kernel/test_layout.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(LAYOUT_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VGA_BINARY): $(VGA_BUILD_DIR)/bootloader.o \
	$(VGA_BUILD_DIR)/fpu.o $(VGA_BUILD_DIR)/vga.o \
	$(VGA_SUPPORT_OBJECT) $(VGA_BUILD_DIR)/test_vga.o \
	$(VGA_BUILD_DIR)/memmove.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(VGA_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(VGA_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(VGA_BUILD_DIR)/vga.o: ../kernel/drivers/vga.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VGA_BUILD_DIR)/test_vga.o: kernel/test_vga.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VGA_BUILD_DIR)/memmove.o: ../libc/string/memmove.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VGA_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(TTY_BINARY): $(TTY_BUILD_DIR)/bootloader.o $(TTY_BUILD_DIR)/fpu.o \
	$(TTY_SUPPORT_OBJECT) $(TTY_BUILD_DIR)/tty.o \
	$(TTY_BUILD_DIR)/test_tty.o $(TTY_BUILD_DIR)/memmove.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(TTY_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(TTY_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(TTY_BUILD_DIR)/test_tty.o: kernel/test_tty.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(TTY_BUILD_DIR)/tty.o: ../kernel/core/tty.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(TTY_BUILD_DIR)/memmove.o: ../libc/string/memmove.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(TTY_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FRAMEBUFFER_BINARY): $(FRAMEBUFFER_BUILD_DIR)/bootloader.o \
	$(FRAMEBUFFER_BUILD_DIR)/fpu.o $(FRAMEBUFFER_BUILD_DIR)/framebuffer.o \
	$(FRAMEBUFFER_SUPPORT_OBJECT) $(FRAMEBUFFER_BUILD_DIR)/test_framebuffer.o \
	$(FRAMEBUFFER_BUILD_DIR)/memcpy.o $(FRAMEBUFFER_BUILD_DIR)/memmove.o \
	$(FRAMEBUFFER_BUILD_DIR)/memset.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(FRAMEBUFFER_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(FRAMEBUFFER_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(FRAMEBUFFER_BUILD_DIR)/framebuffer.o: ../kernel/drivers/framebuffer.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FRAMEBUFFER_BUILD_DIR)/test_framebuffer.o: kernel/test_framebuffer.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FRAMEBUFFER_BUILD_DIR)/memcpy.o: ../libc/string/memcpy.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FRAMEBUFFER_BUILD_DIR)/memmove.o: ../libc/string/memmove.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FRAMEBUFFER_BUILD_DIR)/memset.o: ../libc/string/memset.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(FRAMEBUFFER_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VFS_BINARY): $(VFS_BUILD_DIR)/bootloader.o $(VFS_BUILD_DIR)/fpu.o \
	$(VFS_SUPPORT_OBJECT) $(VFS_BUILD_DIR)/vfs.o $(VFS_BUILD_DIR)/test_vfs.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(VFS_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(VFS_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(VFS_BUILD_DIR)/vfs.o: ../kernel/fs/vfs.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VFS_BUILD_DIR)/test_vfs.o: kernel/test_vfs.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(VFS_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(SYS_WRITE_BINARY): $(SYS_WRITE_BUILD_DIR)/bootloader.o \
	$(SYS_WRITE_BUILD_DIR)/fpu.o $(SYS_WRITE_SUPPORT_OBJECT) \
	$(SYS_WRITE_BUILD_DIR)/vfs.o $(SYS_WRITE_BUILD_DIR)/write.o \
	$(SYS_WRITE_BUILD_DIR)/test_sys_write.o
	@mkdir -p $(dir $@)
	ld.lld -m elf_i386 -T ../kernel/arch/i386/linker.ld $^ -o $@

$(SYS_WRITE_BUILD_DIR)/bootloader.o: ../kernel/arch/i386/asm/bootloader.s
	@mkdir -p $(dir $@)
	nasm $(KERNEL_ASMFLAGS) $< -o $@

$(SYS_WRITE_BUILD_DIR)/fpu.o: ../kernel/arch/i386/asm/fpu.s
	@mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(SYS_WRITE_BUILD_DIR)/vfs.o: ../kernel/fs/vfs.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(SYS_WRITE_BUILD_DIR)/write.o: ../kernel/syscall/write.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(SYS_WRITE_BUILD_DIR)/test_sys_write.o: kernel/test_sys_write.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(SYS_WRITE_SUPPORT_OBJECT): kernel/test_support.c
	@mkdir -p $(dir $@)
	$(KERNEL_CC) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS) -c $< -o $@

$(KERNEL_ISO): $(KERNEL_BINARY) kernel/grub.cfg
	@mkdir -p $(KERNEL_BUILD_DIR)/iso/boot/grub
	cp $(KERNEL_BINARY) $(KERNEL_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(KERNEL_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(KERNEL_BUILD_DIR)/iso

$(GDT_ISO): $(GDT_BINARY) kernel/grub.cfg
	@mkdir -p $(GDT_BUILD_DIR)/iso/boot/grub
	cp $(GDT_BINARY) $(GDT_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(GDT_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(GDT_BUILD_DIR)/iso

$(MULTIBOOT_ISO): $(MULTIBOOT_BINARY) kernel/grub.cfg
	@mkdir -p $(MULTIBOOT_BUILD_DIR)/iso/boot/grub
	cp $(MULTIBOOT_BINARY) $(MULTIBOOT_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(MULTIBOOT_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(MULTIBOOT_BUILD_DIR)/iso

$(FPU_ISO): $(FPU_BINARY) kernel/grub.cfg
	@mkdir -p $(FPU_BUILD_DIR)/iso/boot/grub
	cp $(FPU_BINARY) $(FPU_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(FPU_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(FPU_BUILD_DIR)/iso

$(STACK_ISO): $(STACK_BINARY) kernel/grub.cfg
	@mkdir -p $(STACK_BUILD_DIR)/iso/boot/grub
	cp $(STACK_BINARY) $(STACK_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(STACK_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(STACK_BUILD_DIR)/iso

$(LAYOUT_ISO): $(LAYOUT_BINARY) kernel/grub.cfg
	@mkdir -p $(LAYOUT_BUILD_DIR)/iso/boot/grub
	cp $(LAYOUT_BINARY) $(LAYOUT_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(LAYOUT_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(LAYOUT_BUILD_DIR)/iso

$(VGA_ISO): $(VGA_BINARY) kernel/grub.cfg
	@mkdir -p $(VGA_BUILD_DIR)/iso/boot/grub
	cp $(VGA_BINARY) $(VGA_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(VGA_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(VGA_BUILD_DIR)/iso

$(TTY_ISO): $(TTY_BINARY) kernel/grub.cfg
	@mkdir -p $(TTY_BUILD_DIR)/iso/boot/grub
	cp $(TTY_BINARY) $(TTY_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(TTY_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(TTY_BUILD_DIR)/iso

$(FRAMEBUFFER_ISO): $(FRAMEBUFFER_BINARY) kernel/grub.cfg
	@mkdir -p $(FRAMEBUFFER_BUILD_DIR)/iso/boot/grub
	cp $(FRAMEBUFFER_BINARY) $(FRAMEBUFFER_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(FRAMEBUFFER_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(FRAMEBUFFER_BUILD_DIR)/iso

$(VFS_ISO): $(VFS_BINARY) kernel/grub.cfg
	@mkdir -p $(VFS_BUILD_DIR)/iso/boot/grub
	cp $(VFS_BINARY) $(VFS_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(VFS_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(VFS_BUILD_DIR)/iso

$(SYS_WRITE_ISO): $(SYS_WRITE_BINARY) kernel/grub.cfg
	@mkdir -p $(SYS_WRITE_BUILD_DIR)/iso/boot/grub
	cp $(SYS_WRITE_BINARY) $(SYS_WRITE_BUILD_DIR)/iso/boot/kernel
	cp kernel/grub.cfg $(SYS_WRITE_BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(SYS_WRITE_BUILD_DIR)/iso

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

clean:
	rm -rf $(BUILD_DIR)
