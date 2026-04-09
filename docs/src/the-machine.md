# The Machine

T32 is small enough that we can describe the whole machine on one page. That is
a lovely property for learning.

## Registers and pointers

T32 has four pieces of execution state you will think about most often:

- `PC`: the program counter, which points at the next instruction
- `A`: a single 8-bit general-purpose register
- `DP`: a 16-bit data pointer used to access memory
- `SP`: a 16-bit stack pointer used by stack operations and subroutines

## Memory

T32 has a **64 KiB** memory tape. Programs are loaded at the start of memory,
and execution begins at address `$0000`.

This means code and data live in the same address space. In practice, many T32
programs place instructions first and data later in the file, often after a
`HLT` or at the bottom of the source.

## Flags

The virtual machine maintains two status flags:

- `Z`: set when the most recent result is zero
- `N`: set when the most recent result is negative in signed 8-bit terms

You mainly use these with conditional jumps such as `JEQ` and `JNG`.

## The machine model in one sentence

Most T32 programs follow a simple rhythm:

1. point `DP` at some memory
2. move bytes between memory and `A`
3. compute in `A`
4. branch using flags
5. use the stack when a value must survive across calls or temporary work

That is the whole dance.

## Why this design is interesting

T32 is deliberately constrained:

- only one general-purpose register
- only 32 instructions
- 8-bit arithmetic
- explicit memory movement

These constraints make data flow visible. You can often look at a short T32
program and see exactly where every value comes from and where it goes.
