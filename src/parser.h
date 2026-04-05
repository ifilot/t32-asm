#pragma once

#include <string>
#include <optional>
#include <vector>
#include <regex>
#include <cctype>

#include "tokens.h"

/**
 * Parser class; reads a string and converts it to a list of tokens
 */
class Parser {
private:
    std::string_view input;
    const char* current;
    const char* end;

public:
    /**
     * Construct the Parser class
     */
    explicit Parser(std::string_view input) :
        input(input),
        current(input.data()),
        end(input.data() + input.size()) {}

    /**
     * Lex the string of characters to a vector of tokens (which can be
     * interpreted)
     */
    std::vector<Token> parse();

    static void print_tokens(const std::vector<Token>& tokens);

private:
    inline bool is_at_end() const {
        return current >= end;
    }

    inline char peek() const {
        return this->is_at_end() ? '\0' : *this->current;
    }

    inline char advance() {
        return this->is_at_end() ? '\0' : *this->current++;
    }

    inline char prev() {
        return *(this->current-1);
    }

    bool is_alpha(char c) {
        return std::isalpha(static_cast<unsigned char>(c));
    }

    bool is_hex(char c) {
        return std::isxdigit(static_cast<unsigned char>(c));
    }

    inline bool is_horizontal_whitespace(char c) {
        return std::isspace(static_cast<unsigned char>(c)) && c != '\n' && c != '\r';
    }

    inline bool is_identifier_start(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
    }

    inline bool is_identifier_char(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    inline bool is_valid_label_name(const std::string& label) {
        static const std::regex label_pattern("^[A-Z_][A-Z0-9_]*$");
        return std::regex_match(label, label_pattern);
    }

    OpCode to_opcode(const std::string& word);
};