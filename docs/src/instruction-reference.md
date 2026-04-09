# Instruction Reference

This chapter is the compact reference. Come here when you know what you want to
do and just need the exact instruction.

## Data movement

| Mnemonic | Bytes | Meaning | Effect |
| --- | --- | --- | --- |
| `LDA` | 1 | Load from memory | `A = mem[DP]` |
| `STA` | 1 | Store to memory | `mem[DP] = A` |
| `LDI imm8` | 2 | Load immediate into `A` | `A = imm` |
| `LDP imm16/label` | 3 | Load immediate into `DP` | `DP = addr` |
| `LDL` | 1 | Set low byte of `DP` from `A` | `DP[7:0] = A` |
| `LDH` | 1 | Set high byte of `DP` from `A` | `DP[15:8] = A` |
| `SDL` | 1 | Read low byte of `DP` into `A` | `A = DP[7:0]` |
| `SDH` | 1 | Read high byte of `DP` into `A` | `A = DP[15:8]` |
| `IDP` | 1 | Increment `DP` | `DP = DP + 1` |
| `DDP` | 1 | Decrement `DP` | `DP = DP - 1` |

## Arithmetic and comparison

| Mnemonic | Bytes | Meaning | Effect |
| --- | --- | --- | --- |
| `ADD` | 1 | Add memory to `A` | `A = A + mem[DP]` |
| `SUB` | 1 | Subtract memory from `A` | `A = A - mem[DP]` |
| `CMP` | 1 | Compare `A` with memory | flags from `A - mem[DP]` |
| `ADI imm8` | 2 | Add immediate | `A = A + imm` |
| `SBI imm8` | 2 | Subtract immediate | `A = A - imm` |
| `CMI imm8` | 2 | Compare immediate | flags from `A - imm` |

These instructions update flags except for store-like operations.

## Logic and shifts

| Mnemonic | Bytes | Meaning | Effect |
| --- | --- | --- | --- |
| `AND` | 1 | Bitwise AND | `A = A & mem[DP]` |
| `ORR` | 1 | Bitwise OR | `A = A \| mem[DP]` |
| `XOR` | 1 | Bitwise XOR | `A = A ^ mem[DP]` |
| `SHL` | 1 | Shift left | `A = A << 1` |
| `SHR` | 1 | Shift right | `A = A >> 1` |

## Stack and subroutines

| Mnemonic | Bytes | Meaning | Effect |
| --- | --- | --- | --- |
| `PSH` | 1 | Push `A` | `push(A)` |
| `POP` | 1 | Pop into `A` | `A = pop()` |
| `JSR addr` | 3 | Jump to subroutine | `push(PC); PC = addr` |
| `RET` | 1 | Return | `PC = pop()` |

## Control flow

| Mnemonic | Bytes | Meaning | Effect |
| --- | --- | --- | --- |
| `JMP addr` | 3 | Unconditional jump | `PC = addr` |
| `JEQ addr` | 3 | Jump if zero | if `Z`, jump |
| `JNG addr` | 3 | Jump if negative | if `N`, jump |
| `NOP` | 1 | Do nothing | no state change |
| `HLT` | 1 | Halt execution | stop VM |

## Input and output

| Mnemonic | Bytes | Meaning | Effect |
| --- | --- | --- | --- |
| `PRT` | 1 | Print byte in `A` | output `A` |
| `RTR` | 1 | Read byte into `A` | `A = read()` |

## Notes on flags

T32 maintains:

- `Z` for zero results
- `N` for negative results

In practice, this means you often compute or compare first, then branch.
