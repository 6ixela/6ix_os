CC = gcc
NASM = nasm
QEMU = qemu-system-x86_64

SRC_DIR = src
BOOT_DIR = boot
INC_DIR = include
OBJ_DIR = obj

BOOT_OBJ = $(OBJ_DIR)/boot.o
KERNEL_OBJ = $(OBJ_DIR)/kernel.o
BIN_FILE = os.bin

all: $(BIN_FILE)

$(BIN_FILE): $(BOOT_OBJ)
	cp $(BOOT_OBJ) $(BIN_FILE)

$(BOOT_OBJ): $(BOOT_DIR)/boot.asm
	$(NASM) -f bin -I 'boot/' $(BOOT_DIR)/boot.asm -o $(BOOT_OBJ)

run: $(BIN_FILE)
	$(QEMU) -drive format=raw,file=$(BIN_FILE) -m 512

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_FILE)
