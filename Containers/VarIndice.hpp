#ifndef VARINDICE_HPP
#define VARINDICE_HPP

#include <map>

//not used by lsystem...
//just for ...
//HMMMMMMMM
struct VarIndice
{
    VarIndice() : letterNum(0), paramNum(0) {}
    VarIndice(unsigned int a, unsigned int b) : letterNum(a), paramNum(b) {}
	unsigned int letterNum;
	unsigned int paramNum;
};
typedef std::map<char, VarIndice> VarIndiceMap;

#endif //VARINDICE_HPP