#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "tokens.h"

class Assembler {
private:
    std::unordered_map<std::string, uint16_t> labels;

public:
    std::vector<uint8_t> assemble(const std::vector<Token>& tokens);
    void assemble(const std::string& path, const std::vector<Token>& tokens);

private:

};
