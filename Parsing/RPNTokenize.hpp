#ifndef RPNTOKENIZE_HPP
#define RPNTOKENIZE_HPP

#include "../Containers/VarIndice.hpp"
#include "../Containers/RPNToken.hpp"
//what namespace are WE?
#include <unordered_set>

namespace EVALPARSE
{

EVAL::RPNList tokenize(const std::string &expression, const VarIndiceMap &varMap, const uint varDepth,
                        const std::unordered_set<char> &globalSet);

}//namespace EVALPARSE

#endif //RPNTOKENIZE_HPP