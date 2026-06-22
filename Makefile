ASM = nasm
CC  = gcc
LD  = ld

SRC_DIR       = boot/src
BOOT_DIR      = boot
KERNEL_DIR    = kernel
DISPLAY_DIR   = display
CPU_DIR       = cpu
LIBC_DIR      = libc
DRIVERS_DIR   = drivers
ASSEMBLER_DIR = assembler
BUILD_DIR     = build

all: $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main_floppy.img: $(BUILD_DIR)/main.bin
	cp $(BUILD_DIR)/main.bin $(BUILD_DIR)/main_floppy.img
	truncate -s 1440k $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main.bin: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	cat $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin > $(BUILD_DIR)/main.bin

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/main.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) -I$(BOOT_DIR)/src/ -I$(BOOT_DIR)/ $(SRC_DIR)/main.asm -f bin -o $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/entry.o: $(KERNEL_DIR)/entry.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 $(KERNEL_DIR)/entry.asm -o $(BUILD_DIR)/entry.o

$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c \
$(DISPLAY_DIR)/screen.h \
$(CPU_DIR)/idt.h \
$(DRIVERS_DIR)/keyboard.h \
$(KERNEL_DIR)/shell.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(KERNEL_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

$(BUILD_DIR)/shell.o: $(KERNEL_DIR)/shell.c \
$(KERNEL_DIR)/shell.h \
$(DISPLAY_DIR)/screen.h \
$(ASSEMBLER_DIR)/assembler.h \
$(ASSEMBLER_DIR)/encoder.h \
$(ASSEMBLER_DIR)/decoder.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(KERNEL_DIR)/shell.c -o $(BUILD_DIR)/shell.o

$(BUILD_DIR)/screen.o: $(DISPLAY_DIR)/screen.c \
$(DISPLAY_DIR)/screen.h \
$(CPU_DIR)/ports.h \
$(LIBC_DIR)/memory.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(DISPLAY_DIR)/screen.c -o $(BUILD_DIR)/screen.o

$(BUILD_DIR)/ports.o: $(CPU_DIR)/ports.c $(CPU_DIR)/ports.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(CPU_DIR)/ports.c -o $(BUILD_DIR)/ports.o

$(BUILD_DIR)/idt.o: $(CPU_DIR)/idt.c $(CPU_DIR)/idt.h $(CPU_DIR)/ports.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(CPU_DIR)/idt.c -o $(BUILD_DIR)/idt.o

$(BUILD_DIR)/isr_asm.o: $(CPU_DIR)/isr.asm
	mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 $(CPU_DIR)/isr.asm -o $(BUILD_DIR)/isr_asm.o

$(BUILD_DIR)/isr.o: $(CPU_DIR)/isr.c \
$(CPU_DIR)/isr.h \
$(CPU_DIR)/ports.h \
$(DISPLAY_DIR)/screen.h \
$(DRIVERS_DIR)/keyboard.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(CPU_DIR)/isr.c -o $(BUILD_DIR)/isr.o

$(BUILD_DIR)/keyboard.o: $(DRIVERS_DIR)/keyboard.c \
$(DRIVERS_DIR)/keyboard.h \
$(CPU_DIR)/ports.h \
$(DISPLAY_DIR)/screen.h \
$(KERNEL_DIR)/shell.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(DRIVERS_DIR)/keyboard.c -o $(BUILD_DIR)/keyboard.o

$(BUILD_DIR)/memory.o: $(LIBC_DIR)/memory.c $(LIBC_DIR)/memory.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(LIBC_DIR)/memory.c -o $(BUILD_DIR)/memory.o

$(BUILD_DIR)/encoder.o: $(ASSEMBLER_DIR)/encoder.c $(ASSEMBLER_DIR)/encoder.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(ASSEMBLER_DIR)/encoder.c -o $(BUILD_DIR)/encoder.o

$(BUILD_DIR)/parser.o: $(ASSEMBLER_DIR)/parser.c $(ASSEMBLER_DIR)/parser.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(ASSEMBLER_DIR)/parser.c -o $(BUILD_DIR)/parser.o

$(BUILD_DIR)/assembler.o: $(ASSEMBLER_DIR)/assembler.c \
$(ASSEMBLER_DIR)/assembler.h \
$(ASSEMBLER_DIR)/encoder.h \
$(ASSEMBLER_DIR)/parser.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(ASSEMBLER_DIR)/assembler.c -o $(BUILD_DIR)/assembler.o

$(BUILD_DIR)/decoder.o: $(ASSEMBLER_DIR)/decoder.c \
$(ASSEMBLER_DIR)/decoder.h \
$(ASSEMBLER_DIR)/encoder.h \
$(DISPLAY_DIR)/screen.h
	mkdir -p $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -fno-pic -nostdlib -c $(ASSEMBLER_DIR)/decoder.c -o $(BUILD_DIR)/decoder.o

$(BUILD_DIR)/kernel.bin: \
$(BUILD_DIR)/entry.o \
$(BUILD_DIR)/kernel.o \
$(BUILD_DIR)/shell.o \
$(BUILD_DIR)/screen.o \
$(BUILD_DIR)/ports.o \
$(BUILD_DIR)/idt.o \
$(BUILD_DIR)/isr_asm.o \
$(BUILD_DIR)/isr.o \
$(BUILD_DIR)/keyboard.o \
$(BUILD_DIR)/memory.o \
$(BUILD_DIR)/encoder.o \
$(BUILD_DIR)/parser.o \
$(BUILD_DIR)/assembler.o \
$(BUILD_DIR)/decoder.o \
linker.ld
	$(LD) -m elf_i386 -T linker.ld --oformat binary \
	$(BUILD_DIR)/entry.o \
	$(BUILD_DIR)/kernel.o \
	$(BUILD_DIR)/shell.o \
	$(BUILD_DIR)/screen.o \
	$(BUILD_DIR)/ports.o \
	$(BUILD_DIR)/idt.o \
	$(BUILD_DIR)/isr_asm.o \
	$(BUILD_DIR)/isr.o \
	$(BUILD_DIR)/keyboard.o \
	$(BUILD_DIR)/memory.o \
	$(BUILD_DIR)/encoder.o \
	$(BUILD_DIR)/parser.o \
	$(BUILD_DIR)/assembler.o \
	$(BUILD_DIR)/decoder.o \
	-o $(BUILD_DIR)/kernel.bin

run: $(BUILD_DIR)/main_floppy.img
	qemu-system-i386 -drive file=$(BUILD_DIR)/main_floppy.img,format=raw,if=floppy -no-reboot

clean:
	rm -rf $(BUILD_DIR)
