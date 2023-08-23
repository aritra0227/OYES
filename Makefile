OBJECTS = src/kernel/loader.o src/kernel/kmain.o src/utility/io.o src/stdlib/stdio.o src/utility/utilities.o src/memory/memory_seg.o src/memory/gdt.o src/interrupt/idt.o src/interrupt/load_idt.o src/interrupt/interrupt_handler.o src/interrupt/exceptions.o src/interrupt/system_interrupt_handler.o src/interrupt/pic.o src/hardware/keyboard.o src/memory/pagefault_exception.o src/memory/page_frame_alloc.o src/memory/paging.o src/memory/paging_asm.o src/memory/malloc.o src/kernel/user_mode.o src/programs/user_interrupt.o src/memory/tss.o src/programs/user_entry.o src/interrupt/syscall.o src/kernel/process.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -Iinclude
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
				-b boot/grub/stage2_eltorito    \
				-no-emul-boot                   \
				-boot-load-size 4               \
				-A os                           \
				-input-charset utf8             \
				-quiet                          \
				-boot-info-table                \
				-o os.iso                       \
				iso

run: os.iso
	bochs -f bochsrc.txt -q

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o kernel.elf os.iso
	rm -rf src/*.o
	rm -rf src/memory/*.o
	rm -rf src/hardware/*.o
	rm -rf src/interrupt/*.o
	rm -rf src/kernel/*.o
	rm -rf src/display/*.o
	rm -rf src/programs/*.o
	rm -rf src/utility/*.o
	rm -rf src/stdlib/*.o