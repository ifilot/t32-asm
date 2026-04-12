# Getting Started

Let us get a working T32 toolchain on your machine first.

## What the toolchain does

T32 is a **fictitious assembly language**. There is no physical T32 processor
you can buy or boot, so running a T32 program always involves software that
understands the instruction set.

In this repository, that software is the `t32` executable. It is an all-in-one
tool that combines the two things you need:

- an **assembler**, which turns `.s32` source code into T32 machine code
- a **virtual machine**, which emulates the T32 processor and runs that code

That means the usual workflow looks like this:

1. write a T32 assembly program in a `.s32` file
2. assemble it into a `.o32` object file
3. run that object file in the virtual machine

This is a very friendly setup for learning, because you do not need separate
tools, a custom runtime environment, or any real hardware. Everything is bundled
into one executable.

## Build the project

From the repository root:

```bash
mkdir build
cd build
cmake ..
make -j
```

This builds:

- the `t32` executable
- the test suite

To run the tests:

```bash
make test
```

## The three commands

The `t32` executable currently supports three subcommands, though most users
will mainly care about the first two:

- `assemble`: turn a `.s32` source file into a `.o32` object file
- `run`: execute a `.o32` program in the virtual machine
- `parse`: inspect the parsed token stream for a `.s32` file, mainly useful for debugging the assembler or source syntax

In other words:

- `assemble` is how you build a T32 program
- `run` is how you execute it
- `parse` is mostly a developer aid

Here is the normal assemble-and-run flow:

```bash
APP="hello"
./t32 assemble ../examples/${APP}.s32 ../examples/${APP}.o32
./t32 run ../examples/${APP}.o32
```

You can think of `assemble` as producing the bytes that would live in T32
memory, and `run` as starting the fictional processor inside the emulator.

## Where to go next

If you want the big picture first, continue to [The Machine](./the-machine.md).
If you would rather see code right away, jump to
[Your First Program](./first-program.md).
