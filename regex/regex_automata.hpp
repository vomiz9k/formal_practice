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

    regex_automata(char symbol)
    {
        node root;
        if (symbol == EPS)
        {
            root.terminal = true;
            states.push_back(root);
        }
        else
        {
            node terminal;

            terminal.terminal = true;
            root.next[symbol].insert(1);

            states.push_back(root);
            states.push_back(terminal);
        }
    }

    void concatinate(regex_automata &arg)
    {
        std::unordered_set<int> terminals;
        int second_start_pos = states.size();

        for (int i = 0; i < second_start_pos; ++i)
            if (states[i].terminal)
                terminals.insert(i);

        for (auto &state : arg.states)
            for (auto &next_states : state.next)
            {
                std::unordered_set<int> cache(next_states.second);

                for (auto to : cache)
                {
                    next_states.second.erase(to);
                    if (to != 0)
                        next_states.second.insert(to + second_start_pos - 1);
                    else
                        next_states.second.insert(terminals.begin(), terminals.end());
                }
            }

        states.insert(states.end(), ++arg.states.begin(), arg.states.end());

        for (auto i : terminals)
        {
            for (auto j : arg.states[0].next)
                states[i].next[j.first].insert(j.second.begin(), j.second.end());

            states[i].terminal = arg.states[0].terminal;
        }
    }

    void add(regex_automata &arg)
    {
        int second_start_pos = states.size();

        for (auto &state : arg.states)
            for (auto &next_states : state.next)
            {
                std::unordered_set<int> cache(next_states.second);

                for (auto to : cache)
                {
                    if (to != 0)
                    {
                        next_states.second.erase(to);
                        next_states.second.insert(to + second_start_pos - 1);
                    }
                }
            }

        states.insert(states.end(), ++arg.states.begin(), arg.states.end());
        for (auto j : arg.states[0].next)
            states[0].next[j.first].insert(j.second.begin(), j.second.end());
        states[0].terminal = states[0].terminal | arg.states[0].terminal;
    }

    void klini()
    {
        for (auto &state : states)
            if (state.terminal)
                state.next[EPS].insert(0);

        states[0].terminal = 1;
    }

    bool eps_move(int from, int to, std::vector<char> &used)
    {
        if (states[from].next[EPS].count(to))
            return true;

        if (used[from] != 0)
            return false;

        used[from] = 1;

        for (auto i : states[from].next[EPS])
            if (eps_move(i, to, used))
            {
                return true;
            }

        used[from] = 0;
        return false;
    }

    int dfs_search_max_substring_len(const std::string &str, int pos, int state)
    {
        if (pos == str.length())
            if (states[state].terminal)
                return 0;
            else
                return -1;

        int max_len = states[state].terminal ? 0 : -1;
        for (auto vertex : states[state].next[str[pos]])
        {
            int curr_max_len = dfs_search_max_substring_len(str, pos + 1, vertex);
            if (curr_max_len != -1 && curr_max_len + 1 > max_len)
                max_len = curr_max_len + 1;
        }

        return max_len;
    }

    void check_valid_states()
    {
        std::vector<int> used(states.size());
        dfs_check_valid(0, used);

        for (int i = 0; i < states.size(); ++i)
            if (!used[i])
                states[i].valid = false;
    }

    void dfs_check_valid(int state, std::vector<int> &used)
    {
        if (used[state])
            return;
        used[state] = true;
        for (auto next_state_set : states[state].next)
            for (auto vertex : next_state_set.second)
                dfs_check_valid(vertex, used);
    }

    void remove_eps_moves()
    {
        for (int i = 0; i < states.size(); ++i)
        {
            for (int j = 0; j < states.size(); ++j)
            {
                if (i == j && states[i].next[EPS].count(i) != 0)
                    states[i].next[EPS].erase(i);
                else
                {
                    std::vector<char> used(states.size(), 0);

                    if (eps_move(i, j, used))
                    {
                        states[i].next[EPS].insert(j);

                        if (states[j].terminal)
                            states[i].terminal = true;

                        for (auto moves : states[j].next)
                            states[i].next[moves.first].insert(states[j].next[moves.first].begin(),
                                                               states[j].next[moves.first].end());
                    }
                }
            }
        }

        for (int i = 0; i < states.size(); ++i)
            states[i].next.erase(EPS);
    }

public:
    regex_automata(const std::string &str)
    {
        std::stack<regex_automata> stack;
        for (auto symbol : str)
        {
            if ('a' <= symbol && symbol <= 'z' || symbol == EPS)
            {
                stack.emplace(regex_automata(symbol));
            }

            else if (symbol != '+' && symbol != '*' && symbol != '.')
                throw std::runtime_error("bad character");
            else if (!stack.empty())
            {
                if (symbol == '+')
                {
                    regex_automata second_arg = stack.top();
                    stack.pop();
                    stack.top().add(second_arg);
                }
                else if (symbol == '.')
                {
                    regex_automata second_arg = stack.top();
                    stack.pop();
                    stack.top().concatinate(second_arg);
                }
                else if (symbol == '*')
                {
                    stack.top().klini();
                }
            }
            else
                throw std::runtime_error("incorrect regular expression");
            stack.top().remove_eps_moves();
        }

        if (stack.size() != 1)
            throw std::runtime_error("incorrect regular expression");

        states.insert(states.end(), stack.top().states.begin(), stack.top().states.end());

        remove_eps_moves();
        check_valid_states();
    }

    int biggest_substring_len(const std::string &str)
    {
        int max_len = 0;
        for (int i = 0; i < str.length(); ++i)
        {
            int curr_max_len = dfs_search_max_substring_len(str, i, 0);
            if (curr_max_len > max_len)
                max_len = curr_max_len;
        }

        return max_len;
    }

    void dump()
    {
        for (int i = 0; i < states.size(); ++i)
        {
            if (states[i].valid)
            {
                std::cout << "state " << i << '\n';
                for (auto j : states[i].next)
                    for (auto k : j.second)
                        std::cout << i << " --" << j.first << "-> " << k << '\n';
                std::cout << "is terminal " << states[i].terminal << '\n';
                std::cout << "---------------------------------------------------\n";
            }
        }
    }

#ifdef _TEST

    static void test();
    static void test_parse();
    static void test_remove_eps_moves();
    static void test_klini();
    static void test_add();
    static void test_create();
    static void test_concatinate();
    static void test_search();

    static bool equal(regex_automata &first, regex_automata &second);
#endif
};

#ifdef _TEST
#include "test.hpp"
#endif
#endif
