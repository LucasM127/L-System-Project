#include "LSParseFuncs.hpp"

//validation //throws exceptions. //for use in constructors.
float loadNumber(const std::string &s)
{
    if(!s.size()) throw std::runtime_error("-empty-");
    bool hasDecimals = false;
    for(auto c : s)
    {
        if(c == '.')
        {
            if(hasDecimals)
                throw std::runtime_error(s);
            hasDecimals = true;
        }
        else if(!isdigit(c))
            throw std::runtime_error(s);
    }
    return std::stof(s);
}

std::vector<float> loadParams(const std::string &s, uint &i)
{
    std::vector<float> vals;
    std::string param;
    try{
    while(LSPARSE::getNextParam(s,i,param))//the last one...
    {
        vals.emplace_back(loadNumber(param));
        param.clear();
    }
    //vals.emplace_back(loadNumber(param));
    }
    catch(std::exception &e)//make custom type to catch again...
    {
        throw std::runtime_error("Invalid Parameter "+std::string(e.what()));//custom type exceptions???
    }//HMMMMMMMMMM
//    if(s[i] != ')')
//        throw std::runtime_error("Unclosed bracket");
    return vals;
}

//A or B(x,y,z)
LSYSTEM::LModule loadModule(const std::string &s, uint &i, std::vector<float> &vals)
{
    vals.clear();
    char id = s[i];
    //char c_next;
    //if(LSPARSE::next(s,i,c_next) && c_next == '(')
        vals = std::move(loadParams(s,i));//++i));
    return LSYSTEM::LModule(id, &vals[0], vals.size());//vals is destroyed
}

LSYSTEM::LSentence loadLSentence(const std::string &s)
{
    std::vector<float> vals;
    LSYSTEM::LSentence L;
    for(uint i = 0; i < s.size(); ++i)
    {
        L.push_back(loadModule(s,i,vals));
    }

    return L;
}