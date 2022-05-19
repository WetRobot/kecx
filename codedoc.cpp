
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <regex>
#include <cassert>

// utils -----------------------------------------------------------------------
#include <sys/stat.h>
inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

std::string regex_extract_group_i(
    std::string x,
    std::regex re,
    int i
) {
    assert(i >= 0);
    std::smatch matched_groups;
    std::string out = "";
    if (std::regex_search(x, matched_groups, re) == true) {
        out = matched_groups.str(i + 1);
    }
    return(out);
}

// main function ---------------------------------------------------------------

void extract_keyed_comment_blocks(
    std::string                                         file_path,
    std::function<std::string(std::string)>             extract_comment_contents,
    std::function<std::string(std::string)>             extract_key,
    std::function<void(std::string line, std::string key)>   store_line
) {
    assert(file_exists(file_path));
    std::ifstream file(file_path);
    std::vector<std::string> update_key;
    std::vector<std::string> key_set;
    int line_no = -1;
    std::string line;
    while (std::getline(file, line)) {
        // @codedoc_comment_block extract_keyed_comment_blocks
        // Function `extract_keyed_comment_blocks` goes through the file
        // at `file_path` one line at a time.
        // 
        // The last character is always removed from a line using
        // `pop_back` because the last character is the line end character
        // which we never want to include anyway.
        // @codedoc_comment_block extract_keyed_comment_blocks
        line_no += 1;
        line.pop_back(); 
        std::string contents = extract_comment_contents(line);
        if (contents == "") {
            continue;
        }
        std::string key = extract_key(line);       
        if (key != "") {
            bool key_was_in_set = false;
            for (int i = 0; i < key_set.size(); i++) {
                if (key == key_set[i]) {
                    key_set.erase(key_set.begin() + i);
                    key_was_in_set = true;
                    break;
                }
            }
            if (!key_was_in_set) {
                key_set.push_back(key);
            }
        } else {
            for (std::string key_ : key_set) {
                store_line(contents, key_);
            }
        }
    }
}

// store to filesystem ---------------------------------------------------------
void store_line_to_filesystem(
    std::string line,
    std::string key
) {
    std::string output_file_path = "./output/" + key + ".txt";
    std::fstream file_conn;
    file_conn.open(output_file_path, std::ios_base::app | std::ios_base::in);
    if (file_conn.is_open()) {
        file_conn << line << std::endl;
    }
}

// using regexes ---------------------------------------------------------------
std::string __COMMENT_LINE_CPP_PREFIX = "^([ ]*[/]{2,}[ ]*)";
std::regex __COMMENT_LINE_CPP_REGEX = std::regex(
    __COMMENT_LINE_CPP_PREFIX + 
    "(.+)$"
);

std::string extract_comment_contents_cpp(std::string line) {
    return(regex_extract_group_i(line, __COMMENT_LINE_CPP_REGEX, 1));
}

std::regex __KEY_CPP_REGEX = std::regex(
    __COMMENT_LINE_CPP_PREFIX + 
    "(@codedoc_comment_block[ ]+)" + 
    "(.+)"
);
std::string extract_key_cpp(std::string line) {
    return(regex_extract_group_i(line, __KEY_CPP_REGEX, 2));
}

void store_line_to_console(std::string line, std::string key) {
    std::cout << "key = " << key << "\n";
    std::cout << "line = " << line << "\n";
}

void extract_keyed_comment_blocks_using_regexes(
    std::string file_path
) {
    extract_keyed_comment_blocks(
        file_path,
        extract_comment_contents_cpp,
        extract_key_cpp,
        store_line_to_filesystem
    );
}

int main() { 
    assert(extract_comment_contents_cpp("// @codedoc_comment_block key1") == "@codedoc_comment_block key1");
    assert(extract_comment_contents_cpp("// comment line 1") == "comment line 1");
    assert(extract_comment_contents_cpp("not a comment") == "");

    assert(extract_key_cpp("// @codedoc_comment_block key1") == "key1");
    assert(extract_key_cpp("// comment line 1") == "");
    assert(extract_key_cpp("not a comment") == "");

    assert(regex_extract_group_i("abc", std::regex("(a)(b)(c)"), 0) == "a");
    assert(regex_extract_group_i("abc", std::regex("(a)(b)(c)"), 1) == "b");
    assert(regex_extract_group_i("abc", std::regex("(a)(b)(c)"), 2) == "c");

    extract_keyed_comment_blocks_using_regexes("examples/input1.cpp");    
    return(0);
}

