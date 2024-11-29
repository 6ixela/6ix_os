CC=i386-elf-gcc
NASM=nasm
LD=i386-elf-ld
GDB=i386-elf-gdb

CFLAGS=-m32 -ffreestanding -fno-PIC -g
LDFLAGS=-melf_i386

NASMFLAGS=-felf32

QEMU=qemu-system-i386

SRC_DIR=src
BOOT_DIR=boot
INC_DIR=include
OBJ_DIR=obj

BOOT_BIN=$(OBJ_DIR)/boot.bin
KERNEL_BIN=$(OBJ_DIR)/kernel.bin

KERNEL_SRC=$(wildcard src/*.c src/*/*.c src/*/*/*.c)
ASM_SRC=$(wildcard src/*.asm src/*/*.asm src/*/*/*.asm)
OBJ=$(KERNEL_SRC:.c=.o)
OBJ_ASM=$(ASM_SRC:.asm=.o)

BIN_FILE=os.bin

all: init $(BOOT_BIN) $(KERNEL_BIN) $(BIN_FILE)

run: $(BIN_FILE)
	$(QEMU) -fda $(BIN_FILE)

debug: CFLAGS += -g
debug: $(BIN_FILE) kernel_elf
	$(QEMU) -s -S -fda $(BIN_FILE) &
	$(GDB) -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_ELF)"


$(BIN_FILE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

$(BOOT_BIN): $(BOOT_DIR)/boot.asm
	$(NASM) $(NASMFLAGS) $< -f bin -I 'boot/' -o $(BOOT_BIN)


$(KERNEL_BIN): $(OBJ_ASM) $(OBJ)
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

kernel_elf: $(OBJ_ASM) $(OBJ)
	$(LD) -o obj/$@ -Ttext 0x1000 $^

%.o: %.asm
	$(NASM) $^ -f elf -o $@

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@


init:
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -rf $(OBJ_DIR)/*.o $(BIN_FILE)
	$(RM) -rf $(OBJ) src/kernel_entry.o
	$(RM) -rf $(OBJ_DIR)/*
