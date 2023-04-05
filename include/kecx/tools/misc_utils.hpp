#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <functional>
#include <sys/stat.h>

namespace utils{
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Get position of first match of `x` in `y`. If there is no match,
     * return `-1`.
     * @param x
     * Object to match in `y`.
     * @param y
     * Vector of objects.
    */
    template<typename T>
    int match_first(const T& x, const std::vector<T>& y) {
        int m = -1;
        int i = -1;
        for (T y_elem : y) {
            i += 1;
            if (x == y_elem) {
                m = i;
                break;
            }
        }
        return(m);
    } 

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Return `true` if string `x` matches regex `r`, else `false`.
     * Uses `std::regex_search`.
     * @param x
     * A string.
     * @param r
     * A regex.
     * @param args
     * Additional arbitrary arguments passed to `std::regex_search`.
    */
    template <typename... Args>
    bool re_detect(
        const std::string& x, 
        const std::regex& r,
        Args... args
    ) {
        std::smatch m;
        bool out = false;
        if (std::regex_search(x, m, r, args...)) {
            out = true;
        }
        return(out);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Return extracted part of `x` that matches `r`. If there is no match,
     * return `""`.
     * @param x
     * A string.
     * @param r
     * A regex.
     * @param args
     * Additional arbitrary arguments passed to `std::regex_search`.
    */
    template <typename... Args>
    std::string re_extract(
        const std::string& x,
        const std::regex& r,
        Args... args
    ) {
        std::smatch m;
        std::string out = "";
        if (std::regex_search(x, m, r, args...)) {
            out = m.str();
        }
        return(out);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Return last capture group of match in string `x` against regex `r`.
     * @param x
     * A string.
     * @param r
     * A regex.
     * @param args
     * Additional arbitrary arguments passed to `std::regex_search`.
    */
    template <typename... Args>
    std::string re_extract_last_group(
        const std::string& x,
        const std::regex& r,
        Args... args
    ) {
        std::smatch m;
        std::string out = "";
        if (std::regex_search(x, m, r, args...)) {
            out = m[m.size() - 1].str();
        }
        return(out);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Defines what a key is allowed to look like. Currently allowed to contain
     * any characters on the same line (after the tag and any whitespace).
    */
    std::string key_regex_string() {
        return(".+");
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Returns a regex which matches any tag supplied via `tag_set`
     * plus any interim whitespaces, the key, and any trailing whitespaces.
     * The key is in the last capture group of the regex.
    */
    std::regex tag_set_to_regex(const std::vector<std::string>& tag_set) {
        std::string s = "(";
        for (std::string tag : tag_set) {
            s += "(" + tag + ")|";
        }
        s.pop_back(); // remove trailing |
        s += ")[ ]*(" + key_regex_string() + ")[ ]*$";
        std::regex r = std::regex(s);
        return(r);
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Printing for debugging.
     * @param x
     * Any object.
     * @param x_nm
     * Name of object in debug message.
    */
    template<typename T>
    void print(const T& x, const std::string& x_nm) {
        std::cout << x_nm << " = " << x << std::endl;
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Printing for debugging.
     * @param x
     * Any object.
     * @param x_nm
     * Name of object in debug message.
    */
    void print(const std::string& x, const std::string& x_nm) {
        std::cout << x_nm << " = \"" << x << "\"" << std::endl;
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Printing for debugging.
     * @param x
     * Any vector object.
     * @param x_nm
     * Name of object in debug message.
    */
    template<typename T>
    void print(const std::vector<T>& x, const std::string& x_nm) {
        std::string msg = x_nm + " = [";
        for (T x_elem : x) {
            msg += x_elem;
            msg += ", ";
        }
        if (x.size() > 0) {
            msg.pop_back(); // rm trailing ", "
            msg.pop_back(); // rm trailing ", "
        }
        msg += "]";
        std::cout << msg << std::endl;
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Requests user to press enter to proceed. Used in debugging.
    */
    void press_enter_to_proceed() {
        std::cout << "press enter to proceed" << std::endl;
        std::cin.ignore();
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    /**
     * @brief
     * Returns `true` if file is accessible (exists + you have permission to
     * read), else `false`.
     * @param file_path
     * Path to a file.
    */
    bool file_is_accessible(const std::string& file_path) {
        struct stat buffer;   
        return (stat (file_path.c_str(), &buffer) == 0); 
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
} // namespace utils

#endif // UTILS_HPP