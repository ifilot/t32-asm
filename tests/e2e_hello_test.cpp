#include "e2e_test_harness.h"

int main(int argc, char** argv) {
    return run_test_case(argc, argv, "hello.s32", [](const std::filesystem::path& cases_dir) {
        run_static_output_case(cases_dir, "hello.s32", "Hello World!");
    });
}
