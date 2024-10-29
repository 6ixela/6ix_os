CC=gcc
NASM=nasm

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

KERNEL_SRC=$(wildcard src/*.c)
OBJ=$(KERNEL_SRC:.c=.o)

BIN_FILE=os.image

all: init $(BOOT_BIN) $(KERNEL_BIN) $(BIN_FILE)

run: $(BIN_FILE)
	$(QEMU) -hda $(BIN_FILE)

debug:
	$(QEMU) -fda $(BIN_FILE) -gdb tcp::9999 -S

$(BIN_FILE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

$(BOOT_BIN): $(BOOT_DIR)/boot.asm
	$(NASM) $(NASMFLAGS) $< -f bin -I 'boot/' -o $(BOOT_BIN)


$(KERNEL_BIN): src/kernel_entry.o src/kernel.o
	ld -o $@ -Ttext 0x1000 $^ --oformat binary

%kernel_entry.o: src/kernel_entry.asm
	$(NASM) $@ -f elf -o src/kernel_entry.o



%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

init:
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -rf $(OBJ_DIR)/*.o $(BIN_FILE)
	$(RM) -rf $(OBJ) src/kernel_entry.o
