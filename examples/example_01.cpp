
#include<vector>
#include<string>

#include "codedoc/codedoc.hpp"

int main() {
    std::vector<std::string> ho   = {"@chunk"};
    std::vector<std::string> hf_h = {"@start"};
    std::vector<std::string> hf_f = {"@stop"};
    std::vector<std::string> e    = {"@block"};
    codedoc::extract::extract(
        "examples/data/input_01.cpp",
        "[/][*]",
        "[*][/]",
        "//",
        ho,
        hf_h,
        hf_f,
        e,
        store::store_default,
        true,
        false,
        false
    );
    return(0);
}