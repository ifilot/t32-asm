# T32

[![Build](https://github.com/ifilot/t32-asm/actions/workflows/ci.yml/badge.svg)](https://github.com/ifilot/t32-asm/actions/workflows/ci.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

![T32 logo](img/t32-logo-128px.png)

**T32** is a minimal instruction set architecture with **32 instructions**,
combining a **register**, **stack**, and **memory tape** into a compact and
expressive computational model.

## Machine Architecture

T32 operates on a small set of components:

- **Program Counter (PC)**  
  Index of the current instruction (16-bit value) in the program.

- **Register**  
  A single 8-bit general-purpose integer register called `A`.

- **Data Pointer**  
  A single 16-bit pointer called `DP`.

- **Stack Pointer**  
  A single 16-bit pointer called `SP`.

- **Memory Tape (RAM)**
  Array of 64 KiB values.

- **Flags**
  Two status flags are maintained by the VM: `Z` (zero) and `N` (negative).

Every T32 program is placed at the start of RAM. The PC is set to `$0000`.
When an invalid opcode is encountered, the VM raises a runtime error and stops.

## Instruction Set Architecture

| Mnemonic | Bytes | Description            | Operation                | Notes           |
| -------- | ----- | ---------------------- | ------------------------ | --------------- |
| `LDA`    | 1     | Load from memory       | `A = mem[DP]`            | Sets flags      |
| `STA`    | 1     | Store to memory        | `mem[DP] = A`            | —               |
| `LDI`    | 2     | Load immediate to A    | `A = imm(8)`             | Sets flags      |
| `LDP`    | 3     | Load immediate to DP   | `DP = imm(16)`           | —               |
| `JSR`    | 3     | Jump to subroutine     | `push(PC); PC = addr`    | Call subroutine |
| `RET`    | 1     | Return from subroutine | `PC = pop()`             | —               |
| `ADD`    | 1     | Add memory to A        | `A = A + mem[DP]`        | Sets flags      |
| `SUB`    | 1     | Subtract memory from A | `A = A - mem[DP]`        | Sets flags      |
| `CMP`    | 1     | Compare A with memory  | flags from `A - mem[DP]` | No write        |
| `PSH`    | 1     | Push A to stack        | `push(A)`                | —               |
| `POP`    | 1     | Pop from stack into A  | `A = pop()`              | Sets flags      |
| `JMP`    | 3     | Unconditional jump     | `PC = addr`              | —               |
| `JEQ`    | 3     | Jump if zero           | if `Z` → `PC = addr`     | From last op    |
| `JNG`    | 3     | Jump if negative       | if `N` → `PC = addr`     | From last op    |
| `PRT`    | 1     | Output byte            | `write(A)`               | 8-bit output    |
| `RTR`    | 1     | Read byte              | `A = read()`             | 8-bit input     |
| `HLT`    | 1     | Halt execution         | stop VM                  | —               |
| `IDP`    | 1     | Increment DP           | `DP = DP + 1`            | —               |
| `DDP`    | 1     | Decrement DP           | `DP = DP - 1`            | —               |
| `AND`    | 1     | Bitwise AND            | `A = A & mem[DP]`        | Sets flags      |
| `ORR`    | 1     | Bitwise OR             | `A = A \| mem[DP]`       | Sets flags      |
| `XOR`    | 1     | Bitwise XOR            | `A = A ^ mem[DP]`        | Sets flags      |
| `SHL`    | 1     | Shift left             | `A = A << 1`             | Sets flags      |
| `SHR`    | 1     | Shift right            | `A = A >> 1`             | Sets flags      |
| `LDL`    | 1     | Load DP low from A     | `DP[7:0] = A`            | —               |
| `LDH`    | 1     | Load DP high from A    | `DP[15:8] = A`           | —               |
| `SDL`    | 1     | Store DP low into A    | `A = DP[7:0]`            | —               |
| `SDH`    | 1     | Store DP high into A   | `A = DP[15:8]`           | —               |
| `ADI`    | 2     | Add immediate          | `A = A + imm(8)`         | Sets flags      |
| `SBI`    | 2     | Sub immediate          | `A = A - imm(8)`         | Sets flags      |
| `CMI`    | 2     | Compare immediate      | flags from `A - imm(8)`  | No write        |
| `NOP`    | 1     | No operation           | —                        | —               |

## Compilation

The T32 assembler and interpreter is bundled into a single executable which can
be built using

```bash
mkdir build
cd build
cmake ..
make -j
```

This builds both the executable as well as the test suite. To execute the tests,
run

```bash
make test
```

## Usage

The `t32` executable accepts 3 commands:

* `parse`: Perform a two-stage parse on a t32 source file (`.s32`) and shows
  the list of opcodes.
* `assemble`: Assembles a t32 source file into a t32 object file (`.o32`).
* `run`: Runs a t32 object file in the virtual machine

A useful oneliner for assembly and execution is as follows:

```bash
APP="hello"; ./t32 assemble ../examples/${APP}.s32 ../examples/${APP}.o32 && ./t32 run ../examples/${APP}.o32
```

## VS Code syntax highlighting for T32 source code

A minimal VS Code language extension is included in [`vscode/`](vscode):

- `vscode/package.json` registers the `t32` language for `.s32` files.
- `vscode/syntaxes/t32.tmLanguage.json` defines TextMate highlighting rules.
- `vscode/language-configuration.json` sets comment/bracket behavior.

To use it locally:

1. Open `vscode/` in VS Code.
2. Run `npm install -g @vscode/vsce` (once).
3. Run `vsce package`.
4. Install the generated `.vsix` via **Extensions → Install from VSIX...**.

After installation, `.s32` files will be highlighted as T32 assembly.