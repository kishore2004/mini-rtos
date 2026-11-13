.RECIPEPREFIX = >

CC	= arm-none-eabi-gcc
CFLAGS	= -mcpu=cortex-m3 -mthumb -O0 -g -Wall -ffreestanding -nostdlib
SRCS	= startup.c main.c context_switch.s

rtos.elf: $(SRCS) linker.ld
> $(CC) $(CFLAGS) -T linker.ld $(SRCS) -o rtos.elf

run: rtos.elf
> qemu-system-arm -machine mps2-an385 -cpu cortex-m3 -nographic -kernel rtos.elf

clean: 
> rm -f rtos.elf