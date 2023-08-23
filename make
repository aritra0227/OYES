rm -rf *.o kernel.elf os.iso
nasm -f elf loader.s -o loader.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  kmain.c -o kmain.o
nasm -f elf io.s -o io.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  stdio.c -o stdio.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  utilities.c -o utilities.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  memory_seg.c -o memory_seg.o
nasm -f elf gdt.s -o gdt.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  idt.c -o idt.o
nasm -f elf load_idt.s -o load_idt.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  interrupt_handler.c -o interrupt_handler.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  exceptions.c -o exceptions.o
nasm -f elf system_interrupt_handler.s -o system_interrupt_handler.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  pic.c -o pic.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  keyboard.c -o keyboard.o
nasm -f elf pagefault_exception.s -o pagefault_exception.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  page_frame_alloc.c -o page_frame_alloc.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  paging.c -o paging.o
nasm -f elf paging_asm.s -o paging_asm.o
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c  malloc.c -o malloc.o
nasm -f elf user_mode.s -o user_mode.o
nasm -f elf user_interrupt.s -o user_interrupt.o
nasm -f elf tss.s -o tss.o
nasm -f elf user_entry.s -o user_entry.o
ld -T link.ld -melf_i386 loader.o kmain.o io.o stdio.o utilities.o memory_seg.o gdt.o idt.o load_idt.o interrupt_handler.o exceptions.o system_interrupt_handler.o pic.o keyboard.o pagefault_exception.o page_frame_alloc.o paging.o paging_asm.o malloc.o user_mode.o user_interrupt.o tss.o user_entry.o -o kernel.elf
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
bochs -f bochsrc.txt -q
========================================================================
                        Bochs x86 Emulator 2.6
            Built from SVN snapshot on September 2nd, 2012
========================================================================
Next at t=0
(0) [0x00000000fffffff0] f000:fff0 (unk. ctxt): jmp far f000:e05b         ; ea5be000f0
<bochs:1> <bochs:2> <bochs:3> <bochs:4> <bochs:5> <bochs:6> (0).[0] [0x00000000fffffff0] f000:fff0 (unk. ctxt): jmp far f000:e05b         ; ea5be000f0
