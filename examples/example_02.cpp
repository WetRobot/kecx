
#include<vector>
#include<string>

#include "./include/kecx/kecx.hpp"

int main() {
    // @doc README.md
    // - `./examples/example_02.cpp`: Extract doxygen comments in
    //   `./examples/data/input_02.cpp` into separate text files --- though
    //   some clean-up work remains. Also, multiple function definitions
    //   in the same file would currently be a problem as arguments with
    //   the same name would be inserted into the same output file.
    std::vector<std::string> ho   = {"@"};
    std::vector<std::string> hf_h = {};
    std::vector<std::string> hf_f = {};
    std::vector<std::string> e    = {};
    kecx::extract::extract(
        "./examples/data/input_02.cpp",
        "[/][*]",
        "[*][/]",
        "//",
        ho,
        hf_h,
        hf_f,
        e,
        "./output/",
        true,
        false,
        false,
        0
    );
    return(0);
}
