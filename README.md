# 6ix_os

6ix_os is a personal operating system project built from scratch. The goal is to create a small, learning-oriented kernel with basic boot, memory, interrupt, and keyboard support.

## What it does for now

- Boots in 32-bit protected mode
- Displays text on the screen through a simple VGA driver
- Sets up an Interrupt Descriptor Table (IDT)
- Handles basic hardware interrupts and timers
- Supports a minimal keyboard input loop
- Includes a simple command shell with commands such as `help`, `clear`, `about`, and `echo`

## Project structure

- `boot/` : Bootloader and startup assembly files
- `include/` : Shared headers
- `src/` : Main source code
  - `drivers/` : Device drivers such as screen and keyboard
  - `idt/` : Interrupt handling
  - `libc/` : Basic library-style helpers
  - `timer/` : System timer support
  - `shell/` : Simple command-line shell

## Requirements

- i386 cross-compiler for GCC
- NASM
- QEMU (optional, for emulation)

## Build

Run the following command from the project root:

```bash
make
```

## Run

You can launch the OS in QEMU with:

```bash
make run
```

## Current status

This project is still in active development. The current focus is on building a stable boot process, keyboard input, basic screen output, and a small interactive shell.