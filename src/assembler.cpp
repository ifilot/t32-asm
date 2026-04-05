#include "assembler.h"

#include <fstream>

/**
 * Convert series of tokens into (virtual) machine code
 */
std::vector<uint8_t> Assembler::assemble(const std::vector<Token>& tokens) {
    labels.clear();

    // first pass: convert labels to offset values
    uint16_t offset = 0;
    for (const auto& token : tokens) {
        if (token.opcode == OpCode::INVALID && token.label_definition) {
            labels[*token.label_definition] = offset;
            continue;
        }
        if (!token.data_bytes.empty()) {
            offset += static_cast<uint16_t>(token.data_bytes.size());
        }
        else {
            offset += instruction_size(token.opcode);
        }
    }

    // second pass: construct output
    std::vector<uint8_t> output;
    output.reserve(offset);

    for(const auto& token: tokens) {
        // skip label definitions
        if (token.opcode == OpCode::INVALID && token.label_definition) {
            continue;
        }

        // raw data directives
        if (!token.data_bytes.empty()) {
            for (uint8_t byte : token.data_bytes) {
                output.push_back(byte);
            }
            continue;
        }

        // grab opcode
        uint8_t op = static_cast<uint8_t>(token.opcode);
        output.push_back(op);

        if (token.operand_label) {
            auto it = labels.find(*token.operand_label);
            if (it == labels.end()) {
                throw std::runtime_error("Undefined label: " + *token.operand_label);
            }
            uint16_t addr = it->second;
            uint8_t lo = addr & 0xFF;
            uint8_t hi = (addr >> 8) & 0xFF;

            output.push_back(lo);
            output.push_back(hi);
            continue; // parse next token
        }

        // check if there is any operand associated
        if (token.operand) {
            uint16_t val = *token.operand;

            // if so, check whether it is a 8-bit or 16-bit operand
            switch (instruction_size(token.opcode)) {
                case 2:
                {
                    uint8_t v8 = val;
                    output.push_back(v8);
                    break;
                }
                case 3:
                {
                    uint8_t lo = val & 0xFF;
                    uint8_t hi = (val >> 8) & 0xFF;

                    output.push_back(lo);
                    output.push_back(hi);
                    break;
                }
            }
        }
    }

    return output;
}

void Assembler::assemble(const std::string& path, const std::vector<Token>& tokens) {
    std::vector<uint8_t> output = assemble(tokens);

    std::ofstream out(path, std::ofstream::out | std::ofstream::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Could not open output file: " + path);
    }

    out.write(reinterpret_cast<const char*>(output.data()), static_cast<std::streamsize>(output.size()));
    out.close();
}
