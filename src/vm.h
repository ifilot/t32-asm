#pragma once

#include <functional>
#include <optional>
#include <vector>
#include <stdint.h>
#include <stdexcept>

#include "opcodes.h"

class VirtualMachine {
public:
    using InputProvider = std::function<std::optional<uint8_t>()>;
    using OutputConsumer = std::function<void(uint8_t)>;

    enum class RunResult {
        Halted,
        WaitingForInput
    };

private:
    uint8_t reg_a = 0;
    uint16_t sp = 0;
    uint16_t dp = 0;
    std::vector<uint8_t> ram;
    uint8_t *ip;    // instruction pointer
    uint8_t *end;   // end of instruction stream

    bool flag_zero = true;
    bool flag_neg = false;
    InputProvider input_provider;
    OutputConsumer output_consumer;

public:
    VirtualMachine(
        const std::vector<uint8_t>& rom,
        InputProvider input = {},
        OutputConsumer output = {}
    );

    RunResult run();

private:
    inline uint8_t advance() {
        return *ip++;
    }

    inline uint16_t addr() {
        uint8_t lo = advance();
        uint8_t hi = advance();

        return static_cast<uint16_t>(lo)
            | (static_cast<uint16_t>(hi) << 8);
    }

    inline void require(size_t n) {
        if (ip + n > end) {
            throw std::runtime_error("Unexpected EOF");
        }
    }

    void set_flags();
};
