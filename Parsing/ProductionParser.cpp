#include "ProductionParser.hpp"

#include "LSParseFuncs.hpp"

void LSDataParser::parse(const LSYSTEM::LSData &lsData)
{
    mapGlobals(lsData.globals);
    skippableLetters = lsData.skippableLetters;//"Redundant?"  "akylgk";//maybe want to m ove over ....
    loadProductionDatas(lsData.productions,    productionDatas,              alphabet);
    loadProductionDatas(lsData.decompositions, decompositionProductionDatas, alphabet);
    loadProductionDatas(lsData.homomorphisms,  homomorphicProductionDatas,   alphabet);

    assertAlphabet();
}

void LSDataParser::mapGlobals(const std::vector<std::pair<std::string, float> > &globalPairs)
{
    //give unique token id... local to this LSystem
    char globalId = 128;
    for(auto &pair : globalPairs)
    {
        globalVarMap[globalId] = pair.second;
        globalNameMap[pair.first] = globalId;
        ++globalId;
    }
}

void LSDataParser::loadProductionDatas(const std::vector<std::string> &productionStrings, std::vector<LSYSTEM::ProductionData> &productionDatas, LSYSTEM::Alphabet &abc)
{
    for(auto &productionString : productionStrings)
    {
        LSYSTEM::ProductionData pd;
        pd.products.push_back(LSYSTEM::ProductData());

try
{
        decomposeProductionString(productionString, pd);

        uint index = 0;
        //???? AND SO IS THIS?
        pd.lContext = parseContextString(pd.lContext, index, pd.products[0].varIndiceMap, abc);
        pd.letter   = parseContextString(pd.letter  , index, pd.products[0].varIndiceMap, abc);
        pd.rContext = parseContextString(pd.rContext, index, pd.products[0].varIndiceMap, abc);
//hmmm
        //fillVarMap(pd.lContext,index,pd.products[0].varIndiceMap,abc);
        //fillVarMap(pd.letter,index,pd.products[0].varIndiceMap,abc);
        //fillVarMap(pd.rContext,index,pd.products[0].varIndiceMap,abc);

        //now I can simplify...
        //pd.lContext = LSPARSE::getLetters(pd.lContext);
        //pd.letter = LSPARSE::getLetters(pd.letter);
        //pd.rContext = LSPARSE::getLetters(pd.rContext);
        if(pd.letter.size() != 1)
            throw std::runtime_error(pd.products[0].rawStatement + " doesn't have a singular predicate letter.");
        //????? THIS IS THE ISSUE
        pd.products[0].product = getProductEvalStrings(pd.products[0].product,pd.products[0].evalStrings, abc);
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
            productions.erase(it);
            it--;
        }
        it++;
    }
    //only one of the contexts exist?
    it = productions.begin();
    while(it!=productions.end())
    {
        if((*it).lContext.size()||(*it).rContext.size())
        {
            sortedProductions.push_back(*it);
            productions.erase(it);
            it--;
        }
        it++;
    }

    for(auto &pd : productions) sortedProductions.push_back(pd);

    productions = sortedProductions;
    //std::cout<<"After sorting productions:\n";
    //for(auto &pd : productions) std::cout<<pd<<"\n";
}


/*
void LSDataParser::loadSkippableLetters(const std::string &skippableString, std::set<char> &skippableLetters)
{
    for(char c: skippableString)
    {
        if(c == ' ') continue;
        skippableLetters.insert(c);
    }
}*/

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

//Check this code....
std::string LSDataParser::parseContextString(const std::string &contextString, uint &varIndex, VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &abc)
{
    std::string letters;
    uint i = -1;
    char c_next;
    while(LSPARSE::next(contextString, i, c_next))
    {
        ++i;
        ++varIndex;
        char cur_letter = c_next;
        letters.push_back(c_next);

        uint paramNum = 0;
        if(LSPARSE::next(contextString, i, c_next) && c_next == '(')
        {
            ++i;
            std::string paramString;//return false???
            while(LSPARSE::getNextParam(contextString,i,paramString))
            {
                if(paramString.size() == 0)
                    continue;//maybe intentional A(,x) only care about the second variable
                if(paramString.size() > 1)
                    throw std::runtime_error("Unrecognized variable \'"+paramString+"\' in context");
                char token = paramString[0];
                if(varIndiceMap.find(token) != varIndiceMap.end())
                    throw std::runtime_error("Can only use variable id \'"+paramString+"\' once in the context");//or something like that
                varIndiceMap[token] = VarIndice(varIndex, paramNum);
                ++paramNum;
                paramString.clear();
            }
            if(paramString.size() > 1)
                throw std::runtime_error("Unrecognized variable \'"+paramString+"\' in context");
            char token = paramString[0];
            if(varIndiceMap.find(token) != varIndiceMap.end())
                throw std::runtime_error("Can only use variable id \'"+paramString+"\' once in the context");//or something like that
            varIndiceMap[token] = VarIndice(varIndex, paramNum);
            ++paramNum;
        }

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
    uint i = -1;
    char c_next;
    while(LSPARSE::next(rawProductString, i, c_next))
    {
        ++i;
        evalStrings.push_back(std::vector<std::string>());
        char cur_letter = c_next;
        letters.push_back(c_next);
        uint paramNum = 0;
        if(LSPARSE::next(rawProductString, i, c_next) && c_next == '(')
        {
            ++i;
            std::string paramString;
            while(LSPARSE::getNextParam(rawProductString,i,paramString))
            {
                if(paramString.size() == 0)
                    throw std::runtime_error("No Rule to generate parameter(s) in letter \'" + std::string(&cur_letter,1) + "\'");
                evalStrings.back().push_back(paramString);
                ++paramNum;
                paramString.clear();
            }
            if(paramString.size() == 0)
                throw std::runtime_error("No Rule to generate parameter(s) in letter \'" + std::string(&cur_letter,1) + "\'");
            evalStrings.back().push_back(paramString);
            ++paramNum;
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

void LSDataParser::assertAlphabet()
{
    for(auto &ppd : productionDatas)
        for(auto &pd : ppd.products)
            for(uint i = 0; i < pd.product.size(); ++i)
            {
                if(alphabet.at(pd.product[i]) != pd.evalStrings[i].size())
                    throw std::runtime_error("Wrong number of parameters for letter '" + std::string(&pd.product[i],1) + "' in production " + pd.rawStatement);
            }
    for(auto &ppd : decompositionProductionDatas)
        for(auto &pd : ppd.products)
            for(uint i = 0; i < pd.product.size(); ++i)
            {
                if(alphabet.at(pd.product[i]) != pd.evalStrings[i].size())
                    throw std::runtime_error("Wrong number of parameters for letter '" + std::string(&pd.product[i],1) + "' in production " + pd.rawStatement);
            }
    for(auto &ppd : homomorphicProductionDatas)
        for(auto &pd : ppd.products)
            for(uint i = 0; i < pd.product.size(); ++i)
            {
                if(alphabet.at(pd.product[i]) != pd.evalStrings[i].size())
                    throw std::runtime_error("Wrong number of parameters for letter '" + std::string(&pd.product[i],1) + "' in production " + pd.rawStatement);
            }
}
/*
void LSDataParser::fillVarMap(const std::string &contextString, uint &varIndex, VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &pnm)
{
    using namespace LSPARSE;

    if(contextString.size()==0)
        return;

    uint varNum = 0;//ie letterNum
    uint i = 0;
    char c_next;
    
    uint numParams = 0;
    uint nextIndex = i+1;
    
    while(next(contextString, i, c_next))
    {
        char curLetter = contextString[i];
        uint paramNum = 0;
        if(c_next == '(')
        {
            while(next(contextString, nextIndex, c_next))
            {
                nextIndex++;
                if(c_next == ')') break;
                if(c_next == ',')
                {
                    ++paramNum;
                    continue;
                }
                //load the 'TOKEN'
                char Token = c_next;
                if(varIndiceMap.find(Token) != varIndiceMap.end())
                    throw std::runtime_error("Using variable \'"+std::string(&Token,1)+"\' multiple times");
                varIndiceMap[Token] = VarIndice(varNum + varIndex, paramNum);
            }
            ++paramNum;//for numParams
        }
        numParams = paramNum;
        if(numParams)
        {
            if( (pnm.find(curLetter) != pnm.end()) && (pnm[curLetter] != numParams) )
                throw std::runtime_error("Letter '" + std::string(&curLetter,1) + "' should have " 
                    + std::to_string(pnm[curLetter]) + " parameters.");
            pnm[curLetter] = numParams;
        }
        else if(pnm.find(curLetter) == pnm.end())
        {
            pnm[curLetter] = numParams;//zero
        }
        varNum++;
        i = nextIndex;
        nextIndex = i+1;
    }
    if(contextString.size()==1)
        varNum++;
    varIndex += varNum;
}*/

/*
//THIS SUCKS
std::string LSDataParser::getProductEvalStrings(const std::string &rawProductString,std::vector<std::vector<std::string> > &evalStrings, LSYSTEM::Alphabet &pnm)
{
    using namespace LSPARSE;

    std::string productString = getLetters(rawProductString);
    if(rawProductString.size() == 0) return productString;

    evalStrings.resize(productString.size());
    std::vector<std::string> productExpressions;
    unsigned int letterIndex = 0;
    unsigned int i = 0;
    while(true)
    {
        char curLetter = rawProductString[i];
        unsigned int curParamNum = 0;
        char c_next;
        if(next(rawProductString, i , c_next) && c_next == '(')
        {
            ++curParamNum;
            //load the parameters
            std::string exp;
            ++i;
            unsigned int curLvl = 0;
            while (next(rawProductString, i, c_next))
            {
                ++i;
                if(c_next == '(') ++curLvl;
                else if(c_next == ')' && curLvl > 0) --curLvl;
                else if(c_next == ')')
                {
                    if(exp.size() == 0)
                    {
                        throw std::runtime_error("No Rule to generate parameter " + std::to_string(curParamNum) + " in letter " + std::string(&curLetter,1) + " ");
                    }
                    evalStrings[letterIndex].push_back(exp);
                    exp.clear();
                    break;
                }
                else if(c_next == ',' && curLvl == 0)
                {
                    if(exp.size() == 0)
                    {
                        throw std::runtime_error("No Rule to generate parameter " + std::to_string(curParamNum) + " in letter " + std::string(&curLetter,1) + " ");
                    }
                    evalStrings[letterIndex].push_back(exp);
                    curParamNum++;
                    exp.clear();
                    continue;
                }
                exp.push_back(c_next);
            }
        }
        //!!!!
        if(pnm.find(curLetter) != pnm.end())
        {
            unsigned int mappedNumParams = pnm[curLetter];
            if(mappedNumParams != curParamNum)
            {
                if(mappedNumParams == 0)
                    pnm[curLetter] = curParamNum;
                else
                    throw std::runtime_error("Letter " + std::string(&curLetter,1) + " has inconsistent parameter size");
            }
        }
        else
            pnm[curLetter] = curParamNum;
        ++letterIndex;
        ++i;
        if(i >= rawProductString.size()) break;
    }

    return productString;
}*/