#ifndef EXTRACT_HPP
#define EXTRACT_HPP

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
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // @docstart README.md
    // ## Features
    //
    // The main feature of this library is extraction of documentation
    // contained within comments. This library does not prepare .html or other
    // documents for you --- it simply extracts what you have written into
    // comments into a location of your choice. Normally you would extract
    // documentation from your comments into text files and produce
    // a larger document out of the smaller ones. This README.md has been
    // produced simply by extracting documentation comments from source files
    // directly into the README.md.
    //
    // ## Main functions
    //
    // You may only need `kecx::extract::extract` or one of its signatures,
    // which currently vary wrt. the first argument (`file_path` / `file_paths`)
    // and arg `store`.
    //
    // ### Single file, arbitrary store callback function `store`
    // ```
    /**
     * @brief
     * Extract commented documentation from a single text file.
     * @param file_path
     * Path to file from which to extract documentation.
     * @param multiline_comment_start
     * Regex to identify multiline comment starts, e.g. "[/][*]".
     * @param multiline_comment_stop
     * Regex to identify multiline comment stops, e.g. "[*][/]".
     * @param singleline_comment
     * Regex to identify single line comments, e.g. "//".
     * @param header_only_tag_set
     * Tags considered header-only tags. E.g. `{"@doc"}`.
     * @param header_tag_set
     * Tags considered header tags in header-footer pairs. E.g. `{"@docstart"}`.
     * @param footer_tag_set
     * Tags considered footer tags in header-footer pairs. E.g. `{"@docstop"}`.
     * @param either_tag_set
     * Tags considered "either", i.e. both header and footer tags.
     * E.g. `{"@doc"}`.
     * @param store
     * Storage method, here an arbitrary callback function.
     * First three arguments reserved for `key`, `line`, and `line_no`.
     * @param store_only_comments_ho
     * If `true`, only comment lines are stored for header-only blocks.
     * @param store_only_comments_hf
     * If `true`, only comment lines are stored for header-footer blocks.
     * @param store_only_comments_e
     * If `true`, only comment lines are stored for "either" blocks.
     * @param verbosity
     * For debugging.
    */
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
        const bool& store_only_comments_hf = false,
        const bool& store_only_comments_e  = false,
        const int& verbosity = 0
    )
    // ```
    //
    // @docstop README.md
    {
        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        if (!utils::file_is_accessible(file_path)) {
            throw std::invalid_argument(
                "file_path = \""
                + file_path
                + "\" is not accessible --- does it exist?"
            );
        }
        std::ifstream file_connection;
        file_connection.open(file_path);

        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
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

        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        keysets::KeySet key_set_ho;
        keysets::KeySet key_set_hf;
        keysets::KeySet key_set_e;

        // header, footer ------------------------------------------------------
        bool search_for_hf = header_tag_set.size() > 0 &&
            footer_tag_set.size() > 0;
        std::regex re_hf_h;
        std::regex re_hf_f;
        if (search_for_hf) {
            re_hf_h = utils::tag_set_to_regex(header_tag_set);
            re_hf_f = utils::tag_set_to_regex(footer_tag_set);
        }
        // either --------------------------------------------------------------
        bool search_for_e = either_tag_set.size() > 0;
        std::regex re_e;
        if (search_for_e) {
            re_e = utils::tag_set_to_regex(either_tag_set);
        }
        // header_only ---------------------------------------------------------
        bool search_for_ho = header_only_tag_set.size() > 0;
        std::regex re_ho;
        if (search_for_ho) {
            re_ho = utils::tag_set_to_regex(header_only_tag_set);
        }

        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
        if (verbosity >= 1) {
            std::cout <<
                "kecx::extract::extract: preparations done --- "
                << "starting while loop over lines"
                << std::endl;
        }
        int line_no = -1;
        std::string line;
        bool is_comment_line = false;
        bool in_multiline_comment = false;
        while (std::getline(file_connection, line)) {
            // -----------------------------------------------------------------
            // -----------------------------------------------------------------
            line_no += 1;
            if (verbosity >= 2) {
                utils::print(line_no, "line_no");
            }

            // -----------------------------------------------------------------
            // comment detection -----------------------------------------------
            bool is_multiline_comment_start = false;
            bool is_multiline_comment_stop = false;
            if (search_for_multiline_comments) {
                if (in_multiline_comment) {
                    // check whether multiline stops here
                    is_multiline_comment_stop = utils::re_detect(
                        line, re_mlc_stop
                    );
                    if (is_multiline_comment_stop) {
                        in_multiline_comment = false;
                        is_comment_line = true;
                    }
                } else {
                    // check whether multiline starts here
                    is_multiline_comment_start = utils::re_detect(
                        line, re_mlc_start
                    );
                    if (is_multiline_comment_start) {
                        in_multiline_comment = true;
                        is_comment_line = true;
                    }
                }
            }
            // singleline comment detection ------------------------------------
            bool is_singleline_comment = false;
            if (!in_multiline_comment && search_for_singleline_comments) {
                is_singleline_comment = utils::re_detect(line, re_slc);
                is_comment_line = is_singleline_comment;
            }

            // comment detection verbosity -------------------------------------
            if (verbosity >= 2) {
                utils::print(is_multiline_comment_start, "is_multiline_comment_start");
                utils::print(is_multiline_comment_stop, "is_multiline_comment_stop");
                utils::print(in_multiline_comment, "in_multiline_comment");
                utils::print(is_singleline_comment, "is_singleline_comment");
                utils::print(is_comment_line, "is_comment_line");
            }

            // -----------------------------------------------------------------
            // key detection ---------------------------------------------------
            bool line_has_key = false;

            // detect header ---------------------------------------------------
            // e.g. "// @start my_key"
            std::string key_hf_h = "";
            if (search_for_hf && is_comment_line && !line_has_key) {
                key_hf_h = utils::re_extract_last_group(line, re_hf_h);
                if (key_hf_h != "") {
                    // found a header tag
                    line_has_key = true;
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
                    if (key_set_e.is_active(key_e)) {
                        key_set_e.deactivate(key_e);
                    } else {
                        key_set_e.activate(key_e);
                    }
                }
            }

            // detect header_only ----------------------------------------------
            // e.g. "// @chunk my_key"
            std::string key_ho = "";
            if (search_for_ho && is_comment_line && !line_has_key) {
                key_ho = utils::re_extract_last_group(line, re_ho);
            }
            if (key_ho != "") {
                // found a header_only tag
                line_has_key = true;
                key_set_ho.deactivate_all();
                key_set_ho.activate(key_ho);
            } else if (!is_comment_line || line_has_key) {
                key_set_ho.deactivate_all();
            }

            // -----------------------------------------------------------------
            // key detection verbosity -----------------------------------------
            if (verbosity >= 2) {
                utils::print(line, "line");
                utils::print(key_set_ho.get(), "key_set_ho.get()");
                utils::print(key_set_hf.get(), "key_set_hf.get()");
                utils::print(key_set_e.get(), "key_set_e.get()");
                utils::print(line_has_key, "line_has_key");
            }

            // -----------------------------------------------------------------
            // store -----------------------------------------------------------
            bool store_hf = !line_has_key &&
                key_set_hf.size() > 0 &&
                (is_comment_line || !store_only_comments_hf);
            bool store_e = !line_has_key &&
                key_set_e.size() > 0 &&
                (is_comment_line || !store_only_comments_e);
            bool store_ho = !line_has_key &&
                key_set_ho.size() > 0 &&
                (is_comment_line || !store_only_comments_ho);
            bool store_any = store_hf || store_e || store_ho;
            std::string clean_line = line;
            if (store_any) {
                for (auto clean_re : clean_re_set) {
                    clean_line = std::regex_replace(
                        clean_line,
                        clean_re,
                        "",
                        std::regex_constants::format_first_only
                    );
                }
                if (store_hf) {
                    for (std::string key : key_set_hf.get()) {
                        store(key, clean_line, line_no);
                    }
                }
                if (store_e) {
                    for (std::string key : key_set_e.get()) {
                        store(key, clean_line, line_no);
                    }
                }
                if (store_ho) {
                    for (std::string key : key_set_ho.get()) {
                        store(key, clean_line, line_no);
                    }
                }
            }

            // -----------------------------------------------------------------
            // store verbosity -------------------------------------------------
            if (verbosity >= 2) {
                utils::print(clean_line, "clean_line");
                utils::print(store_hf, "store_hf");
                utils::print(store_e, "store_e");
                utils::print(store_ho, "store_ho");
                utils::print(store_any, "store_any");
                if (verbosity >= 3) {
                    utils::press_enter_to_proceed();
                }
            }

            // -----------------------------------------------------------------
            // -----------------------------------------------------------------
        }
        // ---------------------------------------------------------------------
        // final checks --------------------------------------------------------
        auto key_set_hf_at_end = key_set_hf.get();
        if (key_set_hf_at_end.size() > 0) {
            throw keysets::KeySetNotEmptyException(key_set_hf_at_end);
        }

        if (verbosity >= 1) {
            std::cout <<
                "kecx::extract::extract: while loop done --- processed "
                << line_no << " lines in total"
                << std::endl;
        }
        // ---------------------------------------------------------------------
        // ---------------------------------------------------------------------
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // @docstart README.md
    //
    // ### Single file, write results into text files in directory `store`
    // ```
    /**
     * @brief
     * Extract commented documentation from a single text file.
     * Write extracted texts into dir `store` --- one file for each key.
     * @param file_path
     * Path to file from which to extract documentation.
     * @param multiline_comment_start
     * Regex to identify multiline comment starts, e.g. "[/][*]".
     * @param multiline_comment_stop
     * Regex to identify multiline comment stops, e.g. "[*][/]".
     * @param singleline_comment
     * Regex to identify single line comments, e.g. "//".
     * @param header_only_tag_set
     * Tags considered header-only tags. E.g. `{"@doc"}`.
     * @param header_tag_set
     * Tags considered header tags in header-footer pairs. E.g. `{"@docstart"}`.
     * @param footer_tag_set
     * Tags considered footer tags in header-footer pairs. E.g. `{"@docstop"}`.
     * @param either_tag_set
     * Tags considered "either", i.e. both header and footer tags.
     * E.g. `{"@doc"}`.
     * @param store
     * Here a path to a directory into which one text file will be written for
     * each key.
     * @param store_only_comments_ho
     * If `true`, only comment lines are stored for header-only blocks.
     * @param store_only_comments_hf
     * If `true`, only comment lines are stored for header-footer blocks.
     * @param store_only_comments_e
     * If `true`, only comment lines are stored for "either" blocks.
     * @param verbosity
     * For debugging.
    */
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
        const bool& store_only_comments_hf = false,
        const bool& store_only_comments_e  = false,
        const int& verbosity = 0
    )
    // ```
    //
    // @docstop README.md
    {
        if (!utils::file_is_accessible(store)) {
            std::string msg = "";
            msg += "Cannot access dir path store = ";
            msg += "\"" + store + "\"" + "; ";
            msg += "does it exist?";
            throw std::invalid_argument(msg);
        }
        extract(
            file_path,
            multiline_comment_start,
            multiline_comment_stop,
            singleline_comment,
            header_only_tag_set,
            header_tag_set,
            footer_tag_set,
            either_tag_set,
            store::store_to_txt_factory(store),
            store_only_comments_ho,
            store_only_comments_hf,
            store_only_comments_e,
            verbosity
        );
    }

    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // @docstart README.md
    //
    // ### Multiple files, `store` templated out
    // ```
    /**
     * @brief
     * Extract commented documentation from a single text file.
     * Write extracted texts into dir `store` --- one file for each key.
     * @param file_paths
     * One or more file paths to process.
     * @param multiline_comment_start
     * Regex to identify multiline comment starts, e.g. "[/][*]".
     * @param multiline_comment_stop
     * Regex to identify multiline comment stops, e.g. "[*][/]".
     * @param singleline_comment
     * Regex to identify single line comments, e.g. "//".
     * @param header_only_tag_set
     * Tags considered header-only tags. E.g. `{"@doc"}`.
     * @param header_tag_set
     * Tags considered header tags in header-footer pairs. E.g. `{"@docstart"}`.
     * @param footer_tag_set
     * Tags considered footer tags in header-footer pairs. E.g. `{"@docstop"}`.
     * @param either_tag_set
     * Tags considered "either", i.e. both header and footer tags.
     * E.g. `{"@doc"}`.
     * @param store
     * See other signatures.
     * @param store_only_comments_ho
     * If `true`, only comment lines are stored for header-only blocks.
     * @param store_only_comments_hf
     * If `true`, only comment lines are stored for header-footer blocks.
     * @param store_only_comments_e
     * If `true`, only comment lines are stored for "either" blocks.
     * @param verbosity
     * For debugging.
    */
    template<typename T>
    void extract(
        const std::vector<std::string>& file_paths,
        const std::string& multiline_comment_start,
        const std::string& multiline_comment_stop,
        const std::string& singleline_comment,
        const std::vector<std::string>& header_only_tag_set,
        const std::vector<std::string>& header_tag_set,
        const std::vector<std::string>& footer_tag_set,
        const std::vector<std::string>& either_tag_set,
        const T& store,
        const bool& store_only_comments_ho = true,
        const bool& store_only_comments_hf = false,
        const bool& store_only_comments_e  = false,
        const int& verbosity = 0
    )
    // ```
    //
    // @docstop README.md
    {
        for (std::string file_path : file_paths) {
            extract(
                file_path,
                multiline_comment_start,
                multiline_comment_stop,
                singleline_comment,
                header_only_tag_set,
                header_tag_set,
                footer_tag_set,
                either_tag_set,
                store,
                store_only_comments_ho,
                store_only_comments_hf,
                store_only_comments_e,
                verbosity
            );
        }
    } 
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------

} // namespace extract

#endif