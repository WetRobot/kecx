#include<vector>
#include<string>

#include "./include/codedoc/codedoc.hpp"

int main() {
    // @doc README.md
    // - `./doc/make_readme.cpp`
    std::vector<std::string> ho   = {"@doc"};
    std::vector<std::string> hf_h = {"@docstart"};
    std::vector<std::string> hf_f = {"@docstop"};
    std::vector<std::string> e    = {};
    std::vector<std::string> file_paths = {
        "include/codedoc/codedoc.hpp",
        "include/codedoc/extract.hpp"
    };

    codedoc::extract::extract(
        file_paths,
        "[/][*]",
        "[*][/]",
        "//",
        ho,
        hf_h,
        hf_f,
        e,
        "./",
        true,
        false,
        false,
        0
    );
    
    codedoc::extract::extract(
        "./doc/make_readme.sh",
        "",
        "",
        "#",
        ho,
        hf_h,
        hf_f,
        e,
        "./"
    );

    std::vector<std::string> more_file_paths = {
        "./doc/make_readme.cpp",
        "./examples/example_01.cpp"
    };
    codedoc::extract::extract(
        more_file_paths,
        "[/][*]",
        "[*][/]",
        "//",
        ho,
        hf_h,
        hf_f,
        e,
        "./",
        true,
        true,
        true,
        0
    );
    

    return(0);
}