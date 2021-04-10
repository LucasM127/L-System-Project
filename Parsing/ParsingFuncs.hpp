#ifndef COMMON_PARSING_FUNCTIONS_HPP
#define COMMON_PARSING_FUNCTIONS_HPP
//These do not throw exceptions.

#include <string>
#include <unordered_map>
#include <istream>
#include <vector>

namespace LSPARSE
{

//reads next character if next character can be read
bool next(const std::string &string, const unsigned int index, char &c);

//Reads all the digits of a number in a string from certain index i
//if a number is read number holds the value, and i is the index one past the number
bool readNumber(const std::string &string, unsigned int &i, float &number);//string_view???

bool isNum(const std::string &string, float &number);

bool isWhiteSpace(const char c);

//http://bits.mdminhazulhaque.io/cpp/find-and-replace-all-occurrences-in-cpp-string.html
void findAndReplace(std::string& source, const std::unordered_map<std::string, std::string> &replacementStrings);
void findAndReplace(std::string& source, const std::vector<std::pair<std::string, char> > &stringReplacementChars);
void findAndReplace(std::string& source, const std::unordered_map<std::string, char> &stringReplacementChars);

unsigned int readFromDictionary(const std::string &string, const unsigned int i, const std::vector<std::string> &dictionary);

void removeSpaces(std::string& s);

bool getNextLine(std::istream &istream, std::string &nextLine, const std::string &tagline);

//Read number of parameters following a letter in a string.  ie// a(3.2,5x,4)bc(3,5)d a - 4 params, b 0, c 2, d 0
int getNumParams(const std::string &string, const unsigned int index, int &lastIndex);

void prepString(std::string &string, std::unordered_map<std::string, std::string> &defines);

std::string getLetters(const std::string& sentence);

bool bracketsMatch(const std::string& sentence);

bool getNextParam(const std::string &s, uint &i, std::string &param);

}// namespace LSPARSE

#endif //COMMON_PARSING_FUNCTIONS_HPP

/*

enum class IntegralType : int
{
    INT = 0,
    FLOAT,
    NOTANUMBER
};

IntegralType getNumber(const std::string &string, unsigned int &i, float &number)
{
    char c = string[i];
    std::string tempString;
    bool decimalPoint = false;

    if(c == '.') decimalPoint = true;
    else if(!isdigit(c)) return IntegralType::NOTANUMBER;
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

    //check if is integer type
    if(next(string, i, c) && decimalPoint == false)
    {
        if(c == 'i')
        {
            number = static_cast<float>(std::stoi(tempString));
            ++i;
            return IntegralType::INT;
        }
    }

    number = std::stof(tempString);
    return IntegralType::FLOAT;
}



//THIS WORKS
//return longest matching string...
//tokens start with an alphanumeric number
//use a treelike structure... a->aa,ab,ac->acd,aca,acf,ac1 ????
//use an ordered map???
//sorted... so if not find it, can test if matches the beginning of several tokens?
//till only one token left
//hmmmmmmmmm
//compares the two substrings!!!!
//int compare (size_t pos, size_t len, const string& str,
//             size_t subpos, size_t sublen) const;
#include <map>
//don't want it to affect i, token string will affect i...
//same with the float...

bool getTokenString(const std::string &string, const unsigned int i, std::string &tokenString, const std::map<std::string, int> &tokenMap)
{
    tokenString.clear();
    char c = string[i];
    if(!isalpha(c)) return false;

    tokenString.push_back(c);
    //create a subset?

    auto firstIt = tokenMap.begin();
    auto lastIt = tokenMap.end();
    bool matchFound = false;
    for(auto it = tokenMap.begin(); it != tokenMap.end(); ++it)
    {
        if((*it).first[0] == tokenString[0])
        {
            if(matchFound == false)
            {
                firstIt = it;
                matchFound = true;
            }
        }
        else if(matchFound)
        {
            lastIt = it;// std::prev(it);
            break;
        }
    }
    if(!matchFound) return false;

    //repeated code...?
    if(firstIt == std::prev(lastIt) )
    {
        //see if the rest of the string matches...
        for(uint k = 0; k < (*firstIt).first.size()-1; k++)
        {
            if(!next(string, i+k, c)) return false;
            if(c != (*firstIt).first[k+1]) return false;
        }
        tokenString = (*firstIt).first;
        return true;
    }

    unsigned int j = 0;
    while(true)
    {
        if(!next(string, i+j, c)) return false;//match found for first j letters, but doesnt fully match
        tokenString.push_back(c);
        ++j;
        for(auto it = firstIt; it != lastIt; ++it)
        {//skip the iterator from our list... remove it...
            //is a short match found??? say is a potential match, till our list goes to zero....
            //other options...
            //create a sorted list, and copy the pointers to another list, doesn't own the pointers to the strings, just a reference to the string list
            //as match -> new list, dont match, take out of list, final list....
            //stack, list or something
            //list fullMatch, or just a stringFullMatch,
            //use the string at the end to return.  if a 'fuller' match appears just replace it.
            //le sigh.
            if((*it).first[j] == tokenString[j])//if((*it).first.compare(0,tokenString.size(),tokenString) == 0)
            {
                if(matchFound == false)
                {
                    firstIt = it;
                    matchFound = true;
                }
            }
            else if(matchFound)
            {
                lastIt = it;
                break;
            }
        }
        if(!matchFound) return false;

        if(firstIt == std::prev(lastIt) )
        {
            //see if the rest of the string matches...
            for(uint k = j+1; k < (*firstIt).first.size()-1; k++)
            {
                if(!next(string, i+k, c)) return false;
                if(c != (*firstIt).first[k+1]) return false;
            }
            tokenString = (*firstIt).first;
            return true;
        }
    }

    return false;
}

#include <list>
//templatize it??? <std::string, T> as T doesn't really matter...
const std::string *getTokenString(const std::string &string, const unsigned int i, const std::map<std::string, int> &tokenMap)
{
    std::string tempString;
    const std::string *matchedString = nullptr;

    char c = string[i];
    if(!isalpha(c)) return matchedString;

    tempString.push_back(c);

    //auto firstIt = tokenMap.begin();
    //auto lastIt = tokenMap.end();
    bool matchFound = false;

    std::list< const std::string* > list;

    //initialize list with all potential matches
    //x is before x0 or xx in the sorting... (assumed)
    for(auto it = tokenMap.begin(); it != tokenMap.end(); ++it)
    {
        if((*it).first[0] == tempString[0])
        {
            if(matchFound == false)
            {
                matchFound = true;
            }
            list.push_back(&(*it).first);
        }
        else if(matchFound)
        {
            break;
        }
    }
    if(list.size()==0) return matchedString;
    //check if is a direct match with the first element, x before xx you know ????
    if(list.front()->size() == 1)
    {
        matchedString = list.front();
        list.pop_front();
    }
//also assumes x and x cannot be duplicate, no duplicates in a map.
//map has no duplicates, multimap may but that's ok    
    unsigned int j = 0;
    while(true)
    {
        matchFound = false;
        if(!next(string, i+j, c)) return matchedString;//no more space in string for any future matches to apply...
        tempString.push_back(c);
        ++j;
        for(auto it = list.begin(); it != list.end();)
        {
            if((*(*it))[j] != tempString[j])
                it = list.erase(it);
            else ++it;
        }
        if(list.size() == 0) return matchedString;
        if(list.front()->size() == j+1)
        {
            matchedString = list.front();
            list.pop_front();
        }
    }
    return matchedString;
}
*/