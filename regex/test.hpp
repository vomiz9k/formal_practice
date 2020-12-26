#ifdef _TEST

void regex_automata::test()
{
    try
    {
        test_create();
        test_search();
        test_remove_eps_moves();
        test_klini();
        test_add();

        test_concatinate();

        test_parse();
    }
    catch (std::runtime_error &err)
    {
        std::cout << "ERROR! " << err.what() << '\n';
    }
}

void regex_automata::test_create()
{
    test_empty_create();
    test_single_sym_create();
}

void regex_automata::test_empty_create()
{
    regex_automata empty(EPS);
    if (empty.states.size() != 1 ||
        empty.states[0].terminal != true ||
        empty.states[0].valid != true ||
        empty.states[0].next.size() != 0)
        throw std::runtime_error("bad creation empty automata");
}

void regex_automata::test_single_sym_create()
{
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

void regex_automata::test_search()
{
    regex_automata auto1("acb..bab.c.*.ab.ba.+.+*a.");
    if (auto1.biggest_substring_len("abbaa") != 4)
        throw std::runtime_error("bad search");
}

void regex_automata::test_parse()
{
    test_single_sym_parse();
    test_concatinate_parse();
    test_add_parse();
    test_klini_parse();
}

void regex_automata::test_single_sym_parse()
{
    regex_automata symbol("a");
    regex_automata symbol1('a');
    if (!equal(symbol1, symbol))
        throw std::runtime_error("bad 1-sym automata parsing");
}

void regex_automata::test_add_parse()
{
    regex_automata add("ab+");
    regex_automata add1('a');
    regex_automata add2('b');
    add1.add(add2);
    if (!equal(add, add1))
        throw std::runtime_error("bad addition parsing");
}

void regex_automata::test_concatinate_parse()
{
    regex_automata conc("ab.");
    regex_automata conc1('a');
    regex_automata conc2('b');
    conc1.concatinate(conc2);
    if (!equal(conc, conc1))
        throw std::runtime_error("bad concatination parsing");
}

void regex_automata::test_klini_parse()
{
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

bool regex_automata::equal(regex_automata &first, regex_automata &second)
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

#endif