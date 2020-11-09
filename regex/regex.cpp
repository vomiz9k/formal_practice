#include <iostream>
#include <string>
#include "regex_automata.h"


int main()
{
    std::string regex, str;
    std::cin >> regex >> str;
    regex_automata automata(regex);
    std::cout << automata.biggest_substring_len(str);
}
