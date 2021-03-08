#include "LSFileParser.hpp"
#include "ParsingFuncs.hpp"

#include <fstream>
#include <array>

#include <sstream>

void LSParser::parseFile(const std::string &fileName, LSYSTEM::LSFileData &fd, LSYSTEM::LSystemData &lsData)
{
    const std::array<std::string, 7> tagLines = 
    {
        "Globals:", "Axiom:", "Skippable Letters:", "Productions:", "Decomposition Productions:", "Homomorphic Productions:", "Interpreter Parameters:"
    };

    int curLineCtr;
    //separate into 'sections', predetermined order mandatory(?)
    bool passed = true;
    std::string errorString;
    uint curTagIndex = 0;
    std::string curLine;
    curLineCtr = 0;

    std::ifstream ifstream;
    ifstream.open(fileName);
    if(!ifstream) throw std::runtime_error("Unable to open " + fileName);

    auto getNextLine = [&](std::istream &istream, std::string &nextLine, uint &curIndex)->bool
    {
        do
        {
            if(!std::getline(istream,nextLine)) return false;
            ++curLineCtr;
            if(nextLine.size() && nextLine.at(nextLine.size()-1) == '\r')
                nextLine.pop_back();
            for(uint i = 0; i < tagLines.size(); ++i)
                if(nextLine == tagLines[i])
                {
                    //???
                    curIndex = i;
                    return false;
                }
        }
        while(nextLine.empty() || (nextLine[0] == ';') );

        return true;
    };

    fd.clear();

    //load Defines
    while(getNextLine(ifstream, curLine, curTagIndex))
        fd.defines.push_back(curLine);
    if(curTagIndex == 0)//"Globals:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            fd.globals.push_back(curLine);
    }
    if(curTagIndex == 1)//"Axiom:"
    {
        if(getNextLine(ifstream, curLine, curTagIndex))
        {
            fd.axiom = curLine;
            getNextLine(ifstream, curLine, curTagIndex);
        }
        
    }
    if(curTagIndex == 2)//"Skippable Letters:"
    {
        if(getNextLine(ifstream, curLine, curTagIndex))
        {
            fd.skippableLetters = curLine;
            getNextLine(ifstream, curLine, curTagIndex);
        }
    }
    if(curTagIndex == 3)//"Productions:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            fd.productions.push_back(curLine);
    }
    if(curTagIndex == 4)//"Decomposition Productions:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            fd.decompositions.push_back(curLine);
    }
    if(curTagIndex == 5)//"Homomorphic Productions:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            fd.homomorphisms.push_back(curLine);
    }
    if(curTagIndex == 6)//"Interpreter Parameters:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            fd.interpretor.push_back(curLine);
    }

    if(!ifstream.eof())
    {
        passed = false;
        errorString = curLine + " on line " + std::to_string(curLineCtr) + " is an unrecognized tagline.\n";
        errorString += "Recognized taglines are (in order):";
        for(std::size_t i = 0; i < tagLines.size(); ++i)
            errorString += "\n(" + std::to_string(i+1) + ")\t" + tagLines[i];
    }

    ifstream.close();

    if(!passed)
        throw std::runtime_error(errorString);
    
    convertFileData(fd, lsData);
}

void LSParser::convertFileData(LSYSTEM::LSFileData &fd, LSYSTEM::LSystemData &lsData)
{
    //to create the evaluators!
    std::unordered_map<std::string, std::string> defineMapping;
    std::vector<std::pair<char, float> > globalPairs;

    loadDefines(fd.defines, defineMapping);
    applyDefines(fd, defineMapping);

    loadGlobals(fd.globals, globalPairs, defineMapping);
    loadSkippableLetters(fd.skippableLetters, lsData.skippableLetters);
    loadProductionDatas(fd.productions, lsData.productionDatas, lsData.abc);
    loadProductionDatas(fd.decompositions, lsData.decompositionProductionDatas, lsData.abc);
    loadProductionDatas(fd.homomorphisms, lsData.homomorphicProductionDatas, lsData.abc);

    updateAlphabet(fd.axiom,lsData.abc);
}

void LSParser::applyDefines(LSYSTEM::LSFileData &fd, const std::unordered_map<std::string, std::string> &defineMapping)
{
    {
        LSPARSE::removeSpaces(fd.axiom);
        LSPARSE::findAndReplace(fd.axiom, defineMapping);
    }
    for(auto &string : fd.productions)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, defineMapping);
    }
    for(auto &string : fd.decompositions)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, defineMapping);
    }
    for(auto &string : fd.homomorphisms)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, defineMapping);
    }
    for(auto &string : fd.interpretor)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, defineMapping);
    }
}

void LSParser::loadProductionDatas(const std::vector<std::string> &productionStrings, std::vector<LSYSTEM::ProductionData> &productionDatas, LSYSTEM::Alphabet &abc)
{
    for(auto &productionString : productionStrings)
    {
        LSYSTEM::ProductionData pd;
        pd.products.push_back(LSYSTEM::ProductData());

try
{
        decomposeProductionString(productionString, pd);

        uint index = 0;
        
        fillVarMap(pd.lContext,index,pd.products[0].varIndiceMap,abc);
        fillVarMap(pd.letter,index,pd.products[0].varIndiceMap,abc);
        fillVarMap(pd.rContext,index,pd.products[0].varIndiceMap,abc);

        //now I can simplify...
        pd.lContext = getLetters(pd.lContext);
        pd.letter = getLetters(pd.letter);
        pd.rContext = getLetters(pd.rContext);
        if(pd.letter.size() != 1)
            throw std::runtime_error(pd.products[0].rawStatement + " doesn't have a singular predicate letter.");
        
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

void LSParser::sortProductions(std::vector<LSYSTEM::ProductionData>& productions)
{
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
}

//recursively
void LSParser::loadDefines(const std::vector<std::string> &defineStrings, std::unordered_map<std::string, std::string> &defineMapping)
{
    for(auto &string : defineStrings)
    {
        std::stringstream sstream;
        std::string define, replacement;

        sstream << string;
        sstream >> define;

        if(define != "#define")
        {
            std::string errorString = "Invalid string " + string + "\nOnly #define [target] [name] allowed at beginning of ls file.";
            throw std::runtime_error(errorString);
        }

        sstream>>define;

        if(define.size() == 0)
        {
            std::string errorString = "Invalid #define " + string + " with no [target]\nOnly #define [target] [name] allowed at beginning of ls file.";
            throw std::runtime_error(errorString);
        }

        sstream>>replacement;

        LSPARSE::findAndReplace(replacement, defineMapping);
        defineMapping[define] = replacement;
    }   
}

void LSParser::loadGlobals(const std::vector<std::string> &globalStrings, std::vector<std::pair<char, float> > &globalPairs,
                    std::unordered_map<std::string, std::string> &defineMapping)
{
    char globalId = 128;
    for(auto &string : globalStrings)
    {
        //if value is not a value, not a global
        std::stringstream sstream;
        std::string name;
        float value;
        
        sstream << string;
        sstream >> name;

        sstream >> value;
        if(sstream.fail())
        {
            std::string errorString = "Global string " + string + " usage [target] [value] has to have value of numeric type.";
            throw std::runtime_error(errorString);
        }

        defineMapping[name] = globalId;//replace with an untypable variable name
        globalPairs.push_back({globalId, value});
        ++globalId;
    }
}

void LSParser::loadSkippableLetters(const std::string &skippableString, std::set<char> &skippableLetters)
{
    for(char c: skippableString)
    {
        if(c == ' ') continue;
        skippableLetters.insert(c);
    }
}

std::string LSParser::getLetters(const std::string& sentence)
{
    std::string output;
    char c;
    int curLevel = 0;
    for(unsigned int i = 0; i<sentence.size();i++)
    {
        c = sentence[i];
        if(c == ' ') continue;
        if(c != '(') output.push_back(c);
        else while(true)
        {
            i++;
            c = sentence[i];
            if(c == ')')
            {
                //c = sentence[i];
                if(curLevel == 0) break;
                curLevel--;
            }
            else if(c == '(') curLevel++;
        }
    }
    return output;
}

bool LSParser::stochasticMatch(std::vector<LSYSTEM::ProductionData>& productions, LSYSTEM::ProductionData& p)
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
void LSParser::decomposeProductionString(const std::string &productionString, LSYSTEM::ProductionData &pd)
{
    using namespace LSPARSE;

    std::string testSentence = productionString;
    std::size_t stringPos;
    LSYSTEM::ProductData &product = pd.products[0];
    product.rawStatement = productionString;

    removeSpaces(testSentence);
    if(!bracketsMatch(testSentence))
        throw std::runtime_error("Mismatching brackets.");

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
}

void LSParser::updateAlphabet(const std::string &axiom, LSYSTEM::Alphabet &abc)
{
    using namespace LSPARSE;

    char c_next;
    uint i = -1;
    while (next(axiom, i, c_next))
    {
        //read the letter.
        ++i;
        char letter = c_next;
        uint numParams = 0;
        if(next(axiom,i,c_next) && c_next == '(')
        {
            while(next(axiom,i++,c_next))
            {
                if(c_next == ',')
                {
                    ++numParams;
                    continue;
                }
                if(c_next == ')')
                {
                    ++numParams;
                    break;
                }
            }
        }
        if(abc.find(letter) == abc.end())
        {
            abc[letter] = numParams;
        }
        else if(abc.at(letter) != numParams)
            throw std::runtime_error("Invalid num params for letter " + std::string(&letter,0) + " in axiom " + axiom);
    }
}

//and I got to redo this yet again :/ 'Blegh'
void LSParser::fillVarMap(const std::string &contextString, uint &varIndex, VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &pnm)
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
}

std::string LSParser::getProductEvalStrings(const std::string &rawProductString,std::vector<std::vector<std::string> > &evalStrings, LSYSTEM::Alphabet &pnm)
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
}