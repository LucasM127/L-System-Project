#include "ProductionParser.hpp"

#include "LSParseFuncs.hpp"
#include "RPNTokenize.hpp"

void LSDataParser::parse(const LSYSTEM::LSData &lsData)
{
    loadSkippableLetters(lsData.skippableLetters, data.skippableLetters);
    data.globalMap = lsData.globalMap;
    loadProductionDatas(lsData.productions,    data.productionDatas,              data.abc);
    loadProductionDatas(lsData.decompositions, data.decompositionProductionDatas, data.abc);
    loadProductionDatas(lsData.homomorphisms,  data.homomorphicProductionDatas,   data.abc);

    assertAlphabet(data.abc);//
}

void LSDataParser::loadProductionDatas(const std::vector<std::string> &productionStrings, std::vector<LSYSTEM::ProductionData> &productionDatas, LSYSTEM::Alphabet &abc)
{//OK
    for(auto &productionString : productionStrings)
    {
        LSYSTEM::ProductionData pd;
        pd.products.push_back(LSYSTEM::ProductData());

try
{
        decomposeProductionString(productionString, pd);

        unsigned int index = 0;
        
        pd.lContext = parseContextString(pd.lContext, index, pd.products[0].varIndiceMap, abc);
        pd.letter   = parseContextString(pd.letter  , index, pd.products[0].varIndiceMap, abc);
        pd.rContext = parseContextString(pd.rContext, index, pd.products[0].varIndiceMap, abc);

        if(pd.letter.size() != 1)
            throw std::runtime_error(pd.products[0].rawStatement + " doesn't have a singular predicate letter.");
        
        pd.products[0].product = getProductEvalStrings(pd.products[0].product, pd.products[0].evalStrings, abc);
}
catch(std::runtime_error &e)
{
    throw(std::runtime_error("Error in production: " + productionString + "\n" + e.what()));
}
        
        if(!stochasticMatch(productionDatas, pd))
            productionDatas.push_back(pd);
    }

    sortProductions(productionDatas);
}
//#include "../Containers/OstreamOperators.hpp"
//Sorts 
void LSDataParser::sortProductions(std::vector<LSYSTEM::ProductionData>& productions)
{
    //std::cout<<"Before sorting productions:\n";
    //for(auto &pd : productions) std::cout<<pd<<"\n";

    std::vector<LSYSTEM::ProductionData> sortedProductions;

    auto it = productions.begin();
    while(it!=productions.end())
    {
        if((*it).lContext.size()&&(*it).rContext.size())
        {
            sortedProductions.push_back(*it);
            it = productions.erase(it);
            //--it;
        }
        else ++it;
    }
    //only one of the contexts exist?
    it = productions.begin();
    while(it!=productions.end())
    {
        if((*it).lContext.size()||(*it).rContext.size())
        {
            sortedProductions.push_back(*it);
            it = productions.erase(it);
            //--it;
        }
        else ++it;
    }

    for(auto &pd : productions) sortedProductions.push_back(pd);

    productions = sortedProductions;
    //std::cout<<"After sorting productions:\n";
    //for(auto &pd : productions) std::cout<<pd<<"\n";
}

void LSDataParser::loadSkippableLetters(const std::string &skippableString, std::set<char> &skippableLetters)
{
    for(char c: skippableString)
    {
        if(LSPARSE::isWhiteSpace(c)) continue;
        skippableLetters.insert(c);
    }
}

bool LSDataParser::stochasticMatch(std::vector<LSYSTEM::ProductionData>& productions, LSYSTEM::ProductionData& p)
{
    for(auto& pd : productions)
    {
        if( (pd.letter == p.letter) && (pd.lContext == p.lContext) && (pd.rContext == p.rContext) )
        {
            pd.products.push_back(p.products[0]);
            return true;
        }
    }
    return false;
}

//String to be of form LCONTEXT '<' PREDICATE '>' RCONTEXT ':' CONDITIONAL_EXP '=>' PRODUCT ':' STOCHASTIC_WEIGHT
void LSDataParser::decomposeProductionString(const std::string &productionString, LSYSTEM::ProductionData &pd)
{
    using namespace LSPARSE;

    std::string testSentence = productionString;
    std::size_t stringPos;
    LSYSTEM::ProductData &product = pd.products[0];
    product.rawStatement = productionString;

    removeSpaces(testSentence);
    //Not here, could have 'bracket' in the context
//    if(!bracketsMatch(testSentence))
//        throw std::runtime_error("Mismatching brackets.");

    //Get Product & Stochastic Weight
    {
        std::string tempProductString;
        stringPos = testSentence.find("=>");
        if(stringPos == std::string::npos)
        {
            std::string errorString;
            errorString = "'" + productionString + "' doesn't produce anything!  Missing '=>'\n";
            throw std::runtime_error(errorString);
        }
        tempProductString = testSentence.substr(stringPos+2);//from the position to the end copied over
        testSentence.erase(stringPos);

        //check for ':' for stochastic Weight expression / value in our product
        std::string tempProductStochasticWeightString;
        stringPos = tempProductString.find(":");
        if(stringPos != std::string::npos)
        {
            tempProductStochasticWeightString = tempProductString.substr(stringPos+1);
            tempProductString.erase(stringPos);
        }

        product.product = tempProductString;
        product.productWeight = tempProductStochasticWeightString;
    }//this holds the stochastic weight here above

    //Get Conditional Expression Statement
    {
        std::string tempConditionalString;
        stringPos = testSentence.find(":");
        if(stringPos != std::string::npos)
        {
            tempConditionalString = testSentence.substr(stringPos+1);
            testSentence.erase(stringPos);
        }
        product.conditional = tempConditionalString;
    }

    //Get Right Context
    {
        std::string tempRContextString;
        stringPos = testSentence.find(">");
        if(stringPos != std::string::npos)
        {
            tempRContextString = testSentence.substr(stringPos+1);
            testSentence.erase(stringPos);
        }
        pd.rContext = tempRContextString;
    }

    //Get Left Context and Production Letter
    {
        std::string tempLContextString;
        std::string tempProductLetterString;
        stringPos = testSentence.find("<");
        if(stringPos != std::string::npos)
        {
            tempProductLetterString = testSentence.substr(stringPos+1);
            testSentence.erase(stringPos);
            tempLContextString = testSentence;
        }
        else tempProductLetterString = testSentence;
        pd.letter = tempProductLetterString;
        pd.lContext = tempLContextString;
        //if(getLetters(pd.predicate).size() != 1) throw std::runtime_error("Production " + productionString + " can only contain a single Predicate Letter");
    }

    if(!bracketsMatch(product.product) || !bracketsMatch(product.conditional) || !bracketsMatch(product.productWeight))
        throw std::runtime_error("Mismatching brackets.");//?
}

std::string LSDataParser::parseContextString(const std::string &contextString, unsigned int &varIndex, VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &abc)
{
    std::string letters;
    std::string paramString;
    unsigned int i = -1;
    char c_next;
    while(LSPARSE::next(contextString, i, c_next))
    {
        ++i;
        char cur_letter = c_next;
        letters.push_back(c_next);

        unsigned int paramNum = 0;
        while(LSPARSE::getNextParam(contextString, i, paramString))
        {
            if(paramString.size() == 0);//maybe intentional A(,x) only care about the second variable
            else if(paramString.size() > 1)
                throw std::runtime_error("Unrecognized variable \'"+paramString+"\' in context");
            else
            {
                char token = paramString[0];
                if(varIndiceMap.find(token) != varIndiceMap.end())
                    throw std::runtime_error("Can only use variable id \'"+paramString+"\' once in the context");//or something like that
                varIndiceMap[token] = VarIndice(varIndex, paramNum);
            }
            ++paramNum;
            paramString.clear();
        }
        ++varIndex;

        //update alphabet
        if(abc.find(cur_letter) == abc.end())
            abc[cur_letter] = paramNum;
        else
        {
            if((abc.at(cur_letter) != paramNum) && (paramNum != 0))
                throw std::runtime_error("Letter '" + std::string(&cur_letter,1) + "' should have " 
                    + std::to_string(abc.at(cur_letter)) + " parameters.");
        }
    }

    return letters;
}

std::string LSDataParser::getProductEvalStrings(const std::string &rawProductString,std::vector<std::vector<std::string> > &evalStrings, LSYSTEM::Alphabet &abc)
{
    std::string letters;
    std::string paramString;
    unsigned int i = -1;
    char c_next;
    while(LSPARSE::next(rawProductString, i, c_next))
    {
        ++i;
        evalStrings.push_back(std::vector<std::string>());
        char cur_letter = c_next;
        letters.push_back(c_next);
        unsigned int paramNum = 0;
        while(LSPARSE::getNextParam(rawProductString, i, paramString))
        {
            if(paramString.size() == 0)
                throw std::runtime_error("No Rule to generate parameter(s) in letter \'" + std::string(&cur_letter,1) + "\'");
            evalStrings.back().push_back(paramString);
            ++paramNum;
            paramString.clear();
        }

        //update alphabet
        if(abc.find(cur_letter) == abc.end())
            abc[cur_letter] = paramNum;
        else
        {
            if((abc.at(cur_letter) != paramNum) && (abc.at(cur_letter) != 0))
                throw std::runtime_error("Letter '" + std::string(&cur_letter,1) + "' should have " 
                    + std::to_string(abc.at(cur_letter)) + " parameters.");
        }
    }

    return letters;
}

void LSDataParser::assertAlphabet(const LSYSTEM::Alphabet &abc)
{
    for(auto &ppd : data.productionDatas)
        for(auto &pd : ppd.products)
            for(unsigned int i = 0; i < pd.product.size(); ++i)
            {
                if(abc.at(pd.product[i]) != pd.evalStrings[i].size())
                    throw std::runtime_error("Wrong number of parameters for letter '" + std::string(&pd.product[i],1) + "' in production " + pd.rawStatement);
            }
    for(auto &ppd : data.decompositionProductionDatas)
        for(auto &pd : ppd.products)
            for(unsigned int i = 0; i < pd.product.size(); ++i)
            {
                if(abc.at(pd.product[i]) != pd.evalStrings[i].size())
                    throw std::runtime_error("Wrong number of parameters for letter '" + std::string(&pd.product[i],1) + "' in production " + pd.rawStatement);
            }
    for(auto &ppd : data.homomorphicProductionDatas)
        for(auto &pd : ppd.products)
            for(unsigned int i = 0; i < pd.product.size(); ++i)
            {
                if(abc.at(pd.product[i]) != pd.evalStrings[i].size())
                    throw std::runtime_error("Wrong number of parameters for letter '" + std::string(&pd.product[i],1) + "' in production " + pd.rawStatement);
            }
}