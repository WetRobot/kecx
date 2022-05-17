
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <regex>


bool update_key_data(
    std::string                 key,
    int                         line_no,
    std::vector<std::string>    key_set, 
    std::vector<int>            start_line_nos,
    std::vector<int>            stop_line_nos
) {
    bool was_in_set = false;
    if (key != "") {
        for (int i = 0; i < key_set.size(); i++) {
            if (key_set[i] == key) {
                was_in_set = true;
                key_set.erase(key_set.begin() + i);
                start_line_nos.erase(start_line_nos.begin() + i);
                stop_line_nos.erase(stop_line_nos.begin() + i);
                break;
            }
        }
        if (!was_in_set) {
            key_set.push_back(key);
            start_line_nos.push_back(line_no);
            stop_line_nos.push_back(-1);
        }
    }
    return(was_in_set);
}

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

void extract_keyed_comment_blocks(
    std::string                                         file_path,
    std::function<bool(std::string)>                    detect_comment_line,
    std::function<std::string(std::string)>             clean_comment_line,
    std::function<std::string(std::string)>             extract_key,
    std::function<void(std::string line, std::string key)>   store_line
) {
    std::ifstream file(file_path);
    std::vector<int> start_line_nos;
    std::vector<int> stop_line_nos;
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
        bool include = detect_comment_line(line);
        if (!include) {
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
            update_key_data(
                key, line_no, key_set, start_line_nos, stop_line_nos
            );
            std::cout << "key_set = ";
            for (std::string key_ : key_set) {
                std::cout << key << " ";
            }
            std::cout << "\n";
            
        } else {
            for (std::string key : key_set) {
                store_line(clean_comment_line(line), key);
            }
        }
    }
}

// using regexes ---------------------------------------------------------------
std::string __COMMENT_LINE_CPP_PREFIX = "[ ]*[/]{2,}[ ]*";
std::regex __COMMENT_LINE_CPP_PREFIX_REGEX = std::regex(__COMMENT_LINE_CPP_PREFIX);
std::string __COMMENT_LINE_CPP_CONTENT = ".*$";
std::regex __COMMENT_LINE_CPP_CONTENT_REGEX = std::regex(__COMMENT_LINE_CPP_CONTENT);
std::regex __COMMENT_LINE_CPP_REGEX = std::regex(
    "^(" + __COMMENT_LINE_CPP_PREFIX + ")(" + __COMMENT_LINE_CPP_CONTENT + ")"
);
bool detect_comment_line_cpp(std::string line) {
    return(std::regex_match(line, __COMMENT_LINE_CPP_REGEX));
}
std::string clean_comment_line_cpp(std::string line) {
    return(std::regex_replace(line, __COMMENT_LINE_CPP_PREFIX_REGEX, ""));
}
std::string __KEY_PREFIX_CPP = __COMMENT_LINE_CPP_PREFIX + "@codedoc_comment_block[ ]+";
std::regex __KEY_PREFIX_CPP_REGEX = std::regex(__KEY_PREFIX_CPP);
std::string extract_key_cpp(std::string line) {
    return(std::regex_replace(line, __KEY_PREFIX_CPP_REGEX, ""));
}
void store_line_to_console(std::string line, std::string key) {
    std::cout << "key = " << key << "; line = " << line << "\n";
}
void extract_keyed_comment_blocks_using_regexes(
    std::string                                             file_path
) {
    extract_keyed_comment_blocks(
        file_path,
        detect_comment_line_cpp,
        clean_comment_line_cpp,
        extract_key_cpp,
        store_line_to_console
    );
}

int main() { 
    extract_keyed_comment_blocks_using_regexes("examples/input1.cpp");    
    return(0);
}
