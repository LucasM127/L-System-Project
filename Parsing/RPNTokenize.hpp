#ifndef RPNTOKENIZE_HPP
#define RPNTOKENIZE_HPP

#include "../Containers/VarIndice.hpp"
#include "../Containers/RPNToken.hpp"

EVAL::RPNList tokenize(const std::string &expression, const VarIndiceMap &varMap, const uint varDepth,
                        std::unordered_map<std::string, char> &globalNameMap, std::unordered_map<char, float> &globalVarMap);

#endif //RPNTOKENIZE_HPP