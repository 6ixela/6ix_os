CC=i386-elf-gcc
NASM=nasm
LD=i386-elf-ld
GDB=i386-elf-gdb

CFLAGS=-m32 -ffreestanding -fno-PIC -g
NASMFLAGS=-felf32

QEMU=qemu-system-i386

BOOT_DIR=boot
OBJ_DIR=obj
TEST_DIR=$(OBJ_DIR)/security-test

BOOT_BIN=$(OBJ_DIR)/boot.bin
KERNEL_BIN=$(OBJ_DIR)/kernel.bin
BIN_FILE=os.bin

KERNEL_SRC=$(wildcard src/*.c src/*/*.c src/*/*/*.c)
ASM_SRC=$(wildcard src/*.asm src/*/*.asm src/*/*/*.asm)
OBJ=$(KERNEL_SRC:.c=.o)
OBJ_ASM=$(ASM_SRC:.asm=.o)

TEST_C_OBJ=$(TEST_DIR)/gdt/gdt.o \
	$(TEST_DIR)/idt/idt.o \
	$(TEST_DIR)/idt/isr.o \
	$(TEST_DIR)/syscall/syscall.o \
	$(TEST_DIR)/drivers/screen/io.o \
	$(TEST_DIR)/drivers/screen/screen.o \
	$(TEST_DIR)/libc/memory/memory.o \
	$(TEST_DIR)/tests/test_kernel.o
TEST_ASM_OBJ=$(TEST_DIR)/kernel_entry.o \
	$(TEST_DIR)/gdt/gdt_asm.o \
	$(TEST_DIR)/idt/idt_asm.o \
	$(TEST_DIR)/user/user_mode.o \
	$(TEST_DIR)/tests/ring3_program.o
TEST_BOOT_BIN=$(TEST_DIR)/boot.bin
TEST_KERNEL_BIN=$(TEST_DIR)/kernel.bin
TEST_IMAGE=$(TEST_DIR)/os.bin

.PHONY: all run debug test clean init

all: init $(BIN_FILE)

run: $(BIN_FILE)
	$(QEMU) -fda $(BIN_FILE)

test: $(TEST_IMAGE)
	sh tests/run-security-tests.sh "$(QEMU)" "$(TEST_IMAGE)"

debug: CFLAGS += -g
debug: $(BIN_FILE) kernel_elf
	$(QEMU) -s -S -fda $(BIN_FILE) &
	$(GDB) -ex "target remote localhost:1234" -ex "symbol-file $(OBJ_DIR)/kernel_elf"

$(BIN_FILE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

$(BOOT_BIN): $(BOOT_DIR)/boot.asm $(KERNEL_BIN)
	mkdir -p $(dir $@)
	sectors=$$((($$(wc -c < $(KERNEL_BIN)) + 511) / 512)); \
	$(NASM) $(NASMFLAGS) -D KERNEL_SECTORS=$$sectors $< -f bin -I 'boot/' -o $@

$(KERNEL_BIN): $(OBJ_ASM) $(OBJ)
	mkdir -p $(dir $@)
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

kernel_elf: $(OBJ_ASM) $(OBJ)
	$(LD) -o $(OBJ_DIR)/$@ -Ttext 0x1000 $^

%.o: %.asm
	$(NASM) $< $(NASMFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_DIR)/%.o: src/%.asm
	mkdir -p $(dir $@)
	$(NASM) $< $(NASMFLAGS) -o $@

$(TEST_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_DIR)/tests/%.o: tests/%.asm
	mkdir -p $(dir $@)
	$(NASM) $< $(NASMFLAGS) -o $@

$(TEST_DIR)/tests/%.o: tests/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_BOOT_BIN): $(BOOT_DIR)/boot.asm $(TEST_KERNEL_BIN)
	mkdir -p $(dir $@)
	sectors=$$((($$(wc -c < $(TEST_KERNEL_BIN)) + 511) / 512)); \
	$(NASM) $(NASMFLAGS) -D KERNEL_SECTORS=$$sectors $< -f bin -I 'boot/' -o $@

$(TEST_KERNEL_BIN): $(TEST_ASM_OBJ) $(TEST_C_OBJ)
	mkdir -p $(dir $@)
	$(LD) -o $@ -Ttext 0x1000 $^ --oformat binary

$(TEST_IMAGE): $(TEST_BOOT_BIN) $(TEST_KERNEL_BIN)
	cat $^ > $@

init:
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -rf $(OBJ_DIR)/*.o $(BIN_FILE)
	$(RM) -rf $(OBJ) $(OBJ_ASM)
	$(RM) -rf $(OBJ_DIR)/*
