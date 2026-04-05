#include <CLI/CLI.hpp>
#include <fstream>
#include <vector>
#include <stdexcept>

#include "parser.h"
#include "assembler.h"
#include "vm.h"

// forward declaration
std::string read_file(const std::string& filename);
std::vector<uint8_t> read_binary_file(const std::string& filename);

int main(int argc, char* argv[]) {
    CLI::App app{"t32 assembler/interpreter"};

    std::string input_file;
    std::string output_file;

    // Default mode (VM execution)
    app.add_option("input", input_file, "Program file to run");

    // --- Subcommand: parse ---
    auto parse_cmd = app.add_subcommand("parse", "Parse and print instructions");
    parse_cmd->add_option("input", input_file, "Input file")->required();

    // --- Subcommand: assemble ---
    auto assemble_cmd = app.add_subcommand("assemble", "Assemble into object file");
    assemble_cmd->add_option("input", input_file, "Input file")->required();
    assemble_cmd->add_option("output", output_file, "Output file")->required();

    // --- Subcommand: run ---
    auto run_cmd = app.add_subcommand("run", "Run object file");
    run_cmd->add_option("input", input_file, "Input file")->required();

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);

    // --- Determine mode ---
    if (*parse_cmd) {
        const std::string code = read_file(input_file);

        Parser parser(code);
        auto tokens = parser.parse();
        Parser::print_tokens(tokens);

        return 0;
    }

    if (*assemble_cmd) {
        const std::string code = read_file(input_file);

        Parser parser(code);
        auto tokens = parser.parse();

        Assembler assembler;
        assembler.assemble(output_file, tokens);

        return 0;
    }

    if (*run_cmd) {
        const std::vector<uint8_t> rom = read_binary_file(input_file);
        VirtualMachine vm(rom);
        VirtualMachine::RunResult run_result = vm.run();
        if (run_result == VirtualMachine::RunResult::WaitingForInput) {
            throw std::runtime_error("Program is waiting for input but no input source is available.");
        }
        std::cout << std::endl;

        return 0;
    }

    return -1;
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open input file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<uint8_t> read_binary_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size < 0) {
        throw std::runtime_error("Failed to determine file size: " + filename);
    }

    std::vector<uint8_t> buffer(size);

    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    return buffer;
}
