#ifndef _REGEX_AUTOMATA
#define _REGEX_AUTOMATA
#include <unordered_map>
#include <vector>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <utility>
#include <stdexcept>

class regex_automata
{
    struct node
    {
        std::unordered_map<char, std::unordered_set<int>> next;
        bool terminal = false;
        bool valid = true;
    };

    static const int EPS = '1';
    std::vector<node> states;

    regex_automata(char symbol);

    void concatinate(regex_automata &arg);

    void connect_states(regex_automata &arg, std::unordered_set<int> &terminals);

    void add(regex_automata &arg);

    void update_next_arrays(regex_automata &arg);

    void klini();

    bool reachable_by_eps(int from, int to, std::vector<char> &visited_states);

    int dfs_search_max_substring_len(const std::string &str, int curr_str_index, int state);

    void check_valid_states();

    void dfs_check_valid(int state, std::vector<int> &visited_states);

    void remove_eps_moves();

    void check_eps_closure(int from, int to);

public:
    regex_automata(const std::string &str);

    int biggest_substring_len(const std::string &str);

#ifdef _TEST

    static void test();

    static void test_parse();
    static void test_single_sym_parse();
    static void test_concatinate_parse();
    static void test_add_parse();
    static void test_klini_parse();

    static void test_remove_eps_moves();

    static void test_klini();

    static void test_add();

    static void test_create();
    static void test_empty_create();
    static void test_single_sym_create();

    static void test_concatinate();
    static void test_search();

    static bool equal(regex_automata &first, regex_automata &second);
#endif
};

#ifdef _TEST
#include "test.hpp"
#endif
#endif
