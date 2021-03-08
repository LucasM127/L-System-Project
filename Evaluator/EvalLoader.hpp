#ifndef EVAL_LOADER_HPP
#define EVAL_LOADER_HPP

#include <map>
#include <vector>

#include "Evaluator.hpp"
#include "RPNToken.hpp"

class EvalLoader
{
public:
    EvalLoader(){}
    virtual ~EvalLoader();
    virtual void init(){}
    virtual void generate(){}
    virtual void close(){}
	//uses the VarIndiceMap 'container'
    virtual Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) = 0;
    Evaluator *getBasicEval(const std::string &exp, const float f);
    inline const RPNList &list(){return m_rpnListStack.top();}
#ifdef EVAL_DEBUG
	void testDistribute();
#endif
protected:
    void simplify();
	void expand(RPNList &target);
	inline RPNList &get(){return m_rpnListStack.top();}//call after parse
    void shuntYardAlgorithm(const std::string &expression);
//	void parse(const std::string &expression, const VarIndiceMap &varMap);

    static std::map<char,opFnPtr> binaryOpMap;
	static std::map<char,opFnPtr> unaryOpMap;
//private:
	static std::vector<std::pair<std::string, char> > tokenStringReplaceVector;//
	static std::map<char, bool> opLeftAssociativityMap;
	static std::map<char, int> opPriorityMap;

	void tokenize(const std::string &expression, std::string &tokenizedExpression);
	
	void evaluate(std::stack<char>& opStack);
	
	bool isUnary(const char op);
	bool isAnOp(const char op);

	void expand(RPNList &rpnList, RPNList &rpnListFinal);
	void simplify(RPNList& rpnList);
	void simplifyBinary(RPNList& rpnList);
	void simplifyUnary(RPNList& rpnList);
	void distribute(RPNList &rpnList, int complexIt);
	std::stack<float> m_floatStack;

	std::stack< RPNList > m_rpnListStack;

    std::vector<Evaluator*> m_evaluators;
};
//this one and the other one that is 'Dependant' on the varindicemapping
class RuntimeEvalLoader : public EvalLoader
{
public:
    Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) override;
};

#include <fstream>

class LibPtrEvaluator;

class LibEvalLoader : public EvalLoader
{
public:
    void init() override;
    void generate() override;
    void close() override;
    void testConvert();
    Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) override;
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
    float evaluate(const float *v) override;
    void load(void *soLibHandle);
private:
    const std::string m_func_sig;
    FuncPtr m_funcPtr;
};

#endif //EVAL_LOADER_HPP