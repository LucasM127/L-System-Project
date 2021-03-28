#ifndef OSTREAM_OPERATORS_HPP
#define OSTREAM_OPERATORS_HPP

#include <ostream>
#include "LSFileData.hpp"
#include "ProductionData.hpp"
#include "Alphabet.hpp"
#include "LSentence.hpp"
#include "RPNToken.hpp"

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSFileData &fd);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSData &ld);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::ProductionData &pd);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::ProductData &pd);

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSystemData &lsd);

std::ostream &operator<<(std::ostream &os, const LSYSTEM::Alphabet &alphabet);

std::ostream &operator<<(std::ostream &os, const LSYSTEM::LSentence &lsentence);

std::ostream &operator<<(std::ostream &os, const EVAL::RPNToken &token);

std::ostream &operator<<(std::ostream &os, const EVAL::RPNList &rpnList);

//https://stackoverflow.com/questions/535444/custom-manipulator-for-c-iostream
namespace OSManip 
{

struct Letters
{
    explicit Letters(std::ostream & os);

    template<typename Rhs>
    friend std::ostream & operator<<(Letters const& q, Rhs const& rhs);
    
    friend std::ostream & operator<<(Letters const& q, LSYSTEM::LSentence const& lsentence);
private:
    std::ostream & os;
};

struct LetterCreator { };
extern LetterCreator letter;

Letters operator<<(std::ostream & os, LetterCreator);

}//namespace OSManip

#endif //OSTREAM_OPERATORS_HPP