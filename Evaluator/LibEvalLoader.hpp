#ifndef LIB_EVAL_LOADER_HPP
#define LIB_EVAL_LOADER_HPP

#include "EvalLoader.hpp"
#include <fstream>
/******
namespace EVAL
{

class LibPtrEvaluator;

class LibLoader : public Loader
{
public:
    void init() override;
    void generate() override;
    void close() override;
    void testConvert();
    //Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) override;
    Evaluator* load(const std::string &expression, const RPNList &tokenizedExp, const std::string &comment) override;
    std::string convertToCode(const RPNList &rpnList);
    //std::string parse(const std::string &exp, const VarIndiceMap &varMap);
private:
    std::string fileName;
    std::ofstream file;
    std::vector<LibPtrEvaluator*> m_libEvaluators;
    void * m_soLibHandle;

    static std::map<char,std::string> funcOpMap;
};

typedef float (*FuncPtr)(const float *);

class LibPtrEvaluator : public Evaluator
{
public:
    LibPtrEvaluator(const std::string exp, const std::string &func_sig);
    float evaluate(float *v) override;
    void load(void *soLibHandle);
private:
    const std::string m_func_sig;
    FuncPtr m_funcPtr;
};

}//namespace EVAL
****/
#endif //LIB_EVAL_LOADER_HPP