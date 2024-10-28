CC = gcc
NASM = nasm
NASMFLAGS=-felf32
QEMU = qemu-system-x86_64

SRC_DIR = src
BOOT_DIR = boot
INC_DIR = include
OBJ_DIR = obj

BOOT_OBJ = $(OBJ_DIR)/boot.o
KERNEL_OBJ = $(OBJ_DIR)/kernel.o
BIN_FILE = os.bin

all: $(BIN_FILE)

run: $(BIN_FILE)
	$(QEMU) -hda $(BIN_FILE)

debug:
	$(QEMU) -fda $(BIN_FILE) -gdb tcp::9999 -S

$(BIN_FILE): $(BOOT_OBJ)
	cp $(BOOT_OBJ) $(BIN_FILE)

$(BOOT_OBJ): $(BOOT_DIR)/boot.asm
	$(NASM) $(NASMFLAGS) $< -f bin -I 'boot/' -o $(BOOT_OBJ)



clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_FILE)
