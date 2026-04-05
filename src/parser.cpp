#include "parser.h"

#include <stdexcept>
#include <unordered_map>
#include <algorithm>

std::vector<Token> Parser::parse() {
    std::vector<Token> tokens;
    int line = 1;
    std::optional<std::string> current_top_label;

    // skip horizontal whitespace characters
    const auto skip_horizontal_ws = [&]() {
        while (!is_at_end() && is_horizontal_whitespace(peek())) {
            advance();
        }
    };

    // check identifier characters
    const auto read_identifier = [&]() {
        std::string value;
        while (!is_at_end() && is_identifier_char(peek())) {
            value += advance();
        }
        return value;
    };

    while (!is_at_end()) {
        char c = peek();

        // handle newlines separately (\n and Windows \r\n)
        if (c == '\n') {
            line++;
            advance();
            continue;
        }
        if (c == '\r') {
            advance();
            if (!is_at_end() && peek() == '\n') {
                advance();
            }
            line++;
            continue;
        }

        // ignore spaces/tabs only
        if (is_horizontal_whitespace(c)) {
            advance();
            continue;
        }

        // parse comments
        if (c == ';') {
            while (!is_at_end() && advance() != '\n') {}
            line++;
            continue;
        }

        // parse directives (.data / .ascii)
        if (c == '.') {
            advance(); // consume '.'

            std::string directive;
            while (!is_at_end() && std::isalpha(static_cast<unsigned char>(peek()))) {
                directive += advance();
            }

            std::transform(directive.begin(), directive.end(), directive.begin(), ::toupper);
            skip_horizontal_ws();

            if (directive == "DATA") {
                std::vector<uint8_t> bytes;

                while (!is_at_end() && peek() != '\n' && peek() != '\r' && peek() != ';') {
                    skip_horizontal_ws();

                    if (is_at_end() || peek() == '\n' || peek() == '\r' || peek() == ';') {
                        break;
                    }

                    bool is_hex = false;
                    if (peek() == '$') {
                        is_hex = true;
                        advance();
                    }

                    std::string number;
                    while (!is_at_end()) {
                        char p = peek();
                        if (is_hex && std::isxdigit(static_cast<unsigned char>(p))) {
                            number += advance();
                            continue;
                        }
                        if (!is_hex && std::isdigit(static_cast<unsigned char>(p))) {
                            number += advance();
                            continue;
                        }
                        break;
                    }

                    if (number.empty()) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Expected 8-bit value in .data"
                        );
                    }

                    int base = is_hex ? 16 : 10;
                    uint32_t value = std::stoul(number, nullptr, base);
                    if (value > 0xFF) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": .data value out of range (0-255)"
                        );
                    }
                    bytes.push_back(static_cast<uint8_t>(value));

                    skip_horizontal_ws();

                    if (!is_at_end() && peek() == ',') {
                        advance();
                        continue;
                    }

                    if (!is_at_end() && peek() != '\n' && peek() != '\r' && peek() != ';') {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Expected ',' between .data values"
                        );
                    }
                }

                if (bytes.empty()) {
                    throw std::runtime_error(
                        "Line " + std::to_string(line) + ": .data requires at least one value"
                    );
                }

                tokens.push_back(Token{
                    OpCode::INVALID,
                    std::nullopt,
                    std::nullopt,
                    std::nullopt,
                    line,
                    bytes
                });
                continue;
            }

            if (directive == "ASCII") {
                if (is_at_end() || peek() != '"') {
                    throw std::runtime_error(
                        "Line " + std::to_string(line) + ": .ascii expects a quoted string"
                    );
                }

                advance(); // consume opening quote
                std::vector<uint8_t> bytes;

                while (!is_at_end() && peek() != '"') {
                    char ch = advance();
                    if (ch == '\\') {
                        if (is_at_end()) {
                            throw std::runtime_error(
                                "Line " + std::to_string(line) + ": Unterminated escape in .ascii"
                            );
                        }
                        char escaped = advance();
                        switch (escaped) {
                            case 'n': bytes.push_back('\n'); break;
                            case 'r': bytes.push_back('\r'); break;
                            case 't': bytes.push_back('\t'); break;
                            case '\\': bytes.push_back('\\'); break;
                            case '"': bytes.push_back('"'); break;
                            default:
                                throw std::runtime_error(
                                    "Line " + std::to_string(line) + ": Unsupported escape sequence in .ascii"
                                );
                        }
                    }
                    else {
                        bytes.push_back(static_cast<uint8_t>(ch));
                    }
                }

                if (is_at_end() || peek() != '"') {
                    throw std::runtime_error(
                        "Line " + std::to_string(line) + ": Unterminated string in .ascii"
                    );
                }
                advance(); // consume closing quote

                tokens.push_back(Token{
                    OpCode::INVALID,
                    std::nullopt,
                    std::nullopt,
                    std::nullopt,
                    line,
                    bytes
                });
                continue;
            }

            throw std::runtime_error(
                "Line " + std::to_string(line) + ": Invalid directive: ." + directive
            );
        }

        // parse mnemonic or label
        if (c == '@' || is_identifier_start(c)) {
            std::string word;
            bool is_sublabel = false;

            if (peek() == '@') {
                is_sublabel = true;
                word += advance();
            }

            // read full word (allow alnum for labels like loop1)
            word += read_identifier();

            // normalize to uppercase
            std::transform(word.begin(), word.end(), word.begin(), ::toupper);

            // --- LABEL DEFINITION ---
            if (!is_at_end() && peek() == ':') {
                advance(); // consume ':'

                std::string label_definition = word;
                if (is_sublabel) {
                    if (word.size() == 1 || !is_valid_label_name(word.substr(1))) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Invalid sublabel name: " + word
                        );
                    }
                    if (!current_top_label.has_value()) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Sublabel requires a preceding top-level label"
                        );
                    }
                    label_definition = *current_top_label + "." + word.substr(1);
                }
                else {
                    if (!is_valid_label_name(word)) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Invalid label name: " + word
                        );
                    }
                    current_top_label = word;
                }

                tokens.push_back(Token{
                    OpCode::INVALID,
                    std::nullopt,
                    std::nullopt,
                    label_definition,
                    line
                });

                // skip spaces after label (for "label: LDI $48")
                skip_horizontal_ws();

                continue;
            }

            // --- OPCODE ---
            OpCode opcode = to_opcode(word);

            if (opcode == OpCode::INVALID) {
                throw std::runtime_error(
                    "Line " + std::to_string(line) + ": Invalid mnemonic: " + word
                );
            }

            // skip spaces before operand (not newline!)
            skip_horizontal_ws();

            std::optional<uint16_t> operand;
            std::optional<std::string> operand_label;

            // --- OPERAND ---
            if (!is_at_end()) {
                if (peek() == '$') {
                    // --- HEX ---
                    advance(); // consume '$'

                    std::string hex;

                    while (!is_at_end() && std::isxdigit(static_cast<unsigned char>(peek()))) {
                        hex += advance();
                    }

                    if (hex.empty()) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Expected hexadecimal number"
                        );
                    }

                    // stricter validation
                    if (!is_at_end() && std::isalnum(static_cast<unsigned char>(peek()))) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Invalid hex literal"
                        );
                    }

                    uint32_t val = std::stoul(hex, nullptr, 16);
                    if (val > 0xFFFF) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Hex value too large"
                        );
                    }

                    operand = static_cast<uint16_t>(val);
                }
                else if (std::isdigit(static_cast<unsigned char>(peek()))) {
                    // --- DECIMAL ---
                    std::string decimal;

                    while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
                        decimal += advance();
                    }

                    if (!is_at_end() && std::isalnum(static_cast<unsigned char>(peek()))) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Invalid decimal literal"
                        );
                    }

                    uint32_t val = std::stoul(decimal, nullptr, 10);
                    if (val > 0xFFFF) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Decimal value too large"
                        );
                    }

                    operand = static_cast<uint16_t>(val);
                }
                else if (is_identifier_start(peek())) {
                    // --- LABEL OPERAND ---
                    std::string label = read_identifier();

                    std::transform(label.begin(), label.end(), label.begin(), ::toupper);
                    operand_label = label;
                }
                else if (peek() == '@') {
                    advance(); // consume '@'

                    std::string sublabel = read_identifier();

                    std::transform(sublabel.begin(), sublabel.end(), sublabel.begin(), ::toupper);

                    if (!is_valid_label_name(sublabel)) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Invalid sublabel reference"
                        );
                    }
                    if (!current_top_label.has_value()) {
                        throw std::runtime_error(
                            "Line " + std::to_string(line) + ": Sublabel reference requires a preceding top-level label"
                        );
                    }

                    operand_label = *current_top_label + "." + sublabel;
                }
            }

            tokens.push_back(Token{
                opcode,
                operand,
                operand_label,
                std::nullopt,
                line
            });

            continue;
        }

        // unknown character → error (important!)
        throw std::runtime_error(
            "Line " + std::to_string(line) + ": Unexpected character: " + std::string(1, c)
        );
    }

    return tokens;
}

OpCode Parser::to_opcode(const std::string& word) {
    static const std::unordered_map<std::string, OpCode> map = {
        {"LDA", OpCode::LDA},
        {"STA", OpCode::STA},
        {"LDI", OpCode::LDI},
        {"LDP", OpCode::LDP},
        {"JSR", OpCode::JSR},
        {"RET", OpCode::RET},
        {"ADD", OpCode::ADD},
        {"SUB", OpCode::SUB},
        {"CMP", OpCode::CMP},
        {"PSH", OpCode::PSH},
        {"POP", OpCode::POP},
        {"JMP", OpCode::JMP},
        {"JEQ", OpCode::JEQ},
        {"JNG", OpCode::JNG},
        {"PRT", OpCode::PRT},
        {"RTR", OpCode::RTR},
        {"HLT", OpCode::HLT},
        {"IDP", OpCode::IDP},
        {"DDP", OpCode::DDP},
        {"AND", OpCode::AND},
        {"ORR", OpCode::ORR},
        {"XOR", OpCode::XOR},
        {"SHL", OpCode::SHL},
        {"SHR", OpCode::SHR},
        {"LDL", OpCode::LDL},
        {"LDH", OpCode::LDH},
        {"SDL", OpCode::SDL},
        {"SDH", OpCode::SDH},
        {"ADI", OpCode::ADI},
        {"SBI", OpCode::SBI},
        {"CMI", OpCode::CMI},
        {"NOP", OpCode::NOP},
    };

    auto it = map.find(word);
    if (it != map.end()) {
        return it->second;
    }

    return OpCode::INVALID;
}

#include <iostream>
#include <iomanip>

#include <iostream>
#include <iomanip>

const char* opcode_to_string(OpCode op) {
    switch (op) {
        case OpCode::LDA: return "LDA";
        case OpCode::STA: return "STA";
        case OpCode::LDI: return "LDI";
        case OpCode::LDP: return "LDP";
        case OpCode::JSR: return "JSR";
        case OpCode::RET: return "RET";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::CMP: return "CMP";
        case OpCode::PSH: return "PSH";
        case OpCode::POP: return "POP";
        case OpCode::JMP: return "JMP";
        case OpCode::JEQ: return "JEQ";
        case OpCode::JNG: return "JNG";
        case OpCode::PRT: return "PRT";
        case OpCode::RTR: return "RTR";
        case OpCode::HLT: return "HLT";
        case OpCode::IDP: return "IDP";
        case OpCode::DDP: return "DDP";
        case OpCode::AND: return "AND";
        case OpCode::ORR: return "ORR";
        case OpCode::XOR: return "XOR";
        case OpCode::SHL: return "SHL";
        case OpCode::SHR: return "SHR";
        case OpCode::LDL: return "LDL";
        case OpCode::LDH: return "LDH";
        case OpCode::SDL: return "SDL";
        case OpCode::SDH: return "SDH";
        case OpCode::ADI: return "ADI";
        case OpCode::SBI: return "SBI";
        case OpCode::CMI: return "CMI";
        case OpCode::NOP: return "NOP";
        default: return "INVALID";
    }
}

void Parser::print_tokens(const std::vector<Token>& tokens) {
    uint16_t offset = 0;

    for (const auto& token : tokens) {
        // skip label definitions
        if(token.label_definition) {
            continue;
        }

        // print offset (4-digit hex)
        std::cout << std::uppercase << std::hex
                  << std::setw(4) << std::setfill('0')
                  << offset << "  ";

        // print instruction
        if (!token.data_bytes.empty()) {
            std::cout << ".DATA";
        }
        else {
            std::cout << opcode_to_string(token.opcode);
        }

        // print operand if present
        if (token.operand.has_value()) {
            uint16_t value = token.operand.value();

            std::cout << " $"
                      << std::setw((value <= 0xFF) ? 2 : 4)
                      << std::setfill('0')
                      << value;
        } else if (!token.data_bytes.empty()) {
            std::cout << " ";
            for (size_t i = 0; i < token.data_bytes.size(); ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                std::cout << "$"
                          << std::setw(2) << std::setfill('0')
                          << static_cast<int>(token.data_bytes[i]);
            }
        } else if (token.operand_label) {
            std::cout << " " << token.operand_label.value();
        }

        std::cout << std::dec << '\n';

        // increment offset
        if (!token.data_bytes.empty()) {
            offset += static_cast<uint16_t>(token.data_bytes.size());
        }
        else {
            offset += instruction_size(token.opcode);
        }
    }
}
