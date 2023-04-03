
#include<vector>
#include<string>

#include "codedoc/extract.hpp"
#include "codedoc/store.hpp"

int main() {
    std::vector<std::string> ho   = {"@chunk"};
    std::vector<std::string> hf_h = {"@start"};
    std::vector<std::string> hf_f = {"@stop"};
    std::vector<std::string> e    = {"@block"};
    // store::cb_store_type storer = store::cb_store_to_txt_factory(
    //     // "C:/Users/joona/Documents/projects/TextBlockExtractor.cpp/output/"
    //     "./output/"
    // );
    store::cb_store_type storer = store::cb_store_default;
    extract::extract(
        "examples/input1.cpp",
        "[/][*]",
        "[*][/]",
        "//",
        ho,
        hf_h,
        hf_f,
        e,
        storer,
        true,
        false,
        false
    );
    return(0);
}
