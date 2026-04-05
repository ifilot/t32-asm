#include "e2e_test_harness.h"

int main(int argc, char** argv) {
    return run_test_case(argc, argv, "int2dec.s32", [](const std::filesystem::path& cases_dir) {
        run_input_output_case(cases_dir, "int2dec.s32", 0, "0");
        run_input_output_case(cases_dir, "int2dec.s32", 9, "9");
        run_input_output_case(cases_dir, "int2dec.s32", 42, "42");
        run_input_output_case(cases_dir, "int2dec.s32", 255, "255");
    });
}
