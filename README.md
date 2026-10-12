# Lab 7 Submission

**Student:** Tejeshwar Bhagat | **Roll number:** 2401MC23

This folder contains my four xv6 solutions. I kept the layout close to Lab 6: one user program and one patch for each question, plus the output, screenshots, and write-up. The program sources are in `tests/`; the patches add them to the xv6 root. The patches are for MIT `xv6-public` commit `eeb7b415dbcb12cc362d0783e41c3d1f44066b17`.

## Files

- `tests/threadtest.c` — Q1: shared-address-space threads and an unsynchronized counter.
- `tests/synctest.c` — Q2: semaphore-protected counter and producer-consumer buffer.
- `tests/deadlock.c` — Q3: matrix detection, Banker safety check, and pipe circular-wait example.
- `tests/priotest.c` — Q4: priority scheduling and aging.
- `diffs/q1_threads.diff` through `diffs/q4_priority.diff` — question-wise kernel and user-program changes. Apply them in order.
- `output_log.txt` and `screenshots/` — the QEMU results from the runs described below.
- `Lab7_write_up.pdf` — design notes, results, and build notes.

## Build and run

The patches use the xv6-public version listed above. In a terminal, clone that repository and apply the diffs in order:

```sh
git clone https://github.com/mit-pdos/xv6-public.git xv6-public
cd xv6-public
git checkout eeb7b415dbcb12cc362d0783e41c3d1f44066b17
git apply /path/to/lab7_submission_2401MC23/diffs/q1_threads.diff
git apply /path/to/lab7_submission_2401MC23/diffs/q2_semaphores.diff
git apply /path/to/lab7_submission_2401MC23/diffs/q3_deadlock.diff
git apply /path/to/lab7_submission_2401MC23/diffs/q4_priority.diff
```

Build and start xv6 with the i686 ELF cross compiler and QEMU:

```sh
make TOOLPREFIX=i686-elf- QEMU=qemu-system-i386 \
  CFLAGS='-fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -Wno-error=array-bounds -Wno-error=infinite-recursion -Wno-error=unused-but-set-variable -fno-omit-frame-pointer -fno-stack-protector -fno-pie -no-pie' \
  qemu-nox CPUS=2
```

At the xv6 prompt, run `threadtest`, `synctest`, and `deadlock`. To see Q4's priority order clearly, exit QEMU with **Control-A**, then **X**, and restart the same `make` command with `CPUS=1`; run `priotest` at the prompt.

The warning flags in the build command are for three warnings in untouched xv6 files that the newer cross compiler treats as errors. The kernel and all four test programs built with this command.

## Results and screenshots

Q1's final count is lower than 300,000 because the increments are deliberately unsynchronized; the number changes between runs. Q2 reaches 300,000, and its producer and consumer move all 12 items through a buffer of size five. Q3 reports the circular wait, prints a safe sequence for the safe matrix, and denies the Banker request that makes the state unsafe. In the Q4 run, the jobs finish in priority order and the aging messages show waiting priorities improve.

The PNGs are terminal-style images made from the captured QEMU serial output; the same output is in `output_log.txt`. To take screenshots of the Terminal window yourself, run a test at the xv6 prompt, press **Command-Shift-4**, then **Space**, and click the Terminal window. Save them as `Q1_threadtest.png`, `Q2_synctest.png`, `Q3_deadlock.png`, and `Q4_priotest.png` in `screenshots/`.
