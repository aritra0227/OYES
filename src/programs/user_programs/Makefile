CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		 -nostartfiles -nodefaultlibs #-Wall -Wextra -Werror \
		 -Wno-unused-function -c
AS = nasm
ASFLAGS = -f elf
LD = ld
LDFLAGS = -T link.ld -melf_i386
STDLIB = start.o user_interrupt.o

init: main.o $(STDLIB)
	$(LD) $(LDFLAGS) main.o $(STDLIB) -o main 

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf main *.o