
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <regex>
#include <sys/stat.h>
#include <cassert>


inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

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
        // at `file_path` one line at a time. A line can only be
        // in a comment block if the callback function `detect_comment_line`
        // returns `true` for the line.
        // @codedoc_comment_block extract_keyed_comment_blocks
        line_no += 1;
        std::string contents = extract_comment_contents(line);
        if (contents == "") {
            continue;
        }
        // @codedoc_comment_block extract_keyed_comment_blocks
        // If `include` has returned `true`, a key is attempted to be
        // extracted from the line.
        // @codedoc_comment_block extract_keyed_comment_blocks
        std::string key = extract_key(line);       
        if (key != "") {
            // @codedoc_comment_block extract_keyed_comment_blocks
            // If `key != ""` for a given line, either the start or end of a
            // comment block has been encountered. If `key` has been found
            // an even (or zero) number of times before this time, 
            // it is the start of a block.
            // Otherwise it is the end of a block.
            // @codedoc_comment_block extract_keyed_comment_blocks 
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
std::regex __COMMENT_LINE_CPP_PREFIX_REGEX = std::regex(__COMMENT_LINE_CPP_PREFIX);
std::string __COMMENT_LINE_CPP_CONTENT = "(.+)$";
std::regex __COMMENT_LINE_CPP_CONTENT_REGEX = std::regex(__COMMENT_LINE_CPP_CONTENT);
std::regex __COMMENT_LINE_CPP_REGEX = std::regex(
    __COMMENT_LINE_CPP_PREFIX + __COMMENT_LINE_CPP_CONTENT
);
std::string clean_comment_line_cpp(std::string line) {
    return(std::regex_replace(line, __COMMENT_LINE_CPP_PREFIX_REGEX, ""));
}

std::string extract_comment_contents_cpp(std::string line) {
    std::string clean_line = clean_comment_line_cpp(line);
    std::string out = "";
    if (clean_line != line) {
        out = clean_line;
    }
    return(out);
}

std::string __KEY_PREFIX_CPP = __COMMENT_LINE_CPP_PREFIX + "@codedoc_comment_block[ ]+";
std::regex __KEY_PREFIX_CPP_REGEX = std::regex(__KEY_PREFIX_CPP);
std::string extract_key_cpp(std::string line) {
    std::string clean_line = std::regex_replace(line, __KEY_PREFIX_CPP_REGEX, "");
    std::string key = "";
    if (line != clean_line) {
        key = clean_line;
    }
    std::cout << "extract_key_cpp: key = " << key << "\n";
    std::cout << "extract_key_cpp: key.length() = " << key.length() << "\n";
    return(key);
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
    assert(extract_key_cpp("// @codedoc_comment_block key1") == "key1");
    assert(extract_key_cpp("// comment line 1") == "");
    assert(extract_key_cpp("not a comment") == "");
    extract_keyed_comment_blocks_using_regexes("examples/input1.cpp");    
    return(0);
}

