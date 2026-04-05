#include "e2e_test_harness.h"

int main(int argc, char** argv) {
    return run_test_case(argc, argv, "int2hex.s32", [](const std::filesystem::path& cases_dir) {
        run_input_output_case(cases_dir, "int2hex.s32", 0, "00");
        run_input_output_case(cases_dir, "int2hex.s32", 15, "0F");
        run_input_output_case(cases_dir, "int2hex.s32", 42, "2A");
        run_input_output_case(cases_dir, "int2hex.s32", 255, "FF");
    });
}
