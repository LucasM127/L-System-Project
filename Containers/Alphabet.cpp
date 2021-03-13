#include "Alphabet.hpp"

namespace LSYSTEM
{

bool compatible(const Alphabet &a, const Alphabet &b)
{
    for(auto &pair : a)
    {
        if(b.find(pair.first) == b.end())
            continue;
        if(b.at(pair.first) != pair.second)
            return false;
    }
    for(auto &pair : b)
    {
        if(a.find(pair.first) == a.end())
            continue;
        if(a.at(pair.first) != pair.second)
            return false;
    }
    return true;
}

void combine(const Alphabet &a, Alphabet &b)
{
    for(auto &pair : a)
        b[pair.first] = pair.second;
}

} // LSYSTEM