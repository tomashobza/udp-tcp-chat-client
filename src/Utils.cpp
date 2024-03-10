#include "Utils.hpp"

std::string Utils::read_stdin()
{
    std::string input;
    std::getline(std::cin, input);
    return input;
}