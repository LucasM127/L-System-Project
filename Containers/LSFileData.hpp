#ifndef LS_FILE_DATA_HPP
#define LS_FILE_DATA_HPP

#include <string>
#include <vector>

namespace LSYSTEM
{

//decomposed raw string data
struct LSFileData
{
    std::vector<std::string> defines;
    std::vector<std::string> globals;
    std::string axiom;
    std::string skippableLetters;
    std::vector<std::string> productions;
    std::vector<std::string> decompositions;
    std::vector<std::string> homomorphisms;
    std::vector<std::string> interpretor;

    void clear()
    {
        defines.clear();
        globals.clear();
        axiom.clear();
        skippableLetters.clear();
        productions.clear();
        decompositions.clear();
        homomorphisms.clear();
        interpretor.clear();
    }
};

}//namespace LSYSTEM

#endif //LS_FILE_DATA_HPP