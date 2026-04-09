# The Language

T32 source files use the `.s32` extension. The language is intentionally small
and easy to scan.

## Comments

Comments start with `;` and continue to the end of the line.

```t32
; this is a comment
LDI 65     ; this is also a comment
```

## Mnemonics

Instruction names are written as mnemonics such as `LDI`, `LDA`, `ADD`, and
`HLT`.

```t32
LDI 72
PRT
HLT
```

## Numbers

You can write numeric operands in decimal or hexadecimal.

- decimal: `10`
- hexadecimal: `$0A`

Examples:

```t32
LDI 65
LDI $41
LDP $1234
```

## Labels

Labels mark addresses in the program. They are useful for jumps, subroutines,
and data locations.

```t32
start:
    LDI 65
    PRT
    HLT
```

You can then refer to that label as an operand:

```t32
JMP start
```

## Sublabels

T32 also supports local-looking sublabels that begin with `@`. A sublabel is
scoped under the most recent top-level label.

```t32
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

Inside `printstr`, the assembler treats `@loop` and `@done` as labels under
that section.

## Data directives

T32 supports two simple directives for embedding data directly in the program.

### `.data`

Use `.data` for raw bytes:

```t32
bytes:
    .data $48, $69, $21, $00
```

### `.ascii`

Use `.ascii` for a quoted string:

```t32
message:
    .ascii "Hello"
    .data $00
```

Common escapes supported by the parser include `\n`, `\r`, `\t`, `\\`, and `\"`.
