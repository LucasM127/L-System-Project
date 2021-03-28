#include "OstreamOperators.hpp"

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSFileData &fd)
{
    os<<"Define Strings:\n";
    for(const auto &string : fd.defines)
        os<<string<<"\n";
    os<<"Global Strings:\n";
    for(const auto &string : fd.globals)
        os<<string<<"\n";
    os<<"Axiom String:\n";
    os<<fd.axiom<<"\n";
    os<<"Skippable String:\n";
    os<<fd.skippableLetters<<"\n";
    os<<"Production Strings:\n";
    for(const auto &string : fd.productions)
        os<<string<<"\n";
    os<<"Decomposition Strings:\n";
    for(const auto &string : fd.decompositions)
        os<<string<<"\n";
    os<<"Homomorphism Strings:\n";
    for(const auto &string : fd.homomorphisms)
        os<<string<<"\n";
    os<<"Interpretor Strings:\n";
    for(const auto &string : fd.interpretor)
        os<<string<<"\n";

    return os;
}

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSData &ld)
{
    os<<"Productions: \n";
    for(const auto &string : ld.productions)
        os<<string<<"\n";
    os<<"Decompositions: \n";
    for(const auto &string : ld.decompositions)
        os<<string<<"\n";
    os<<"Homomorphisms: \n";
    for(const auto &string : ld.homomorphisms)
        os<<string<<"\n";
    os<<"Skippable Letters: \n";
    for(auto c : ld.skippableLetters)
        os<<c<<" ";
    os<<"\n";
//        os<<ld.skipString<<"\n";
    os<<"Globals:\n";
    for(const auto &pair : ld.globals)
        os<<pair.first<<" = "<<pair.second<<"\n";
    
    return os;
}    

std::ostream& operator<<(std::ostream &os, const LSYSTEM::ProductionData &pd)
{
    os<<pd.lContext<<" < "<<pd.letter<<" > "<<pd.rContext<<" =>\n";
    for(auto &p : pd.products)
        os<<p;
    os<<"\n";
    
    return os;
}

std::ostream& operator<<(std::ostream &os, const LSYSTEM::ProductData &pd)
{
    os<<pd.rawStatement<<"\n";
    os<<"Product: '"<<pd.product<<"' Condition: '"<<pd.conditional<<"' Weight: '"<<pd.productWeight<<"'\n";
    os<<"Var Map:";
    for(const auto &varIndice : pd.varIndiceMap)
        os << " " << varIndice.first << varIndice.second.letterNum << varIndice.second.paramNum;
    os<<"\nEvals:";
    for(const auto &v : pd.evalStrings)
        for(const auto &s : v)
            os << " " << s;
    return os;
}

std::ostream& operator<<(std::ostream &os, const LSYSTEM::LSystemData &lsd)
{
    os<<"Productions:\n";
    for(auto &s : lsd.productionDatas)
        os<<s<<"\n";
    os<<"\n";

    os<<"Decompositions:\n";
    for(auto &s : lsd.decompositionProductionDatas)
        os<<s<<"\n";
    os<<"\n";

    os<<"Homomorphisms:\n";
    for(auto &s : lsd.homomorphicProductionDatas)
        os<<s<<"\n";
    os<<"\n";

    os<<"Skippable Letters:";
    for(auto c : lsd.skippableLetters)
        os<<" "<<c;
    os<<"\n";

    os<<"Alphabet: "<<lsd.abc<<"\n";

    return os;
}

//a[3] b[4]
std::ostream &operator<<(std::ostream &os, const LSYSTEM::Alphabet &alphabet)
{
    for(auto &pair : alphabet)
        os<<pair.first<<"["<<pair.second<<"] ";

    return os;
}

std::ostream &operator<<(std::ostream &os, const LSYSTEM::LSentence &lsentence)
{
    for(uint i = 0; i < lsentence.size(); ++i)
    {
        os<<lsentence[i].id;
        if(lsentence[i].numParams)
        {
            os<<"("<<lsentence[i+1].value;
            for(uint j = 1; j < lsentence[i].numParams; ++j)
            {
                os<<","<<lsentence[i+j+1].value;
            }
            os<<")";
            i += lsentence[i].numParams;
        }
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const EVAL::RPNToken &token)
{
    switch (token.type)
    {
    case EVAL::RPNToken::TYPE::CONST:
        os<<"VAL"<<token.value;
        break;
    case EVAL::RPNToken::TYPE::GLOBAL:
        os<<"GLB:"<<token.token;
        break;
    case EVAL::RPNToken::TYPE::SYMBOL:
        os<<"SYM:"<<token.token;
        break;
    case EVAL::RPNToken::TYPE::OP:
        os<<"OP:"<<token.token;
        break;
    case EVAL::RPNToken::TYPE::VAR:
        os<<"VAR:"<<token.index;//Don't want to send in the varIndiceMap so there we have it
        break;
    default:
        break;
    }
    os<<" ";
    return os;
}

std::ostream &operator<<(std::ostream &os, const EVAL::RPNList &rpnList)
{
    os<<"[";
    for(auto &tok : rpnList)
    {
        if(tok.type == EVAL::RPNToken::TYPE::COMPLEX)
            os<<*tok.rpnList;
        else
            os<<tok;
    }
    os<<"]";

    return os;
}

//https://stackoverflow.com/questions/535444/custom-manipulator-for-c-iostream
namespace OSManip 
{

Letters::Letters(std::ostream & os):os(os){}

template<typename Rhs>
std::ostream & operator<<(Letters const &q, Rhs const &rhs)
{
    return q.os << rhs;
}

std::ostream & operator<<(Letters const &q, LSYSTEM::LSentence const &lsentence)
{
    for(uint i = 0; i < lsentence.size(); i = lsentence.next(i))
    {
        q.os<<lsentence[i].id;
    }
    return q.os;
}

Letters operator<<(std::ostream &os, LetterCreator)
{
    return Letters(os);
}

LetterCreator letter;

}//namespace OSManip
