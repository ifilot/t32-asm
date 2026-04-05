#include "e2e_test_harness.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "assembler.h"
#include "parser.h"
#include "vm.h"

namespace {

std::string read_text_file(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open test source: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<uint8_t> compile_s32_to_rom(const std::filesystem::path& source_path) {
    const std::string source = read_text_file(source_path);
    Parser parser(source);
    const std::vector<Token> tokens = parser.parse();

    Assembler assembler;
    return assembler.assemble(tokens);
}

std::string run_rom(const std::vector<uint8_t>& rom, const std::vector<uint8_t>& input_bytes = {}) {
    std::string output;
    size_t input_index = 0;

    VirtualMachine vm(
        rom,
        [&]() -> std::optional<uint8_t> {
            if (input_index >= input_bytes.size()) {
                return std::nullopt;
            }
            return input_bytes[input_index++];
        },
        [&](uint8_t value) {
            output.push_back(static_cast<char>(value));
        }
    );

    while (true) {
        VirtualMachine::RunResult result = vm.run();
        if (result == VirtualMachine::RunResult::Halted) {
            break;
        }

        throw std::runtime_error("Program requested more input than supplied.");
    }

    return output;
}

void expect_eq(const std::string& actual, const std::string& expected, std::string_view context) {
    if (actual != expected) {
        throw std::runtime_error(
            std::string(context) + "\nExpected: " + expected + "\nActual  : " + actual
        );
    }
}

} // namespace

void run_static_output_case(
    const std::filesystem::path& cases_dir,
    std::string_view source_name,
    const std::string& expected_output
) {
    std::cout << "[e2e] Running " << source_name << "\n";
    const std::vector<uint8_t> rom = compile_s32_to_rom(cases_dir / source_name);
    const std::string output = run_rom(rom);
    expect_eq(output, expected_output, "Output mismatch for " + std::string(source_name));
}

void run_input_output_case(
    const std::filesystem::path& cases_dir,
    std::string_view source_name,
    uint8_t input,
    const std::string& expected_output
) {
    std::cout << "[e2e] Running " << source_name << "\n";
    const std::vector<uint8_t> rom = compile_s32_to_rom(cases_dir / source_name);
    const std::string output = run_rom(rom, {input});
    expect_eq(output, expected_output, "Output mismatch for " + std::string(source_name));
}

int run_test_case(
    int argc,
    char** argv,
    std::string_view test_name,
    const std::function<void(const std::filesystem::path&)>& test_runner
) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <test_cases_dir>\n";
        return 2;
    }

    try {
        const std::filesystem::path cases_dir = argv[1];
        std::cout << "s32 test source: " << test_name << "\n";
        test_runner(cases_dir);
        std::cout << "e2e test passed for " << test_name << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
