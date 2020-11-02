#include "Evaluator.hpp"

#include <string>
#include <iostream>
#include <dlfcn.h>//dlopen -ldl linkage
#include <sstream>

namespace LSYSTEM
{

float returnOne(const float * V){ return 1.f; }

//init -> add -> generate
EvalLoader::EvalLoader() : m_basicEvaluator(returnOne)
{
    //m_evaluators.reserve(10);
}

void EvalLoader::init()
{
    file.open("LSYSTEM_FUNCS.cpp");//override mode
    if(!file) throw;
    file << "#include <cmath>\n\n";
    file << "#ifdef __cplusplus\n";
    file << "extern \"C\" {\n";
    file << "#endif\n\n";
}

Evaluator *EvalLoader::addBasicEvaluator()
{
    return &m_basicEvaluator;
}

//assumes the expression is valid c code
Evaluator *EvalLoader::add(const std::string& expression, const VarIndiceMapping &varIndiceMap, int varMaxDepth, const std::string &comment)
{
    int n = m_evaluators.size();

    //std::string parsedExp = parse(expression, varIndiceMap);
    //std::string parsedExp = expression;
    file << "//" << comment <<"\n";
    file << "float func_"<<n<<"(const float *V)\n";
    file << "{\n";

    for(auto &var : varIndiceMap)
    {
        file << "float "<< var.first << " = V[" << var.second.letterNum * varMaxDepth + var.second.paramNum << "];\n";
    }
    file << "return " << expression << ";\n";
    file << "}\n\n";

    m_evaluators.push_back(new Evaluator("func_" + std::to_string(n)));
    return m_evaluators.back();
}

void EvalLoader::generate()
{
    file << "#ifdef __cplusplus\n";
    file << "}\n";//extern C
    file << "#endif\n";

    file.close();

    system("g++ -c -fPIC LSYSTEM_FUNCS.cpp -o LSYSTEM_FUNCS.o");
    system("g++ -shared -o LSYSTEM_FUNCS.so LSYSTEM_FUNCS.o");

    m_soLibHandle  = dlopen("./LSYSTEM_FUNCS.so", RTLD_LAZY);
    if (!m_soLibHandle)
    {
        std::cerr << dlerror() << std::endl;
        throw;
    }

    for (auto p_e : m_evaluators)
    {
        p_e->load(m_soLibHandle);
    }
}

//simple, just inserts a '*' between two consecutive numbers ie 3x => 3*x for compiler
//No support for () expressions as a number yet
std::string EvalLoader::parse(const std::string &exp, const VarIndiceMapping &varMap)
{
    std::stringstream sstream;
    std::stringstream sstream2;
    sstream << exp;

    char c;

    auto isNum = [](char x)->bool
    {    
        return ((x >= '0' && x < '9') || x == '.');
    };

    float f;
    bool lastWasNum = false;
    //^token?  load () ech...
    while(sstream)
    {
        c = sstream.peek();
        if(c == EOF) break;
        if(varMap.find(c) != varMap.end())
        {
            if(lastWasNum)
            {
                //insert * sign
                sstream2 << '*';
            }
            lastWasNum = true;
            sstream >> c;
            sstream2 << c;
        }
        else if(isNum(c))
        {
            if(lastWasNum)
            {
                //insert * sign
                sstream2 << '*';
            }
            sstream >> f;
            lastWasNum = true;
            sstream2 << f;
        }
        else
        {
            sstream >> c;
            sstream2 << c;
            lastWasNum = false;
        }
    }

    return sstream2.str();
}

void EvalLoader::close()
{
    dlclose(m_soLibHandle);
}

EvalLoader::~EvalLoader()
{
    for(auto p_e : m_evaluators) delete p_e;
}

Evaluator::Evaluator(const std::string &func_sig) : m_func_sig(func_sig), m_func_ptr(nullptr)
{
}

Evaluator::Evaluator(Func_ptr F) : m_func_ptr(F)
{}

void Evaluator::load(void * soLibHandle)
{
    m_func_ptr = (Func_ptr)dlsym(soLibHandle, m_func_sig.c_str());
    char *error;
    if ((error=dlerror()) != NULL)
    {
        throw std::runtime_error(error);
    }
}

float Evaluator::evaluate(const float *V)
{
    return (*m_func_ptr)(V);
}

} //namespace LSYSTEM