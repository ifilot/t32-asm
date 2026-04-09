# Working With Data

In T32, data movement is half the story. Since there is only one general-purpose
register, it helps to get comfortable with the flow between `A`, `DP`, memory,
and the stack.

## Reading and writing memory

The pair `LDA` and `STA` are your basic memory tools.

```t32
    LDP value
    LDA
    ADI 1
    STA
```

This sequence:

1. points `DP` at `value`
2. loads `mem[DP]` into `A`
3. adds `1`
4. stores the result back into memory

## Walking through memory

Use `IDP` and `DDP` to move the data pointer.

```t32
    LDP buffer
    LDA
    IDP
    LDA
```

This reads two adjacent bytes from memory.

## Immediate values

Use immediate instructions when the value is known directly in the code:

- `LDI imm8`
- `ADI imm8`
- `SBI imm8`
- `CMI imm8`

These are often the simplest way to bring constants into a program.

## Splitting and rebuilding `DP`

T32 lets you inspect or modify the low and high bytes of `DP`:

- `LDL`: copy `A` into the low byte of `DP`
- `LDH`: copy `A` into the high byte of `DP`
- `SDL`: copy the low byte of `DP` into `A`
- `SDH`: copy the high byte of `DP` into `A`

This is useful when you need to compute an address in pieces.

## Bitwise operations

T32 includes a compact set of bitwise tools:

- `AND`
- `ORR`
- `XOR`
- `SHL`
- `SHR`

These all operate through `A`, usually using `mem[DP]` as the second operand for
the binary operations.
