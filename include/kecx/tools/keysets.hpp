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
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    class KeyAlreadyActiveException : public std::exception {
        public:
            KeyAlreadyActiveException(const std::string& key) : key_(key) {}

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
    
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    class KeyNotActiveException : public std::exception {
        public:
            KeyNotActiveException(const std::string& key) : key_(key) {}

            const char* what() const noexcept override {
                std::string msg = 
                    "Key \"" 
                    + key_
                    + "\" does not exist in the the of currently known keys"
                    + " --- "
                    + "most likely this is due to an internal error that "
                    + "you should report to the maintainer.";
                return(msg.c_str());
            }

        private:
            std::string key_;
    };

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
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

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    class KeySet {
        private:
            std::vector<std::string> key_set;

        public:
            /**
             * @brief 
             * Get number of elements in key set.
            */
            int size() {
                return(key_set.size());
            }

            /**
             * @brief 
             * Detect whether `key` has been stored in this key set.
             * @param key
             * A key to attempt to find in the key set.
            */
            bool is_active(const std::string& key) {
                return(utils::match_first(key, key_set) >= 0);
            }

            /**
             * @brief 
             * Adds key into key set. This is called "activating" to hint
             * different behaviour of the programme in the active state of
             * a key.
             * @param key
             * A key to attempt to activate. If the key is already active,
             * a `KeyAlreadyActiveException` is thrown.
            */
            void activate(const std::string& key) {
                if (is_active(key)) {
                    throw KeyAlreadyActiveException(key);
                } else {
                    key_set.push_back(key);
                }
            }

            /**
             * @brief 
             * Removes key from key set.
             * @param key
             * A key to attempt to deactivate. If the key is already active,
             * a `KeyAlreadyActiveException` is thrown.
            */
            void deactivate(const std::string& key) {
                int m = utils::match_first(key, key_set);
                if (m == -1) {
                    throw KeyNotActiveException(key);
                }
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
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
}
#endif
