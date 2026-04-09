# Subroutines and the Stack

T32 has just enough stack machinery to make structured programs pleasant.

## Calling and returning

Use:

- `JSR addr` to call a subroutine
- `RET` to return

`JSR` saves the current program position on the stack before jumping to the
target address. `RET` restores that position.

## Saving values across calls

Because T32 has only one general-purpose register, `PSH` and `POP` are very
important.

```t32
printnl:
    PSH
    LDI $0A
    PRT
    POP
    RET
```

This pattern saves `A`, does some work, and restores `A` before returning.

That is a very T32 way to write helper routines: preserve the caller's value
when it matters, and be explicit about what your routine clobbers.

## A practical rule of thumb

When writing a subroutine, decide three things:

- what input it expects
- which state it may change
- whether it should preserve `A`

Writing those expectations in comments makes larger T32 programs much easier to
read.
