#include "regex_automata.hpp"

regex_automata::regex_automata(char symbol)
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

void regex_automata::concatinate(regex_automata &arg)
{
    std::unordered_set<int> terminals;

    for (int i = 0; i < states.size(); ++i)
    {
        if (states[i].terminal)
        {
            terminals.insert(i);
        }
    }

    connect_states(arg, terminals);

    states.insert(states.end(), ++arg.states.begin(), arg.states.end());

    for (auto terminal : terminals)
    {
        for (auto const &[sym, reachable_from_eps] : arg.states[0].next)
        {
            states[terminal].next[sym].insert(reachable_from_eps.begin(), reachable_from_eps.end());
        }

        states[terminal].terminal = arg.states[0].terminal;
    }
}

void regex_automata::connect_states(regex_automata &arg, std::unordered_set<int> &terminals)
{
    int second_start_pos = states.size();
    for (auto &state : arg.states)
    {
        for (auto &[sym, next_set] : state.next)
        {
            std::unordered_set<int> next_set_copy(next_set);

            for (auto to : next_set_copy)
            {
                next_set.erase(to);
                if (to != 0)
                {
                    next_set.insert(to + second_start_pos - 1);
                }
                else
                {
                    next_set.insert(terminals.begin(), terminals.end());
                }
            }
        }
    }
}

void regex_automata::add(regex_automata &arg)
{
    int second_start_pos = states.size();

    update_next_arrays(arg);

    states.insert(states.end(), ++arg.states.begin(), arg.states.end());

    for (auto &[sym, next_set] : arg.states[0].next)
    {
        states[0].next[sym].insert(next_set.begin(), next_set.end());
    }

    states[0].terminal = states[0].terminal | arg.states[0].terminal;
}

void regex_automata::update_next_arrays(regex_automata &arg)
{
    for (auto &state : arg.states)
        for (auto &[sym, next_set] : state.next)
        {
            std::unordered_set<int> next_set_copy(next_set);

            for (auto to : next_set_copy)
            {
                if (to != 0)
                {
                    next_set.erase(to);
                    next_set.insert(to + states.size() - 1);
                }
            }
        }
}

void regex_automata::klini()
{
    for (auto &state : states)
    {
        if (state.terminal)
        {
            state.next[EPS].insert(0);
        }
    }
    states[0].terminal = 1;
}

bool regex_automata::reachable_by_eps(int from, int to, std::vector<char> &visited_states)
{
    if (states[from].next[EPS].count(to))
    {
        return true;
    }

    if (visited_states[from] != 0)
    {
        return false;
    }

    visited_states[from] = 1;

    for (auto i : states[from].next[EPS])
    {
        if (reachable_by_eps(i, to, visited_states))
        {
            return true;
        }
    }

    visited_states[from] = 0;
    return false;
}

int regex_automata::dfs_search_max_substring_len(const std::string &str, int curr_str_index, int state)
{
    if (curr_str_index == str.length())
    {
        if (states[state].terminal)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    int max_len = states[state].terminal ? 0 : -1;

    for (auto vertex : states[state].next[str[curr_str_index]])
    {
        int curr_len = dfs_search_max_substring_len(str, curr_str_index + 1, vertex);
        if (curr_len != -1 && curr_len + 1 > max_len)
        {
            max_len = curr_len + 1;
        }
    }

    return max_len;
}

void regex_automata::check_valid_states()
{
    std::vector<int> visited_states(states.size());
    dfs_check_valid(0, visited_states);

    for (int i = 0; i < states.size(); ++i)
    {
        if (!visited_states[i])
        {
            states[i].valid = false;
        }
    }
}

void regex_automata::dfs_check_valid(int state, std::vector<int> &visited_states)
{
    if (visited_states[state])
        return;

    visited_states[state] = true;
    for (auto next_state_set : states[state].next)
    {
        for (auto vertex : next_state_set.second)
        {
            dfs_check_valid(vertex, visited_states);
        }
    }
}

void regex_automata::remove_eps_moves()
{
    for (int from = 0; from < states.size(); ++from)
    {
        for (int to = 0; to < states.size(); ++to)
        {
            check_eps_closure(from, to);
        }
    }

    for (auto &state : states)
        state.next.erase(EPS);
}

void regex_automata::check_eps_closure(int from, int to)
{
    if (from == to && states[from].next[EPS].count(from) != 0)
    {
        states[from].next[EPS].erase(from);
    }
    else
    {
        std::vector<char> visited_states(states.size(), 0);

        if (reachable_by_eps(from, to, visited_states))
        {
            states[from].next[EPS].insert(to);

            if (states[to].terminal)
            {
                states[from].terminal = true;
            }
            for (auto &[sym, next_set] : states[to].next)
            {
                states[from].next[sym].insert(next_set.begin(),
                                              next_set.end());
            }
        }
    }
}

//public:

regex_automata::regex_automata(const std::string &str)
{
    std::stack<regex_automata> stack;
    for (auto symbol : str)
    {
        if ('a' <= symbol && symbol <= 'z' || symbol == EPS)
        {
            stack.emplace(regex_automata(symbol));
        }
        else if (symbol != '+' && symbol != '*' && symbol != '.')
        {
            throw std::runtime_error("bad character");
        }
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
        {
            throw std::runtime_error("incorrect regular expression");
        }
        stack.top().remove_eps_moves();
    }

    if (stack.size() != 1)
        throw std::runtime_error("incorrect regular expression");

    states.insert(states.end(), stack.top().states.begin(), stack.top().states.end());

    remove_eps_moves();
    check_valid_states();
}

int regex_automata::biggest_substring_len(const std::string &str)
{
    int max_len = 0;
    for (int i = 0; i < str.length(); ++i)
    {
        int curr_len = dfs_search_max_substring_len(str, i, 0);
        if (curr_len > max_len)
        {
            max_len = curr_len;
        }
    }

    return max_len;
}