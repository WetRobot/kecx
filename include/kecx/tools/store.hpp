#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <functional>

namespace store{
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------

    typedef
        std::function<void(
            const std::string& key,
            const std::string& line,
            const int& line_no
        )>
        store_type;
    
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Default function to handle storing extraction results.
     * Writes extracted line into e.g. `./output/x.txt` when `key = "x"`.
     * Note the extensions `.txt` that is always added.
     * @param key
     * Store data for this key.
     * @param line
     * Store this line.
     * @param line_no
     * Line number not stored anywhere by default.
    */
    void store_default(
        const std::string& key,
        const std::string& line,
        const int& line_no
    ) {
        std::string output_file_path = "./output/" + key + ".txt";
        std::fstream file_connection;
        file_connection.open(
            output_file_path,
            std::ios_base::app | std::ios_base::in
        );
        if (file_connection.is_open()) {
            file_connection << line << std::endl;
        }
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Outputs a function which in turn stores extracted line into
     * `output_dir_path` (e.g. `"./output/x"` for `key = "x").
     * Note, no file extension added.
     * @param output_dir_path
     * Path to directory into which the output function will write data.
    */
    auto store_to_txt_factory(std::string output_dir_path) {
        return [&](
            const std::string& key,
            const std::string& line,
            const int& line_no
        ) -> void
        {
            std::string output_file_path = output_dir_path + key;
            std::fstream file_connection;
            file_connection.open(
                output_file_path,
                std::ios_base::app | std::ios_base::in
            );
            if (file_connection.is_open()) {
                file_connection << line << std::endl;
            }
        };
    }
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
} // namespace store

#endif
