#include <unordered_map>
#include <vector>
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

    static const char EPS = '1';

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

    void concatinate(regex_automata& arg)
    {
        std::unordered_set<int> terminals;
        int second_start_pos = states.size();

        for (int i = 0; i < second_start_pos; ++i)
            if (states[i].terminal)
                terminals.insert(i);

        for (auto& state : arg.states)
            for (auto& next_states : state.next)
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
            for(auto j : arg.states[0].next)
                states[i].next[j.first].insert(j.second.begin(), j.second.end());

            states[i].terminal = arg.states[0].terminal;
        }
    }

    void add(regex_automata& arg)
    {
        int second_start_pos = states.size();

        for (auto& state : arg.states)
            for (auto& next_states : state.next)
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
        for (auto& state : states)
            if (state.terminal)
                state.next[EPS].insert(0);

        states[0].terminal = 1;
    }



    bool eps_move(int from, int to, std::vector<char>& used)
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

    int dfs_search_max_substring_len(const std::string& str, int pos, int state)
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
        std::vector <int> used(states.size());
        dfs_check_valid(0, used);

        for (int i = 0; i < states.size(); ++i)
            if (!used[i])
                states[i].valid = false;
    }

    void dfs_check_valid(int state, std::vector<int>& used)
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
    regex_automata(const std::string& str)
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

        states = std::move(stack.top().states);

        remove_eps_moves();
        check_valid_states();
    }

    int biggest_substring_len(const std::string& str)
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

    static void test();
    static void test_parse();
    static void test_remove_eps_moves();
    static void test_klini();
    static void test_add();
    static void test_create();
    static void test_concatinate();
    static void test_search();

    static bool equal(regex_automata& first, regex_automata& second);
};




void regex_automata::test()
{
    try
    {
        test_create();
        test_concatinate();
        test_add();
        test_klini();
        test_remove_eps_moves();
        test_parse();
        test_search();
    }
    catch (std::runtime_error& err)
    {
        std::cout << "ERROR! " << err.what() << '\n';
    }
}

void regex_automata::test_search()
{
    regex_automata auto1("acb..bab.c.*.ab.ba.+.+*a.");
    if (auto1.biggest_substring_len("abbaa") != 4)
        throw std::runtime_error("bad search");
}

void regex_automata::test_parse()
{
    regex_automata symbol("a");
    regex_automata symbol1('a');
    if (!equal(symbol1, symbol))
        throw std::runtime_error("bad 1-sym automata parsing");

    regex_automata conc("ab.");
    regex_automata conc1('a');
    regex_automata conc2('b');
    conc1.concatinate(conc2);
    if (!equal(conc, conc1))
        throw std::runtime_error("bad concatination parsing");

    regex_automata add("ab+");
    regex_automata add1('a');
    regex_automata add2('b');
    add1.add(add2);
    if (!equal(add, add1))
        throw std::runtime_error("bad addition parsing");

    regex_automata klini("a*");
    regex_automata klini1('a');
    klini1.klini();
    klini1.remove_eps_moves();
    if (!equal(klini1, klini))
        throw std::runtime_error("bad klini parsing");
}

void regex_automata::test_remove_eps_moves()
{
    regex_automata symbol('a');
    symbol.klini();
    symbol.remove_eps_moves();
    if (symbol.states.size() != 2 ||
        symbol.states[0].terminal != true ||
        symbol.states[0].valid != true ||
        symbol.states[0].next.size() != 1 ||
        symbol.states[0].next['a'].size() != 1 ||
        *symbol.states[0].next['a'].begin() != 1 ||
        symbol.states[1].terminal != true ||
        symbol.states[1].valid != true ||
        symbol.states[1].next.size() != 1 ||
        symbol.states[1].next['a'].size() != 1 ||
        *symbol.states[1].next['a'].begin() != 1)
        throw std::runtime_error("bad 1-symbol klini epsilon removing");

}
void regex_automata::test_klini()
{
    regex_automata symbol('a');
    symbol.klini();
    if (symbol.states.size() != 2 ||
        symbol.states[0].terminal != true ||
        symbol.states[0].valid != true ||
        symbol.states[0].next.size() != 1 ||
        symbol.states[0].next['a'].size() != 1 ||
        *symbol.states[0].next['a'].begin() != 1 ||
        symbol.states[1].terminal != true ||
        symbol.states[1].valid != true ||
        symbol.states[1].next.size() != 1 ||
        *symbol.states[1].next[EPS].begin() != 0)
        throw std::runtime_error("bad 1-symbol klini");
}

void regex_automata::test_add()
{
    regex_automata first('a');
    regex_automata second('b');
    first.add(second);
    if (first.states.size() != 3 ||
        first.states[0].terminal != false ||
        first.states[0].valid != true ||
        first.states[0].next.size() != 2 ||
        first.states[0].next['a'].size() != 1 ||
        *first.states[0].next['a'].begin() != 1 ||
        first.states[1].terminal != true ||
        first.states[1].valid != true ||
        first.states[1].next.size() != 0 ||
        first.states[0].next['b'].size() != 1 ||
        *first.states[0].next['b'].begin() != 2 ||
        first.states[2].terminal != true ||
        first.states[2].valid != true ||
        first.states[2].next.size() != 0)
        throw std::runtime_error("bad addition of two 2-letters automatas");

    std::vector<regex_automata> symbol(3, 'a');
    regex_automata empty(EPS);

    symbol[0].add(empty);
    empty.add(symbol[1]);

    symbol[2].states[0].terminal = true;
    if (!equal(symbol[0], symbol[2]) ||
        !equal(empty, symbol[2]))
        throw std::runtime_error("bad addition with empty automata");
}

void regex_automata::test_create()
{
    regex_automata empty(EPS);
    if (empty.states.size() != 1 ||
        empty.states[0].terminal != true ||
        empty.states[0].valid != true ||
        empty.states[0].next.size() != 0)
        throw std::runtime_error("bad creation empty automata");

    regex_automata symbol('a');
    if (symbol.states.size() != 2 ||
        symbol.states[0].terminal != false ||
        symbol.states[0].valid != true ||
        symbol.states[0].next.size() != 1 ||
        symbol.states[0].next['a'].size() != 1 ||
        *symbol.states[0].next['a'].begin() != 1 ||
        symbol.states[1].terminal != true ||
        symbol.states[1].valid != true ||
        symbol.states[1].next.size() != 0)
        throw std::runtime_error("bad creation 1-letter automata");
}

void regex_automata::test_concatinate()
{
    regex_automata first('a');
    regex_automata second('b');
    first.concatinate(second);
    if (first.states.size() != 3 ||
        first.states[0].terminal != false ||
        first.states[0].valid != true ||
        first.states[0].next.size() != 1 ||
        first.states[0].next['a'].size() != 1 ||
        *first.states[0].next['a'].begin() != 1 ||
        first.states[1].terminal != false ||
        first.states[1].valid != true ||
        first.states[1].next.size() != 1 ||
        first.states[1].next['b'].size() != 1 ||
        *first.states[1].next['b'].begin() != 2 ||
        first.states[2].terminal != true ||
        first.states[2].valid != true ||
        first.states[2].next.size() != 0)
        throw std::runtime_error("bad concatination of two 2-letters automatas");

    std::vector<regex_automata> symbol(3, 'a');
    regex_automata empty(EPS);
    symbol[0].concatinate(empty);
    empty.concatinate(symbol[1]);

    if (!equal(symbol[0], symbol[2]) ||
        !equal(empty, symbol[2]))
        throw std::runtime_error("bad concatination with empty automata");
}

bool regex_automata::equal(regex_automata& first, regex_automata& second)
{
    if (first.states.size() != second.states.size())
        return false;
    for (int i = 0; i < first.states.size(); ++i)
        if (first.states[i].terminal == second.states[i].terminal &&
            first.states[i].valid == second.states[i].valid &&
            first.states[i].next.size() == second.states[i].next.size())
        {
            for (auto el_next : first.states[i].next)
            {
                for (auto vertex : el_next.second)
                    if (second.states[i].next[el_next.first].count(vertex) == 0)
                        return false;
            }
        }
        else
            return false;

    return true;
}