#ifndef EXTRACT
#define EXTRACT

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <functional>

#include "misc_utils.hpp"
#include "keysets.hpp"
#include "store.hpp"

namespace extract {

    void extract(
        const std::string& file_path,
        const std::string& multiline_comment_start,
        const std::string& multiline_comment_stop,
        const std::string& singleline_comment,
        const std::vector<std::string>& header_only_tag_set,
        const std::vector<std::string>& header_tag_set,
        const std::vector<std::string>& footer_tag_set,
        const std::vector<std::string>& either_tag_set,
        const store::store_type& store = store::store_default,
        const bool& store_only_comments_ho = true,
        const bool& store_only_comments_hf = true,
        const bool& store_only_comments_e = true
    ) {
        std::ifstream file_connection;
        file_connection.open(file_path);

        // prep multiline / singleline -----------------------------------------
        bool search_for_multiline_comments = multiline_comment_start != "" &&
            multiline_comment_stop != "";
        bool search_for_singleline_comments = singleline_comment != "";
        std::regex re_mlc_start = std::regex(multiline_comment_start);
        std::regex re_mlc_stop = std::regex(multiline_comment_stop);
        std::regex re_slc = std::regex(singleline_comment);
        
        std::string clean_re_prefix = "[ ]*";
        std::string clean_re_suffix = "[ ]?";
        std::vector<std::regex> clean_re_set;
        if (search_for_multiline_comments) {
            clean_re_set.push_back(std::regex(
                clean_re_prefix + multiline_comment_start + clean_re_suffix
            ));
            clean_re_set.push_back(std::regex(
                clean_re_prefix + multiline_comment_stop + clean_re_suffix
            ));
        }
        if (search_for_singleline_comments) {            
            clean_re_set.push_back(std::regex(
                clean_re_prefix + singleline_comment + clean_re_suffix
            ));
        }

        // prep key search -----------------------------------------------------
        keysets::KeySet key_set_ho;
        keysets::KeySet key_set_hf;
        keysets::KeySet key_set_e;

        // header_only
        bool search_for_ho = header_only_tag_set.size() > 0;
        std::regex re_ho;
        if (search_for_ho) {
            re_ho = utils::tag_set_to_regex(header_only_tag_set);
        }
        // header, footer
        bool search_for_hf = header_tag_set.size() > 0 &&
            footer_tag_set.size() > 0;
        std::regex re_hf_h;
        std::regex re_hf_f;
        if (search_for_hf) {
            re_hf_h = utils::tag_set_to_regex(header_tag_set);
            re_hf_f = utils::tag_set_to_regex(footer_tag_set);
        }
        // either
        bool search_for_e = either_tag_set.size() > 0;
        std::regex re_e;
        if (search_for_e) {
            re_e = utils::tag_set_to_regex(either_tag_set);
        }

        // begin loop over lines -----------------------------------------------
        int line_no = -1;
        std::string line;
        bool is_comment_line = false;
        bool in_multiline_comment = false;
        while (std::getline(file_connection, line)) {
            line_no += 1;
            // detect whether in comment line ----------------------------------
            if (search_for_multiline_comments) {
                if (in_multiline_comment) {
                    // check whether multiline stops here
                    bool is_multiline_comment_stop = utils::re_detect(
                        line, re_mlc_stop
                    );
                    if (is_multiline_comment_stop) {
                        in_multiline_comment = false;
                        is_comment_line = true;
                    }
                } else {
                    // check whether multiline starts here
                    bool is_multiline_comment_start = utils::re_detect(
                        line, re_mlc_start
                    );
                    if (is_multiline_comment_start) {
                        in_multiline_comment = true;
                        is_comment_line = true;
                    }

                }
            }

            if (!in_multiline_comment && search_for_singleline_comments) {
                is_comment_line = utils::re_detect(line, re_slc);
            }

            // detect header_only ----------------------------------------------
            // e.g. "// @chunk my_key"
            std::string key_ho = "";
            if (search_for_ho && is_comment_line) {
                key_ho = utils::re_extract_last_group(line, re_ho);
            }
            bool line_has_key = false;
            if (key_ho != "") {
                // found a header_only tag
                line_has_key = true;
                key_set_ho.deactivate_all();
                key_set_ho.activate(key_ho);
            }

            // detect header ---------------------------------------------------
            // e.g. "// @start my_key"
            std::string key_hf_h = "";
            if (search_for_hf && is_comment_line && !line_has_key) {
                key_hf_h = utils::re_extract_last_group(line, re_hf_h);
                if (key_hf_h != "") {
                    // found a header tag
                    line_has_key = true;
                    key_set_ho.deactivate_all();
                    key_set_hf.activate(key_hf_h);
                }
            }

            // detect footer ---------------------------------------------------
            // e.g. "// @stop my_key"
            std::string key_hf_f = "";
            if (search_for_hf && is_comment_line && !line_has_key) {
                key_hf_f = utils::re_extract_last_group(line, re_hf_f);
                if (key_hf_f != "") {
                    // found a footer tag
                    line_has_key = true;
                    key_set_ho.deactivate_all();
                    key_set_hf.deactivate(key_hf_f);
                }
            }

            // detect either ---------------------------------------------------
            // e.g. "// @block my_key"
            std::string key_e = "";
            if (search_for_e && is_comment_line && !line_has_key) {
                key_e = utils::re_extract_last_group(line, re_e);
                if (key_e != "") {
                    // found an either tag
                    line_has_key = true;
                    key_set_ho.deactivate_all();
                    if (key_set_e.in_key_set(key_e)) {
                        key_set_e.deactivate(key_e);
                    } else {
                        key_set_e.activate(key_e);
                    }
                }
            }

            // store -----------------------------------------------------------
            if (!line_has_key) {
                for (auto clean_re : clean_re_set) {
                    line = std::regex_replace(line, clean_re, "");
                }
                if (is_comment_line || !store_only_comments_ho) {
                    for (std::string key : key_set_ho.get()) {
                        store(key, line, line_no);
                    }
                }
                if (is_comment_line || !store_only_comments_hf) {
                    for (std::string key : key_set_hf.get()) {
                        store(key, line, line_no);
                    }
                }
                if (is_comment_line || !store_only_comments_e) {
                    for (std::string key : key_set_e.get()) {
                        store(key, line, line_no);
                    }
                }
            }
        }
        // while loop has ended. final checks ----------------------------------
        auto key_set_hf_at_end = key_set_hf.get();
        if (key_set_hf_at_end.size() > 0) {
            throw keysets::KeySetNotEmptyException(key_set_hf_at_end);
        }
    }
    
    void extract(
        const std::string& file_path,
        const std::string& multiline_comment_start,
        const std::string& multiline_comment_stop,
        const std::string& singleline_comment,
        const std::vector<std::string>& header_only_tag_set,
        const std::vector<std::string>& header_tag_set,
        const std::vector<std::string>& footer_tag_set,
        const std::vector<std::string>& either_tag_set,
        const std::string& store,
        const bool& store_only_comments_ho = true,
        const bool& store_only_comments_hf = true,
        const bool& store_only_comments_e = true
    ) {
        extract(
            file_path,
            multiline_comment_start,
            multiline_comment_stop,
            singleline_comment,
            header_only_tag_set,
            header_tag_set,
            footer_tag_set,
            either_tag_set,
            store::store_to_txt_factory(store)
        );
    }
}

#endif