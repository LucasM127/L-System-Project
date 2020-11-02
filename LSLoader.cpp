#include "LSLoader.hpp"

#include <fstream>
#include <algorithm>
#include "Containers.hpp"
#include <iostream>

namespace LSYSTEM
{

//load next character from index into c
bool next(const std::string &string, const unsigned int index, char &c)
{
    if(index+1 < string.size())
    {
        c = string[index+1];
        return true;
    }
    return false;
}

int getNumParams(const std::string &string, const unsigned int index, int &lastIndex)
{
    char c_next;
    int numParams = 0;
    lastIndex = index;
    
    if(!next(string, index, c_next)) return numParams;
    if(c_next != '(') return numParams;
    int nextIndex = index + 1;
    numParams++;
    while(next(string, nextIndex, c_next))
    {
        lastIndex = nextIndex+1;
        nextIndex++;
        if(c_next == ')') return numParams;
        if(c_next == ',') numParams++;
    }
    return -1;//error
}

LSLoader::LSLoader(){}

//http://bits.mdminhazulhaque.io/cpp/find-and-replace-all-occurrences-in-cpp-string.html
void LSLoader::applyDefines(std::string& source)
{
    std::string findString;
    std::string replaceString;
    for(auto define : m_defineMapping)
    {
        findString = define.first;
        replaceString = define.second;
        for(std::string::size_type i = 0; (i = source.find(findString, i)) != std::string::npos;)
        {
            source.replace(i, findString.length(), replaceString);
            i += replaceString.length();
        }
    }
}

void LSLoader::removeSpaces(std::string& s)
{
    s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
}

void LSLoader::prepSyntax()
{
    applyDefines(m_tempLine);
    removeSpaces(m_tempLine);
}

//continues returning the line till we hit eof or hit the next ID tagLine
bool LSLoader::getNextLine()
{
    do{
        m_lineNum++;
        if(!std::getline(m_stream,m_tempLine)) return false;
        if(m_tempLine.size() && m_tempLine.at(m_tempLine.size()-1) == '\r')
            m_tempLine.pop_back();
        if(m_tempLine == m_taglines[m_tagIndex])//trying to read out of bounds???
        {//should be from 0 -> 3
            //if(m_tagIndex < 3) m_tagIndex++;//that's the problem
            return false;
        }
    } while(m_tempLine.empty() || (m_tempLine[0] == ';') );

    return true;
}

void LSLoader::assertTagLine(const std::string& tagLine)
{
    if(m_tempLine != tagLine)
    {
        std::string error = "Missing tagline. Want '";
        error += tagLine;
        error += "' instead of '";
        error += m_tempLine;
        error += "'";
        throw std::runtime_error(error);
    }
    if(m_tagIndex < 4)
        m_tagIndex++;
}

void LSLoader::ensureBracketsMatch(const std::string& sentence)
{
    int numCurvedBrackets = 0;
    int numSquareBrackets = 0;
    for(char c : sentence)
    {
        if(!amParametric) if(c == '(') amParametric = true;
        if(c == '(') numCurvedBrackets++;
        else if(c == '[') numSquareBrackets++;
        else if(c == ')') numCurvedBrackets--;
        else if(c == ']') numSquareBrackets--;
    }

    if(numCurvedBrackets || numSquareBrackets)
    {
        std::string error = "Mismatching brackets in line " + std::to_string(m_lineNum) + ": ";
        error += sentence;
        throw std::runtime_error(error);
    }

}

void LSLoader::getProductionLines(std::vector<std::string>& P)
{
    while(m_stream)
    {
        if(!getNextLine()) break;
        prepSyntax();
        ensureBracketsMatch(m_tempLine);

        P.push_back(m_tempLine);
    }
}

LSystem *LSLoader::loadFile(const std::string &fileName)
{
    m_paramNumMap.clear();
    m_tagIndex = 0;
    amParametric = false;
    m_lineNum = -1;

    m_stream.open(fileName);
    if(!m_stream) throw std::runtime_error("Unable to open " + fileName);

    //Get Defines
    while(true)
    {
        if(!getNextLine()) break;
        applyDefines(m_tempLine);

        std::stringstream sstream;
        std::string define, replacement;
        sstream<<m_tempLine;

        sstream>>define;
        if(define.size()==0) break;
        if(define != "#define")
        {
            std::string tempString = "#defines only before 'Axiom:' tagline, not ";
            tempString.append(define);
            throw std::runtime_error(tempString);
        }
        sstream>>define;
        sstream>>replacement;

        m_defineMapping[define] = replacement;
    }

    //Axiom TagLine
    assertTagLine(m_taglines[m_tagIndex]);
    getNextLine();//the axiom line
    prepSyntax();
    m_axiom = m_tempLine;
    ensureBracketsMatch(m_axiom);
    fillParamNumMap(m_axiom);

    //Skippable Tagline
    while(getNextLine());
    assertTagLine(m_taglines[m_tagIndex]);
    prepSyntax();
    if(getNextLine())
    {
        removeSpaces(m_tempLine);
        for(char c: m_tempLine) m_skippableLetters.insert(c);
        getNextLine();
    }

    //Productions Tagline
    //while(getNextLine());
    assertTagLine(m_taglines[m_tagIndex]);
    getProductionLines(m_productionStrings);

    //Decomposition Productions Tagline
    assertTagLine(m_taglines[m_tagIndex]);
    getProductionLines(m_decompositionProductionStrings);

    //Homomorphic Productions Tagline
    //getNextLine();
    assertTagLine(m_taglines[m_tagIndex]);
    getProductionLines(m_homomorphicProductionStrings);

    //Fill the productiondata structs from our strings
    std::vector<ProductionData> productionDatas;
    std::vector<ProductionData> decompositionProductionDatas;
    std::vector<ProductionData> homomorphicProductionDatas;

    ProductionData tempData;//or maybe just clear it out before loading it... totally..
    if(amParametric)
    {
        for(std::string &productionString : m_productionStrings)
        {
            try
            {
                getParametricProductionData(productionString,tempData);//
                if(!stochasticMatch(productionDatas,tempData)) productionDatas.push_back(tempData);//is not pushing it back i don't get it.
            }
            catch(std::runtime_error& error)
            {
                std::string errorString = error.what();
                errorString.append(productionString);
                throw std::runtime_error(errorString);
            }
        }
        for(std::string productionString : m_decompositionProductionStrings)
        {
            try
            {
                getParametricProductionData(productionString,tempData);//fills up tempData
                if(!stochasticMatch(decompositionProductionDatas,tempData)) decompositionProductionDatas.push_back(tempData);
            }
            catch(std::runtime_error& error)
            {
                std::string errorString = error.what();
                errorString.append(productionString);
                throw std::runtime_error(errorString);
            }
            if(tempData.lContext.size()||tempData.rContext.size()) throw std::runtime_error("Decomposition productions are non-contextual!");//HMMMM
        }//copypasta
        for(std::string productionString : m_homomorphicProductionStrings)
        {
            try
            {
                getParametricProductionData(productionString,tempData);//fills up tempData
                if(!stochasticMatch(homomorphicProductionDatas,tempData)) homomorphicProductionDatas.push_back(tempData);
            }
            catch(std::runtime_error& error)
            {
                std::string errorString = error.what();
                errorString.append(productionString);
                throw std::runtime_error(errorString);
            }
            if(tempData.lContext.size()||tempData.rContext.size()) throw std::runtime_error("Homomorphic productions are non-contextual!");//HMMMM
        }
    }
    else//the same
    {
        for(std::string productionString : m_productionStrings)
        {//fillProductionData/fillParametricProductionData
            try
            {
                getProductionData(productionString,tempData);
                if(!stochasticMatch(productionDatas,tempData)) productionDatas.push_back(tempData);
            }
            catch(std::runtime_error& error)
            {
                std::string errorString = error.what();
                errorString.append(productionString);
                throw std::runtime_error(errorString);
            }
        }
        for(std::string productionString : m_decompositionProductionStrings)
        {
            try
            {
                getProductionData(productionString,tempData);
                if(!stochasticMatch(decompositionProductionDatas,tempData)) decompositionProductionDatas.push_back(tempData);
                else if(tempData.lContext.size()||tempData.rContext.size()) throw std::runtime_error("Decomposition productions are non-contextual!");
            }
            catch(std::runtime_error& error)
            {
                std::string errorString = error.what();
                errorString.append(productionString);
                throw std::runtime_error(errorString);
            }
        }
        for(std::string productionString : m_homomorphicProductionStrings)
        {
            try
            {
                getProductionData(productionString,tempData);
                if(!stochasticMatch(homomorphicProductionDatas,tempData)) homomorphicProductionDatas.push_back(tempData);
                else if(tempData.lContext.size()||tempData.rContext.size()) throw std::runtime_error("Homomorphic productions are non-contextual!");
            }
            catch(std::runtime_error& error)
            {
                std::string errorString = error.what();
                errorString.append(productionString);
                throw std::runtime_error(errorString);
            }
        }
    }

    //Sort productions so applies properly
    sortProductions(productionDatas);
    sortProductions(decompositionProductionDatas);
    sortProductions(homomorphicProductionDatas);

    //production pointers??? (so can access what production the product is from when dealing with the product)
    for(auto &production : productionDatas) for(auto &product : production.products) product.production = &production;
    for(auto &production : decompositionProductionDatas) for(auto &product : production.products) product.production = &production;
    for(auto &production : homomorphicProductionDatas) for(auto &product : production.products) product.production = &production;
    //create productionMaps

    //and create the appropiate LSystem...
    //Just the paramnummap is left :/
    //create the shit here... and move it to our constructor.

    //NOW JUST PRINT IT OUT
    std::cout<<"Done parsing "<<fileName<<"\n";
    std::cout<<"Axiom is: "<<m_axiom<<"\n";
    std::cout<<"Skippable letters were: ";
        for(auto &c : m_skippableLetters) std::cout<<c<<' ';
    std::cout<<"\n";
    std::cout<<"Productions were:\n";
    for(auto &pd : productionDatas)
    {
        std::cout<<pd.rawStatement<<"\n";
        std::cout<<pd.lContext<<" < "<<pd.letter<<" > "<<pd.rContext<<"\n";
        std::cout<<"With products :\n";
        for(auto &p : pd.products)
        {
            std::cout<<"product: "<<p.product<<" weight: "<<p.productWeight<<" condition: "<<p.conditionalExpression<<" evals: \n";
            for(auto &e : p.evalStrings)
            { 
                for(auto &e2 : e)
                    std::cout<<e2<<"_ ";
                std::cout<<"\n";
            }
            std::cout<<"\n";
        }
        std::cout<<"and varindice mapping: ";
        for(auto &m : pd.varIndiceMap)
        {
            std::cout<<m.first<<"-"<<m.second.letterNum<<"x"<<m.second.paramNum<<" ";
        }
        std::cout<<"\n\n";
    }

    std::cout<<"Homomorphic Productions were:\n";
    for(auto &pd : homomorphicProductionDatas)
    {
        std::cout<<pd.rawStatement<<"\n";
        std::cout<<pd.lContext<<" < "<<pd.letter<<" > "<<pd.rContext<<"\n";
        std::cout<<"With products :\n";
        for(auto &p : pd.products)
        {
            std::cout<<"product: "<<p.product<<" weight: "<<p.productWeight<<" condition: "<<p.conditionalExpression<<" evals: \n";
            for(auto &e : p.evalStrings)
            { 
                for(auto &e2 : e)
                    std::cout<<e2<<"_ ";
                std::cout<<"\n";
            }
            std::cout<<"\n";
        }
        std::cout<<"and varindice mapping: ";
        for(auto &m : pd.varIndiceMap)
        {
            std::cout<<m.first<<"-"<<m.second.letterNum<<"x"<<m.second.paramNum<<" ";
        }
        std::cout<<"\n\n";
    }

    std::cout<<"Alphabet: ";
    for(auto &E : m_paramNumMap) std::cout<<E.first<<"-"<<E.second<<" ";
    std::cout<<std::endl;

    /*
    if(amParametric)
    {
        ParametricLSystem* plsystem = new ParametricLSystem(axiom,skippableLetters,productionDatas,homomorphicProductionDatas,paramNumMap);
        return plsystem;
    }
    else
    {
        LSystem* lsystem = new LSystem(axiom,skippableLetters,productionDatas,homomorphicProductionDatas);
        return lsystem;
    }
    */
    if(amParametric)
        return new PLSystem(m_axiom, m_skippableLetters, productionDatas, decompositionProductionDatas, homomorphicProductionDatas, m_paramNumMap);
    return new LSystem(m_axiom, m_skippableLetters, productionDatas, decompositionProductionDatas, homomorphicProductionDatas);
}

std::string LSLoader::getLetters(const std::string& sentence)
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

//String to be of form LCONTEXT '<' PREDICATE '>' RCONTEXT ':' CONDITIONAL_EXP '=>' PRODUCT ':' STOCHASTIC_WEIGHT
void LSLoader::getProductionData(const std::string& productionString, ProductionData& pd)
{
    pd.products.clear();

    pd.rawStatement = productionString;

    std::string testSentence = productionString;
    std::size_t stringPos;

    ProductData tempProductData;

    //Get Product & Stochastic Weight
    {
        std::string tempProductString;
        stringPos = testSentence.find("=>");
        if(stringPos == std::string::npos)
        {
            std::stringstream s;
            s<<"Production '"<<productionString<<"' doesn't Produce anything!  Missing '=>'"<<std::endl;
            throw std::runtime_error(s.str());
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

        tempProductData.product = tempProductString;
        tempProductData.productWeight = tempProductStochasticWeightString;
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
        tempProductData.conditionalExpression = tempConditionalString;
    }

    pd.products.push_back(tempProductData);

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
        if(getLetters(pd.letter).size() != 1) throw std::runtime_error("Production " + productionString + " can only contain a single Predicate Letter");
    }
}

//for each new tempdata...
void LSLoader::getParametricProductionData(const std::string& productionString, ProductionData& ppd)
{
    getProductionData(productionString,ppd);
    ppd.varIndiceMap.clear();

    //fill Var Map
    int index = 0;
    if(ppd.lContext.size()) fillVarMap(ppd.lContext,index,ppd.varIndiceMap);
    fillVarMap(ppd.letter,index,ppd.varIndiceMap);
    if(ppd.rContext.size()) fillVarMap(ppd.rContext,index,ppd.varIndiceMap);

    getProductEvalStrings(ppd.products[0].product,ppd.products[0].evalStrings);

    //simplify
    ppd.lContext = getLetters(ppd.lContext);
    ppd.letter = getLetters(ppd.letter);
    ppd.rContext = getLetters(ppd.rContext);
    ppd.products[0].product = getLetters(ppd.products[0].product);
}

bool LSLoader::stochasticMatch(std::vector<ProductionData>& productions, ProductionData& p)
{
    for(ProductionData& pd : productions)
    {
        //stochastic match if contexts match... already simplified
        if( (pd.letter == p.letter) && (pd.lContext == p.lContext) && (pd.rContext == p.rContext) )// && (pd.condition == p.condition))
        {
            pd.products.push_back(p.products[0]);

            return true;
        }
    }
    return false;
}

void LSLoader::sortProductions(std::vector<ProductionData>& productions)
{
    /*
    auto sorter = [](const ProductionData &L, const ProductionData &R) -> bool
    {
        if(L.lContext.size() && L.rContext.size())
        {
            if(!R.lContext.size()) return true;
            if(!R.rContext.size()) return true;
            int lsize = L.lContext.size() + L.rContext.size();
            return false;
        }
        return false;
    };
*/
    //std::sort(productions.begin(), productions.end(), sorter);

    //sort by existance of a context, then a conditional, then neither.. not conditional now
    std::vector<ProductionData> sortedProductions;
    //both contexts exist?
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
    /***
    //has a conditional?
    it = productions.begin();
    while(it!=productions.end())
    {
        if((*it)->condition.size())
        {
            sortedProductions.push_back(*it);
            productions.erase(it);
            it--;
        }
        it++;
    }
    */
    //then remainder
    for(ProductionData pd : productions) sortedProductions.push_back(pd);

    productions = sortedProductions;
}

//PARAMETRIC SHIT
//this is in the context A(x)<A>A(y) => A(x+y/2) should work, we already know A has parameters, don't need to type it out all the time.  but only if 0
//used in lcontext, rcontext center context...
void LSLoader::fillParamNumMap(const std::string& string)
{
    int index = -1;
    char c;
    int lastIndex = index;
    while(next(string, index, c))
    {
        index++;
        char curLetter = c;
        unsigned int numParams = getNumParams(string, index, lastIndex);
        if(m_paramNumMap.find(curLetter) != m_paramNumMap.end())
        {
            if(m_paramNumMap[curLetter] < numParams)
                m_paramNumMap[curLetter] = numParams;
        }
        else m_paramNumMap[curLetter] = numParams;
        index = lastIndex;
    }

}

void LSLoader::fillVarMap(const std::string &contextString, int &varIndex, VarIndiceMapping& varIndiceMap)
{
    if(contextString.size()==0) return;
    char curLetter;
    int varNum = 0;//ie letterNum
    int i = 0;
    curLetter = contextString[i];
    char c_next = curLetter;
    next(contextString, i, c_next);
    unsigned int numParams = 0;//!!!!!!!!!!!!
    int nextIndex = i+1;
    do
    {
        curLetter = contextString[i];
        unsigned int paramNum = 0;
        if(c_next == '(')
        {
            paramNum++;
            while(next(contextString, nextIndex, c_next))
            {
                nextIndex++;
                if(c_next == ')') break;
                if(c_next == ',')
                {
                    paramNum++;
                    continue;
                }
                //load the 'TOKEN'
                char Token = c_next;
                if(varIndiceMap.find(Token) != varIndiceMap.end())
                {
                    std::string temp;
                    std::stringstream s;
                    s<<"Using variable \'"<<Token<<"\' multiple times in production: ";//will catch and rethrow
                    temp = s.str();
                    throw std::runtime_error(temp);
                }
                varIndiceMap[Token] = VarIndice(varNum + varIndex, paramNum - 1);
            }
        }
        numParams = paramNum;
        if(m_paramNumMap.find(curLetter) != m_paramNumMap.end())
        {
            if(m_paramNumMap[curLetter] < numParams)
                m_paramNumMap[curLetter] = numParams;
        }
        else m_paramNumMap[curLetter] = numParams;
        varNum++;
        i = nextIndex;
    } while(next(contextString, i, c_next));
    varIndex += varNum;
}

//don't worry about error... will find out when we create the productions
//buggy here ?
//FIX IT
void LSLoader::getProductEvalStrings(const std::string& rawProductString,std::vector<std::vector<std::string> >& evalStrings)
{
    if(rawProductString.size() == 0) return;
    evalStrings.resize(getLetters(rawProductString).size());
    std::vector<std::string> productExpressions;
    unsigned int l_index = 0;
    unsigned int i = 0;
    while (true)
    {
        char curLetter = rawProductString[i];
        //load params...
        unsigned int curParamNum = 0;
        char c_next;
        if(next(rawProductString, i, c_next) && c_next == '(')
        {
            curParamNum++;
            //load the parameters...
            std::string exp;
            i++;
            unsigned int curLvl = 0;
            while (next(rawProductString, i , c_next))//peekNext?
            {
                i++;
                if(c_next == '(') curLvl++;
                if(c_next == ')' && curLvl > 0) curLvl--;
                else if(c_next == ')')
                {
                    if(exp.size() == 0)
                    {
                        throw std::runtime_error("No Rule to generate parameter " + std::to_string(curParamNum) + " in letter " + (curLetter) + " ");
                    }
                    evalStrings[l_index].push_back(exp);
                    exp.clear();
                    break;
                }
                if(c_next == ',' && curLvl == 0)
                {
                    if(exp.size() == 0)
                    {
                        throw std::runtime_error("No Rule to generate parameters " + std::to_string(curParamNum) + " in letter " + (curLetter) + " ");
                    }
                    evalStrings[l_index].push_back(exp);
                    curParamNum++;
                    exp.clear();
                    continue;
                }
                exp.push_back(c_next);
            }//when leaves i at the ')' index
            if(m_paramNumMap.find(curLetter) != m_paramNumMap.end())
            {
                unsigned int mappedNumParams = m_paramNumMap[curLetter];
                if(mappedNumParams == 0) m_paramNumMap[curLetter] = curParamNum;
                else if(mappedNumParams != curParamNum)
                    throw std::runtime_error("Letter " + std::to_string(curLetter) + " has inconsistent parameter size");
            }
            else m_paramNumMap[curLetter] = curParamNum;
        }
        l_index++;
        i++;
        if(i >= rawProductString.size()) break;
    }
}

} //namespace LSYSTEM