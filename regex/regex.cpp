#include <iostream>
#include <string>

#define _TEST

#include "regex_automata.hpp"

int main()
{
#ifdef _TEST
    regex_automata::test();
#endif

    std::string regex, str;
    std::cin >> regex >> str;

    try
    {
        regex_automata automata(regex);
        std::cout << automata.biggest_substring_len(str) << '\n';
    }
    catch (std::runtime_error &err)
    {
        std::cout << "ERROR! " << err.what() << '\n';
        return 1;
    }
}
