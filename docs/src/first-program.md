# Your First Program

Let us begin with the smallest pleasant thing: printing a character.

```t32
    LDI 72
    PRT
    HLT
```

## What happens here

- `LDI 72` loads the decimal value `72` into register `A`
- `PRT` writes the byte in `A` to output
- `HLT` stops the machine

Since ASCII character `72` is `H`, the program prints `H`.

The same program written in hexadecimal looks like this:

```t32
    LDI $48
    PRT
    HLT
```

## A slightly friendlier example

The repository includes a full string-printing example in
[`examples/hello.s32`](https://github.com/ifilot/t32-asm/blob/main/examples/hello.s32).
Here is the heart of it:

```t32
    LDP text
    JSR printstr
    HLT

printstr:
@loop:
    LDA
    CMI $00
    JEQ @done
    PRT
    IDP
    JMP @loop
@done:
    RET
```

This shows a very typical T32 pattern:

- `DP` points at some data
- `LDA` pulls the current byte into `A`
- a compare sets flags
- a conditional jump decides what to do next

## Why this example matters

Even this tiny string printer demonstrates several important ideas:

- memory is read through `DP`
- `A` is the value you compute with and print
- flags let you make decisions
- subroutines let you reuse code without adding more registers
