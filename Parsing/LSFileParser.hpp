#ifndef LS_FILE_PARSER_HPP
#define LS_FILE_PARSER_HPP

#include "../Containers/LSFileData.hpp"
#include "../Containers/ProductionData.hpp"
#include <unordered_map>
#include <set>
//parse a LSentence ???

//make it so like VLSentence, LSystem uses this stuff to load itself...
//Using either a file, or enter 'rules' manually
//Work on it from a production Level first.
//Production P("A<B>C=>DEFG", DefineMapping)?
class LSFile
{
public:
    void loadFile(const std::string &fileName);//fills filedata from a file
    const LSYSTEM::LSData &lsData();
    const std::string &axiom();
    const std::vector<std::string> &interpreterStrings;
    void clear();
private:
    LSYSTEM::LSFileData m_fileData;
    std::unordered_map<std::string, std::string> m_defineMapping;
    LSYSTEM::LSData m_lsData;
    std::string m_axiom;

    void loadDefines();
    void applyDefines();
    void convert();
    void loadGlobals();
};

#endif //LS_FILE_PARSER_HPP