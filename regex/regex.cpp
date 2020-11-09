#include <iostream>
#include <string>
#include "regex_automata.h"


int main()
{
    regex_automata::test();

    std::string regex, str;
    std::cin >> regex >> str;
    
    try
    {
        regex_automata automata(regex);
        std::cout << automata.biggest_substring_len(str) << '\n';
    }
    catch (std::runtime_error& err)
    {
        std::cout << "ERROR! " << err.what() << '\n';
        return 1;
    }
}
