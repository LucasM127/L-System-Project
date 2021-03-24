#include "ParsingFuncs.hpp"

#include <algorithm>

namespace LSPARSE
{

//reads next character if next character can be read
bool next(const std::string &string, const unsigned int index, char &c)
{
    if(index+1 < string.size())
    {
        c = string[index+1];
        return true;
    }
    return false;
}

//Reads all the digits of a number in a string from certain index i
//if a number is read number holds the value, and i is the index one past the number
bool readNumber(const std::string &string, unsigned int &i, float &number)
{
    char c = string[i];
    std::string tempString;
    bool decimalPoint = false;

    if(c == '.') decimalPoint = true;
    else if(!isdigit(c)) return false;
    tempString.push_back(c);

    while(next(string, i, c))
    {
        if(c == '.')
        {
            if(decimalPoint)
                break;//act as if is another number
            decimalPoint = true;
        }
        else if(!isdigit(c))
            break;
        tempString.push_back(c);
        ++i;
    }

    number = std::stof(tempString);
    
    return true;
}

//http://bits.mdminhazulhaque.io/cpp/find-and-replace-all-occurrences-in-cpp-string.html
void findAndReplace(std::string& source, const std::unordered_map<std::string, std::string> &replacementStrings)
{
    std::string findString;
    std::string replaceString;
    for(auto &entry : replacementStrings)
    {
        findString = entry.first;
        replaceString = entry.second;
        for(std::string::size_type i = 0; (i = source.find(findString, i)) != std::string::npos;)
        {
            source.replace(i, findString.length(), replaceString);
            i += replaceString.length();
        }
    }
}

void findAndReplace(std::string& source, std::vector<std::pair<std::string, char> > &stringReplacementChars)
{
    std::string findString;
    char replaceChar;
    for(auto define : stringReplacementChars)
    {
        findString = define.first;
        replaceChar = define.second;
        for(std::string::size_type i = 0; (i = source.find(findString, i)) != std::string::npos;)
        {
            source.replace(i, findString.length(), &replaceChar, 1);
            ++i;//i += replaceString.length();
        }
    }
}

void removeSpaces(std::string& s)
{
    s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
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

bool ensureBracketsMatch(const std::string& string)
{
    int numCurvedBrackets = 0;
    int numSquareBrackets = 0;
    for(char c : string)
    {
        if(c == '(') ++numCurvedBrackets;
        else if(c == '[') ++numSquareBrackets;
        else if(c == ')') --numCurvedBrackets;
        else if(c == ']') --numSquareBrackets;
    }

    return !(numCurvedBrackets || numSquareBrackets);
}

std::string getLetters(const std::string& sentence)
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

bool bracketsMatch(const std::string& sentence)
{
    int numCurvedBrackets = 0;
    int numSquareBrackets = 0;
    for(char c : sentence)
    {
        if(c == '(') numCurvedBrackets++;
        else if(c == '[') numSquareBrackets++;
        else if(c == ')') numCurvedBrackets--;
        else if(c == ']') numSquareBrackets--;
    }

    return !(numCurvedBrackets || numSquareBrackets);
}

//For use in a while loop.  Loads next param in 'param' while a parameter exists after current letter.
bool getNextParam(const std::string &s, uint &i, std::string &param)
{
    char c_next;
    if(!LSPARSE::next(s,i,c_next))
        return false;
    if(c_next == ')')
    {
        ++i;
        return false;
    }
    if(!(c_next == '(' || c_next == ','))
        return false;

    ++i;
    uint curLvl = 0;
    while(LSPARSE::next(s,i,c_next))
    {
        if(c_next == '(') ++curLvl;
        else if(c_next == ')' && curLvl > 0) --curLvl;
        else if((c_next == ',' || c_next == ')') && (curLvl == 0)) break;
        ++i;
        param.push_back(c_next);
    }
    return true;
}

}// namespace LSPARSE