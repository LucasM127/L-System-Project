#include <dlfcn.h>//dlopen -ldl linkage
#include "EvalLoader.hpp"

void LibEvalLoader::init()
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
Evaluator* LibEvalLoader::load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment)
{
    shuntYardAlgorithm(expression);
//    parse(expression, varMap);
    simplify();
    const RPNList &rpnList = list();
    //is not expanded!
try
{
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
}
catch(const std::out_of_range& e)
{
    throw std::runtime_error("Variable out of range parsing expression '"+expression+"' in '"+comment);
}

    std::string C_Expression = convertToCode(rpnList);

    int n = m_libEvaluators.size();

    //std::string parsedExp = parse(expression, varIndiceMap);
    //std::string parsedExp = expression;
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

void LibEvalLoader::generate()
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

void LibEvalLoader::close()
{
    dlclose(m_soLibHandle);
}

std::string LibEvalLoader::convertToCode(const RPNList &rpnList)
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

float LibPtrEvaluator::evaluate(const float *v)
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

std::map<char,std::string> LibEvalLoader::funcOpMap =
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
    { 9  , "float(rand())/float(RAND_MAX)*"},//"float(rand()%100)/100.f*"},
	{ 10 , "-"},
    { 11, "&&"},
    { 12, "||"}
};

/*
#include <sstream>
std::string LibEvalLoader::parse(const std::string &exp, const VarIndiceMap &varMap)
{
    std::stringstream sstream;
    std::stringstream sstream2;
    sstream << exp;

    char c;

    auto isNum = [](char x)->bool
    {    
        return ((x >= '0' && x < '9') || x == '.');
    };
//read in number or variable or operator token
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
*/