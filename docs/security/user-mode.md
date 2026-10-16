# User-mode and memory-isolation work

The kernel currently runs entirely at privilege level 0. User mode is being
introduced incrementally so each CPU privilege and memory-protection change can
be debugged before the next one is added.

## First milestone: diagnosable page faults

The interrupt stubs preserve the CPU's page-fault error code, and the exception
handler reports the faulting address from `CR2`, the error-code bits, and
whether the access was a read or write from supervisor or user mode. IDT gates
keep the descriptor privilege level specified by their caller; they no longer
all become callable from ring 3. Interrupt enablement remains the responsibility
of kernel initialization, and interrupt return restores the saved flags.

Run the regression test with:

```sh
make test
```

The test builds a separate kernel image, enables a temporary 4 MiB identity
mapping, then writes to the first unmapped address. QEMU must report a
supervisor write to a non-present page at `0x00400000`. The test image exits via
QEMU's test-only debug-exit device; the normal kernel image does not enable
paging or deliberately fault.

## Second milestone: kernel GDT, TSS, and entry stack

At kernel startup, the kernel replaces the bootloader's temporary GDT with its
own flat GDT. The existing kernel selectors remain `0x08` (code) and `0x10`
(data); ring-3 code and data descriptors are reserved at `0x1B` and `0x23`.
The GDT also has a ring-0 TSS descriptor at selector `0x28`.

The TSS is initialized with `SS0 = 0x10` and the top of a dedicated, aligned
8 KiB kernel stack in `ESP0`. `tss_set_kernel_stack` lets a future scheduler
replace that stack top before running a different user task. The I/O bitmap is
placed beyond the TSS limit, so it grants no direct user-mode I/O permissions.

Run the TSS and page-fault checks with:

```sh
make test
```

The TSS test runs `LTR`, verifies that the processor reports selector `0x28`
as its task register, checks that the descriptor became a busy 32-bit TSS, and
validates the kernel/user segment descriptors and initial stack fields. The
page-fault regression still runs afterward.

The 8 KiB stack is currently shared by the kernel because the OS has no task
scheduler yet; it is not a per-process isolation boundary.

## Third milestone: enter ring 3 and verify the kernel stack switch

The test kernel builds a page directory and one page table mapping the first
4 MiB. Most pages are supervisor-only. It marks one dedicated code page
read-only/user-accessible and maps a separate user stack page at
`0x003FF000` as user-accessible and writable. It then enables paging and enters
ring 3 with an `IRET` frame using `CS=0x1B`, `SS=0x23`, and interrupts disabled.

The test program executes `INT 0x80` through a DPL-3 gate. The CPU must use
the TSS `SS0:ESP0` pair to enter ring 0.
The handler checks the saved user selectors and stack pointer, and verifies
that both the interrupt frame and its own C stack frame are within the
dedicated kernel stack. It reports to QEMU's debug console and exits with a
test status.

Run all five regression tests with:

```sh
make test
```

The ring-3 tests prove that a controlled software interrupt transfers execution
onto the TSS kernel stack and that the processor rejects both a user read
(page-fault error code `0x05`) and a user write (`0x07`) to a supervisor-only
kernel page. Each test checks the faulting address and confirms the exception
frame is on the TSS kernel stack. The protection test resumes after the denied
read only to exercise the write-denial case.

## Fourth milestone: restricted system-call ABI and safe return

Vector `0x80` is a 32-bit interrupt gate with DPL 3. The dispatcher accepts it
only when the saved code and stack selectors are the expected ring-3 selectors,
IOPL is zero, and the saved instruction and stack addresses are below the
user-space ceiling. The initial ABI deliberately accepts no user pointers:

| Register | Meaning |
|---|---|
| `EAX` | System-call number |
| `EBX` | First argument |
| `EAX` on return | Zero on success, negative error on failure |

`SYS_WRITE_CHAR` (number 1) writes the low-byte character from `EBX`. Values
above `0xFF` return `-EINVAL` (`-22`); unknown call numbers return `-ENOSYS`
(`-38`). Avoiding pointer arguments means this first call cannot be tricked into
reading or writing an unchecked user address. Any future buffer-oriented call
must validate the entire user range and its page permissions before dereferencing.

The syscall test executes two successful character calls, checks the invalid
argument and unknown-call return values, then reaches `UD2` only after each
`INT 0x80` has returned to ring 3. Its exception handler verifies the saved
user selectors and that the exception frame and handler stack are on the TSS
kernel stack. Run it, along with the prior checks, using `make test`.

This is a minimal syscall ABI, not a process manager or complete security
boundary. There is no scheduler, per-process kernel stack, or general-purpose
user pointer validator yet. Hardware interrupts remain disabled during the
ring-3 tests. The running shell still executes in ring 0; only the dedicated
test image enters ring 3.

## Difficulty

This is a medium-to-hard low-level kernel task: a wrong interrupt frame or page
permission can prevent the kernel from diagnosing its own fault. QEMU and the
test-only fault scenario make the first boundary repeatable. True isolation is
harder and will only be claimed after ring-3 code can use permitted pages and is
reliably denied access to supervisor pages. The QEMU ring-3 tests provide
targeted evidence for privilege transitions and memory permissions, not a
complete system security claim.
