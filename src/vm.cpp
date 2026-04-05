#include "vm.h"

#include <cstdio>
#include <format>
#include <utility>

VirtualMachine::VirtualMachine(
    const std::vector<uint8_t>& rom,
    InputProvider input,
    OutputConsumer output
) : input_provider(std::move(input)), output_consumer(std::move(output)) {
    if (!input_provider) {
        input_provider = []() -> std::optional<uint8_t> {
            int value = getchar();
            if (value == EOF) {
                return 0;
            }
            return static_cast<uint8_t>(value);
        };
    }

    if (!output_consumer) {
        output_consumer = [](uint8_t value) {
            putchar(value);
        };
    }

    ram = rom;
    ram.resize(64 * 1024);
    ip = ram.data();
    end = ram.data() + ram.size();
    sp = ram.size() - 1;    // stack is always initialized to top of ram
}

VirtualMachine::RunResult VirtualMachine::run() {
    // consume bytes
    while (ip < end) {
        OpCode op = static_cast<OpCode>(advance());

        switch (op) {
            // load from memory
            case OpCode::LDA: {
                reg_a = ram[dp];
                set_flags();
                break;
            }

            // write to memory
            case OpCode::STA: {
                ram[dp] = reg_a;
                break;
            }

            // immediate load
            case OpCode::LDI: {
                require(1);
                uint8_t value = advance();
                reg_a = value;
                set_flags();
                break;
            }

            // set data pointer
            case OpCode::LDP: {
                require(2);
                uint16_t value = addr();
                dp = value;
                break;
            }

            // addition
            case OpCode::ADD: {
                reg_a += ram[dp];
                set_flags();
                break;
            }

            // subtraction
            case OpCode::SUB: {
                reg_a -= ram[dp];
                flag_neg = (reg_a < ram[dp]);
                flag_zero = (reg_a == 0);
                break;
            }

            case OpCode::CMP: {
                uint8_t tmp = reg_a - ram[dp];
                flag_neg = (reg_a < ram[dp]);
                flag_zero = (tmp == 0);
                break;
            }

            // push to stack
            case OpCode::PSH: {
                ram[sp--] = reg_a;
                break;
            }

            // pop from stack
            case OpCode::POP: {
                reg_a = ram[++sp];
                set_flags();
                break;
            }

            // jump to address
            case OpCode::JMP: {
                require(2);
                uint16_t address = addr();
                ip = ram.data() + address;
                break;
            }

            // jump on equal
            case OpCode::JEQ: {
                require(2);
                uint16_t address = addr();
                if(flag_zero) {
                    ip = ram.data() + address;
                }
                break;
            }

            // jump when reg is negative
            case OpCode::JNG: {
                require(2);
                uint16_t address = addr();
                if(flag_neg) {
                    ip = ram.data() + address;
                }
                break;
            }

            // jump to subroutine
            case OpCode::JSR: {
                require(2);
                uint16_t address = addr();

                // compute return address (offset)
                uint16_t ret = static_cast<uint16_t>(ip - ram.data());

                // push return address (16-bit, little endian)
                ram[sp--] = ret & 0xFF;        // low byte
                ram[sp--] = (ret >> 8) & 0xFF; // high byte

                // jump
                ip = ram.data() + address;
                break;
            }

            // return instruction
            case OpCode::RET: {
                uint16_t hi = ram[++sp];
                uint16_t lo = ram[++sp];

                uint16_t ret = (hi << 8) | lo;

                ip = ram.data() + ret;
                break;
            }

            // print char to terminal
            case OpCode::PRT: {
                output_consumer(reg_a);
                break;
            }

            // read character into register A
            case OpCode::RTR: {
                std::optional<uint8_t> input = input_provider();
                if (!input.has_value()) {
                    --ip; // retry RTR when caller provides input
                    return RunResult::WaitingForInput;
                }

                reg_a = input.value();
                set_flags();
                break;
            }

            case OpCode::HLT: {
                return RunResult::Halted;
            }

            case OpCode::IDP: {
                ++dp;
                break;
            }

            case OpCode::DDP: {
                --dp;
                break;
            }

            case OpCode::AND: {
                reg_a &= ram[dp];
                set_flags();
                break;
            }

            case OpCode::ORR: {
                reg_a |= ram[dp];
                set_flags();
                break;
            }

            case OpCode::XOR: {
                reg_a ^= ram[dp];
                set_flags();
                break;
            }

            case OpCode::SHL: {
                reg_a <<= 1;
                set_flags();
                break;
            }

            case OpCode::SHR: {
                reg_a >>= 1;
                set_flags();
                break;
            }

            case OpCode::LDL: {
                dp = static_cast<uint16_t>((dp & 0xFF00) | reg_a);
                break;
            }

            case OpCode::LDH: {
                dp = static_cast<uint16_t>((dp & 0x00FF) | (static_cast<uint16_t>(reg_a) << 8));
                break;
            }

            case OpCode::SDL: {
                reg_a = static_cast<uint8_t>(dp & 0x00FF);
                break;
            }

            case OpCode::SDH: {
                reg_a = static_cast<uint8_t>((dp >> 8) & 0x00FF);
                break;
            }

            case OpCode::ADI: {
                require(1);
                reg_a = static_cast<uint8_t>(reg_a + advance());
                set_flags();
                break;
            }

            case OpCode::SBI: {
                require(1);
                reg_a = static_cast<uint8_t>(reg_a - advance());
                set_flags();
                break;
            }

            case OpCode::CMI: {
                require(1);
                uint8_t val = advance();
                flag_neg = (reg_a < val);
                flag_zero = (reg_a == val);
                break;
            }

            case OpCode::NOP: {
                break;
            }

            // invalid instruction encountered
            default:
                throw std::runtime_error("Invalid instruction encountered: " +
                      std::format("${:02x}", static_cast<uint8_t>(op)) +
                      std::string(" at offset ") +
                      std::format("${:04x}", static_cast<uint16_t>(ip - ram.data())));
        }
    }

    return RunResult::Halted;
}

void VirtualMachine::set_flags() {
    flag_neg = false;
    flag_zero = (reg_a == 0);
}
