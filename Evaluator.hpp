//evaluates an expression
//will use the compiler this time to do it for us...
//go through the expression -> parse it for compiler -> provide args, index -> compile, save as func_ptr to the index
#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <unordered_map>
#include <vector>
#include <fstream>
#include "VarHolder.hpp"
//#include <deque>

namespace LSYSTEM
{

//token is a 'char' though it could be a string too
typedef char VariableToken;
typedef std::unordered_map<VariableToken, VarIndice> VarIndiceMapping;

typedef float (*Func_ptr)(const float *);
//typedef bool (*bool_func_ptr)(float *);

class Evaluator
{
public:
    Evaluator(const std::string &func_sig);
    Evaluator(Func_ptr F);
    void load(void * soLibHandle);
    float evaluate(const float *V);
private:
    const std::string m_func_sig;//use to identify
    Func_ptr m_func_ptr;
};

//like a factory
class EvalLoader
{
public:
    EvalLoader();
    ~EvalLoader();
    Evaluator* add(const std::string& expression, const VarIndiceMapping &varIndiceMap, int varMaxDepth, const std::string &comment);
    Evaluator* addBasicEvaluator();
    void init();
    void generate();
    void close();
private:
    std::string fileName;
    std::ofstream file;
    std::vector<Evaluator*> m_evaluators;
    Evaluator m_basicEvaluator;
    void * m_soLibHandle;
    std::string parse(const std::string &exp, const VarIndiceMapping &varMap);
};

} //namespace LSYSTEM

#endif //EVALUATOR_HPP