#pragma once

#include <stdint.h>
#include <optional>
#include <vector>

#include "opcodes.h"

struct Token {
    OpCode opcode;
    std::optional<uint16_t> operand = 0;
    std::optional<std::string> operand_label;
    std::optional<std::string> label_definition;
    int line = 0;
    std::vector<uint8_t> data_bytes;
};
