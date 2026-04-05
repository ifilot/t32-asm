#pragma once

#include <stdint.h>

enum class OpCode : uint8_t {
    LDA = 0x00,
    STA = 0x01,
    LDI = 0x02,
    LDP = 0x03,
    JSR = 0x04,
    RET = 0x05,
    ADD = 0x06,
    SUB = 0x07,
    CMP = 0x08,
    PSH = 0x09,
    POP = 0x0A,
    JMP = 0x0B,
    JEQ = 0x0C,
    JNG = 0x0D,
    PRT = 0x0E,
    RTR = 0x0F,
    HLT = 0x10,
    IDP = 0x11,
    DDP = 0x12,
    AND = 0x13,
    ORR = 0x14,
    XOR = 0x15,
    SHL = 0x16,
    SHR = 0x17,
    LDL = 0x18,
    LDH = 0x19,
    SDL = 0x1A,
    SDH = 0x1B,
    ADI = 0x1C,
    SBI = 0x1D,
    CMI = 0x1E,
    NOP = 0x1F,

    INVALID = 0xFF
};

static uint16_t instruction_size(OpCode opcode) {
    switch(opcode) {
        case OpCode::LDI: return 2; // opcode + 8-bit operand
        case OpCode::ADI:
        case OpCode::SBI:
        case OpCode::CMI: return 2; // opcode + 8-bit operand
        case OpCode::LDP: return 3; // opcode + 16-bit operand
        case OpCode::JMP:
        case OpCode::JEQ:
        case OpCode::JNG:
        case OpCode::JSR: return 3; // address (16-bit)

        default: return 1; // no operand
    }
}
