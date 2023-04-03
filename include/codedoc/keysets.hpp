#ifndef KEYSETS_HPP
#define KEYSETS_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <functional>

#include "misc_utils.hpp"

namespace keysets{
    class KeyAlreadyExiscodedocxception : public std::exception {
        public:
            KeyAlreadyExiscodedocxception(const std::string& key) : key_(key) {}

            const char* what() const noexcept override {
                std::string msg = 
                    "Key \"" 
                    + key_
                    + "\" already exists in the the of currently known keys"
                    + " --- "
                    + "most likely this is due to not properly closing a "
                    + "comment block.";
                return(msg.c_str());
            }

        private:
            std::string key_;
    };

    class KeySetNotEmptyException : public std::exception {
        public:
            KeySetNotEmptyException(const std::vector<std::string>& keys) : keys_(keys) {}

            const char* what() const noexcept override {
                std::string msg; 
                msg += "The key set is not empty at the end of ";
                msg += "the file. This indicates unclosed comment blocks. ";
                msg += "Remaining keys: ";

                for (const auto& key : keys_) {
                    msg += key + ", ";
                }
                msg.pop_back(); // Remove the last comma and space
                msg.pop_back();
                return(msg.c_str());
            }

        private:
            std::vector<std::string> keys_;
    };

    class KeySet {
        private:
            std::vector<std::string> key_set;

        public:
            int size() {
                return(key_set.size());
            }

            bool in_key_set(const std::string& key) {
                return(utils::match_first(key, key_set) >= 0);
            }

            void activate(const std::string& key) {
                if (in_key_set(key)) {
                    throw KeyAlreadyExiscodedocxception(key);
                } else {
                    key_set.push_back(key);
                }
            }
            void deactivate(const std::string& key) {
                int m = utils::match_first(key, key_set);
                if (m >= 0) {
                    key_set.erase(key_set.begin() + m);
                }
            }
            void deactivate_all() {
                while (key_set.size() > 0) {
                    key_set.pop_back();
                }
            }
            std::vector<std::string> get() {
                return(key_set);
            }

    };

}
#endif
