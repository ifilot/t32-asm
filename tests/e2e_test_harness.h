#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>

void run_static_output_case(
    const std::filesystem::path& cases_dir,
    std::string_view source_name,
    const std::string& expected_output
);

void run_input_output_case(
    const std::filesystem::path& cases_dir,
    std::string_view source_name,
    uint8_t input,
    const std::string& expected_output
);

int run_test_case(
    int argc,
    char** argv,
    std::string_view test_name,
    const std::function<void(const std::filesystem::path&)>& test_runner
);
