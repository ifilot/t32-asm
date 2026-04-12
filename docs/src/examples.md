# Examples

The repository already contains a nice set of real examples:

- [`examples/hello.s32`](https://github.com/ifilot/t32-asm/blob/main/examples/hello.s32)
- [`examples/fibo.s32`](https://github.com/ifilot/t32-asm/blob/main/examples/fibo.s32)
- [`examples/int2dec.s32`](https://github.com/ifilot/t32-asm/blob/main/examples/int2dec.s32)
- [`examples/int2hex.s32`](https://github.com/ifilot/t32-asm/blob/main/examples/int2hex.s32)
- [`examples/prime_sieve_u8.s32`](https://github.com/ifilot/t32-asm/blob/main/examples/prime_sieve_u8.s32)

## Suggested reading order

If you are new to T32, this order works well:

1. `hello.s32`
2. `int2dec.s32`
3. `fibo.s32`
4. `int2hex.s32`
5. `prime_sieve_u8.s32`

That progression starts with simple output, then moves through data handling,
subroutines, loops, and larger algorithmic programs.

## A note about style

The examples in this repository already follow a helpful style:

- labels are descriptive
- sublabels keep loops local
- comments explain intent
- data is grouped near the bottom