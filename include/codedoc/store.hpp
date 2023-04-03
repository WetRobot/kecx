#ifndef STORE
#define STORE

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <functional>

namespace store{

    typedef
        std::function<void(
            const std::string& key,
            const std::string& line,
            const int& line_no
        )>
        store_type;
    
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

    auto store_to_txt_factory(std::string output_dir_path) {
        return [&](
            const std::string& key,
            const std::string& line,
            const int& line_no
        ) -> void
        {
            std::string output_file_path = output_dir_path + key + ".txt";
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
}
#endif
