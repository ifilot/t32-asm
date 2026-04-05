#include "e2e_test_harness.h"

int main(int argc, char** argv) {
    return run_test_case(argc, argv, "fibo.s32", [](const std::filesystem::path& cases_dir) {
        run_static_output_case(cases_dir, "fibo.s32", "1\n2\n3\n5\n8\n13\n21\n34\n55\n89\n144\n233\n");
    });
}
