
#include<vector>
#include<string>

#include "./include/kecx/kecx.hpp"

int main() {
    // @doc README.md
    // - `./examples/example_01.cpp`: A straightforward example of extracting
    //   various kinds of keyed comments from `./examples/data/input_01.cpp`.
    std::vector<std::string> ho   = {"@chunk"};
    std::vector<std::string> hf_h = {"@start"};
    std::vector<std::string> hf_f = {"@stop"};
    std::vector<std::string> e    = {"@block"};
    kecx::extract::extract(
        "./examples/data/input_01.cpp",
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
