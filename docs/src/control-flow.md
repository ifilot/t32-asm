# Control Flow

T32 keeps control flow simple and explicit.

## Unconditional jumps

Use `JMP` to continue execution somewhere else.

```t32
start:
    JMP again

again:
    HLT
```

## Conditional jumps

T32 currently provides two conditionals:

- `JEQ`: jump if the zero flag `Z` is set
- `JNG`: jump if the negative flag `N` is set

These instructions do not compare values by themselves. They rely on flags set
by an earlier operation such as `CMP`, `CMI`, `ADD`, `SUB`, `ADI`, or `SBI`.

```t32
    LDA
    CMI 10
    JEQ equal_to_ten
    JNG less_than_ten
```

In practice:

- `JEQ` is how you test for equality
- `JNG` is how you detect a negative result after subtraction or comparison

## A common loop pattern

```t32
loop:
    LDA
    CMI 0
    JEQ done
    IDP
    JMP loop
done:
    HLT
```

That style appears often in T32 because loops are built from a compare plus one
or two jumps.
