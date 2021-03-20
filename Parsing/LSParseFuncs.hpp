#ifndef LS_PARSE_FUNCS_HPP
#define LS_PARSE_FUNCS_HPP

//use parsing funcs to create the data structures... incrementally?
#include "ParsingFuncs.hpp"
#include "../Containers/LSentence.hpp"
//these Throw exceptions!
float loadNumber(const std::string &string);

std::vector<float> loadParams(const std::string &s, uint &i);

LSYSTEM::LModule loadModule(const std::string &s, uint &i, std::vector<float> &vals);

LSYSTEM::LSentence loadLSentence(const std::string &s);

#endif //LS_PARSE_FUNCS_HPP