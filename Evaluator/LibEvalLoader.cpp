//Linux only unless can figure out how to use existing msvc compiler
#include "LibEvalLoader.hpp"
#include <dlfcn.h>//dlopen -ldl linkage

namespace EVAL
{

void LibLoader::init()
{
    file.open("LSYSTEM_FUNCS.cpp");//override mode
    if(!file)
        throw;
    file << "#include <cmath>\n\n";
    file << "#ifdef __cplusplus\n";
    file << "extern \"C\" {\n";
    file << "#endif\n\n";
}

//almost better to pass in the 'tokenized' validated expression to the function
//create a 'nested' rpnList
//pass that 'nested' rpnList to other functions
//this is what this is doing :)
Evaluator* LibLoader::load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment)
{
    shuntYardAlgorithm(tokenize(expression, varMap));
    simplify();
    //don't expand so can convert back to infix notation
    const RPNList &rpnList = list();
    
    if(rpnList.size()==1)
    {
        Evaluator *eval;
        const RPNToken &T = rpnList.back();
        if(T.isConst)
        {
            eval = new ConstEvaluator(expression, T.value);
            m_evaluators.push_back(eval);
            return eval;
        }
        else if(!T.isComplex)
        {
            eval = new SimpleEvaluator(expression, new LVariable(varMap.at(T.token),maxVarDepth));
            m_evaluators.push_back(eval);
            return eval;
        }
    }

    std::string C_Expression = convertToCode(rpnList);

    int n = m_libEvaluators.size();

    file << "//" << comment <<"\n";
    file << "float func_"<<n<<"(const float *V)\n";
    file << "{\n";

    for(auto &var : varMap)
    {
        file << "float "<< var.first << " = V[" << var.second.letterNum * maxVarDepth + var.second.paramNum << "];\n";
    }
    file << "return " << C_Expression << ";\n";
    file << "}\n\n";

    m_libEvaluators.push_back(new LibPtrEvaluator(expression, "func_" + std::to_string(n)));
    return m_libEvaluators.back();
}

void LibLoader::generate()
{
    file << "#ifdef __cplusplus\n";
    file << "}\n";//extern C
    file << "#endif\n";

    file.close();

    system("g++ -g -c -fPIC LSYSTEM_FUNCS.cpp -o LSYSTEM_FUNCS.o");
    system("g++ -g -shared -o LSYSTEM_FUNCS.so LSYSTEM_FUNCS.o");

    m_soLibHandle  = dlopen("./LSYSTEM_FUNCS.so", RTLD_LAZY);
    if (!m_soLibHandle)
    {
        //std::cerr << dlerror() << std::endl;
        throw std::runtime_error(dlerror());
    }

    for (auto p_e : m_libEvaluators)
    {
        p_e->load(m_soLibHandle);
    }
}

void LibLoader::close()
{
    dlclose(m_soLibHandle);
}

std::string LibLoader::convertToCode(const RPNList &rpnList)
{
    std::stack<std::string> theStack;
    //is non simplifieid string...
    for(auto &T : rpnList)
    {
        if(!T.isOp)
        {
            if(T.isComplex)
                theStack.push("(" + convertToCode(*T.rpnList) + ")");
            else if(T.isConst)
                theStack.push(std::to_string(T.value)+"f");
            else
                theStack.push(std::string(1,T.token));//std::to_string(char(T.value)));
        }
        else
        {
            if(isUnary(T.token))
            {//3-8 sin functions take radians... what should i do about that?
                std::string valString = theStack.top();
                theStack.pop();
                std::string opString = funcOpMap.at(T.token);
                theStack.push(opString + "(" + valString + ")");
            }
            else
            {
                std::string rhsString = theStack.top();
                theStack.pop();
                std::string lhsString = theStack.top();
                theStack.pop();
                std::string opString = funcOpMap.at(T.token);//powf(x,y) 
                if(T.token == '^')
                    theStack.push(opString + "(" + lhsString + "," + rhsString + ")");
                else
                    theStack.push(lhsString + opString + rhsString);
            }
        }
    }

    return theStack.top();
}

LibPtrEvaluator::LibPtrEvaluator(const std::string exp, const std::string &func_sig) : Evaluator(exp, false), m_func_sig(func_sig)
{}

float LibPtrEvaluator::evaluate(float *v)
{
    return (*m_funcPtr)(v);
}

void LibPtrEvaluator::load(void *soLibHandle)
{
    m_funcPtr = (FuncPtr)dlsym(soLibHandle, m_func_sig.c_str());
    char *error;
    if ((error=dlerror()) != NULL)
    {
        throw std::runtime_error(error);
    }
}

std::map<char,std::string> LibLoader::funcOpMap =
{
    {'+' , "+"},
    {'-' , "-"},
    {'*' , "*"},
    {'/' , "/"},
    {'^' , "powf"},
    { 0  , "=="},
    {'>' , ">"},
    {'<' , "<"},
    { 1  , ">="},
    { 2  , "<="},
    { 3  , "sinf"},
    { 4  , "cosf"},
    { 5  , "tanf"},
    { 6  , "asinf"},
    { 7  , "acosf"},
    { 8  , "atanf"},
    { 9  , "float(rand())/float(RAND_MAX)*"},//fix to use random_engine????
    { 10 , "-"},
    { 11, "&&"},
    { 12, "||"}
};

}//namespace EVAL