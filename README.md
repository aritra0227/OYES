## OYES

# Description: 
OYES is a lightweight x86_32 bit operating system I designed intially to provide me hands-on learning and experimentation with low-level systems programming. Currently OYES supports the running basic usermode programs.


# Architecture Overview:
OYES is x86_32 bit Unix-based operating system that leverages the power of both C and x86 assembly languages. Currently, OYES runs within the Bochs emulator environment enabling ease of access and testing.
- Bootloader: Grub
  OYES relies on the Grub bootloader, ensuring a reliable and efficient startup sequence. Initially I wasn't very interested in writing my own bootloader, but I **may** spend some time later on implementing my own.
- Modes:
  Currently OYES supports 32 bit protected mode and usermode. Although as of right now, only simple usermode programs can be run in assembly. In order to run more robust programs (most preferable in C), I should implement a file system first to provide a proper abstraction interface to load the usermode code into physical memory.
  
- Interrupts:
  The Idt and Pic are properly installed and hence OYES supports both hardware and software interrupts and the respective interrupt handling through defining proper interrupt handler.
  - Currently, there's are modules on the serial port and keyboard to read and write inputs. (I will probably write a handler and install the clock once I get to the point of writing a scheduler.)

- Display Output:
  Currently contains a printf implementation which currently writes to the frame buffer. A future refactor should include a better use of a serial port to dump logs and error outputs and using the framebuffer for simply displaying a shell program.

- Memory: 
  - Physical Memory:
    - OYES employs an innovative strategy for physical memory management, characterized by a sudo bitmap and stack implementation. This approach optimally handles memory allocation, allowing dynamic utilization while maintaining performance efficiency.
  - Virtual Memory:
    - Currently Virtual Memory is implemented through 2 layer PAGING utilizing 4KB page frames.
  - Heap:
    - Currently malloc implementation using a free list to query for heap memory.

Current State:
I did start writing some process management syscalls and a planning out a scheduler, however, I realize that I would only be able to run very simply programs if I were to run processes concurrently. Hence I want to first spend some time first creating a writable file system which would provide me with a proper abstraction allowing me:
  - to load robust programs on multiple processes
  - write a nice interactive shell and possibly make life easier on running programs on this shell

# TODO (in a very likely chronological order):
- Write a writable file system
- Enable running C programs
  - May also require an elf parser
- Create an interactive shell program to display the functionalities of my OS (even if it is just editing simple files)
- Finish writing process management syscalls
- Scheduler and multitasking
- ...
- ...
- Load and run a simple interactive game


# PERSONAL NOTES:
- IMPORTANT FOR BOCH EMULATOR TO SHOW UP: 
  - SSH USING -X -Y

References:
- Intel Manual
- OSdev forums and wiki
- wikipedia
- littleosbook
