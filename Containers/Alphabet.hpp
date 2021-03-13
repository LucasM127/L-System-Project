#ifndef ALPHABET_HPP
#define ALPHABET_HPP

#include <unordered_map>

namespace LSYSTEM
{

typedef std::unordered_map<char, unsigned int> Alphabet;

bool compatible(const Alphabet &a, const Alphabet &b);
//b = a + b
//for use iff compatible
void combine(const Alphabet &a, Alphabet &b);

}

#endif