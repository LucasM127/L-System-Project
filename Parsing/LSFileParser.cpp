#include "LSFileParser.hpp"
#include "ParsingFuncs.hpp"

#include <fstream>
#include <array>

#include <sstream>

#include <iostream>

void LSFile::clear()
{
    m_fileData.clear();
    m_defineMapping.clear();
}

void LSFile::loadFile(const std::string &fileName)
{
    clear();

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

    m_fileData.clear();

    //load Defines
    while(getNextLine(ifstream, curLine, curTagIndex))
        m_fileData.defines.push_back(curLine);
    if(curTagIndex == 0)//"Globals:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            m_fileData.globals.push_back(curLine);
    }
    if(curTagIndex == 1)//"Axiom:"
    {
        if(getNextLine(ifstream, curLine, curTagIndex))
        {
            m_fileData.axiom = curLine;
            getNextLine(ifstream, curLine, curTagIndex);
        }
        
    }
    if(curTagIndex == 2)//"Skippable Letters:"
    {
        if(getNextLine(ifstream, curLine, curTagIndex))
        {
            m_fileData.skippableLetters = curLine;
            getNextLine(ifstream, curLine, curTagIndex);
        }
    }
    if(curTagIndex == 3)//"Productions:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            m_fileData.productions.push_back(curLine);
    }
    if(curTagIndex == 4)//"Decomposition Productions:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            m_fileData.decompositions.push_back(curLine);
    }
    if(curTagIndex == 5)//"Homomorphic Productions:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            m_fileData.homomorphisms.push_back(curLine);
    }
    if(curTagIndex == 6)//"Interpreter Parameters:"
    {
        while(getNextLine(ifstream, curLine, curTagIndex))
            m_fileData.interpretor.push_back(curLine);
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
    
    loadDefines();
}

//recursively
void LSFile::loadDefines()
{
    for(auto &string : m_fileData.defines)
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

        LSPARSE::findAndReplace(replacement, m_defineMapping);
        m_defineMapping[define] = replacement;
    }

    applyDefines();
}

void LSFile::applyDefines()
{
    {
        LSPARSE::removeSpaces(m_fileData.axiom);
        LSPARSE::findAndReplace(m_fileData.axiom, m_defineMapping);
    }
    for(auto &string : m_fileData.productions)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, m_defineMapping);
    }
    for(auto &string : m_fileData.decompositions)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, m_defineMapping);
    }
    for(auto &string : m_fileData.homomorphisms)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, m_defineMapping);
    }
    for(auto &string : m_fileData.interpretor)
    {
        LSPARSE::removeSpaces(string);
        LSPARSE::findAndReplace(string, m_defineMapping);
    }

    convert();
}

void LSFile::convert()
{
    loadSkippableLetters();
    loadGlobals();
    m_lsData.productions = std::move(m_fileData.productions);
    m_lsData.decompositions = std::move(m_fileData.decompositions);
    m_lsData.homomorphisms = std::move(m_fileData.homomorphisms);
    m_axiom = std::move(m_fileData.axiom);
}

const std::string &LSFile::axiom()
{
    return m_axiom;
}

const LSYSTEM::LSData &LSFile::lsData()
{
    return m_lsData;
}

void LSFile::loadSkippableLetters()
{
    for(char c: m_fileData.skippableLetters)
    {
        if(c == ' ') continue;
        m_lsData.skippableLetters.insert(c);
    }
}

void LSFile::loadGlobals()
{
    for(auto &string : m_fileData.globals)
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

        m_lsData.globals.push_back({name, value});
    }
}