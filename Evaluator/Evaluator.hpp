#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

//#include "Exception.hpp"

//#include "ValArray.hpp"//hehehe
struct VarIndice
{
    VarIndice() : letterNum(0), paramNum(0) {}
    VarIndice(unsigned int a, unsigned int b) : letterNum(a), paramNum(b) {}
	unsigned int letterNum;
	unsigned int paramNum;
};

#include <string>
#include <stack>
#include <map>
#include <vector>

class Number
{
public:
	virtual const float getVal() const = 0;
	virtual const float getVal(const float *v, const unsigned int depth) const = 0;
	virtual ~Number(){}
	virtual bool isConstant() = 0;
	virtual bool isComplex() = 0;
};

class Constant : public Number
{
public:
	Constant(float n):myVal(n){}
	~Constant(){}
	const float getVal() const {return myVal;}
	const float getVal(const float *v, const unsigned int depth) const {return myVal;}
	bool isConstant() override {return true;}
	bool isComplex()  override {return false;}
private:
	const float myVal;
};

class LVariable : public Number
{
public:
	LVariable(VarIndice vi) : m_varIndice(vi){}
	~LVariable(){}
	const float getVal() const {return 0.0f;}
	const float getVal(const float *v, const unsigned int depth) const;
	bool isConstant() override {return false;}
	bool isComplex()  override {return false;}
private:
	VarIndice m_varIndice;
};

class Global : public Number
{
public:
	Global(unsigned int i) : m_index(i){}
	~Global(){}
	const float getVal() const {return 0.0f;}
	const float getVal(const float *v, const unsigned int depth) const;
	bool isConstant() override {return false;}
	bool isComplex()  override {return false;}
private:
	unsigned int m_index;
};

//can be made from an operator and a sign...
//has a reverse map
//DIDN"T I DO THIS EARLIER???
//what is in the brackets is a 'number'
//what is out is not
//2x(5) -> simple
//2x(1+y) -> 2x*1 + 2x*y -> not simple, just a complex
class ComplexNumber : public Number
{
public:
	ComplexNumber(std::vector<Number*> _numList, std::vector< void(*)(std::stack<float>&) > _opList, std::vector<bool> _isNextStepAPop)
					: numList(_numList), opList(_opList), isNextStepAPop(_isNextStepAPop) {}
	const float getVal() const {return 0.0f;}
	const float getVal(const float *v, const unsigned int depth) const {return 0.0f;}
	bool isConstant() override {return false;}
	bool isComplex()  override {return true;}
//private://?
	std::vector<Number*> numList;
	std::vector< void(*)(std::stack<float>&) > opList;
	std::vector<bool> isNextStepAPop;
};
//ie, what is between parentheses

class Evaluator;

//almost have a loader class here too...
class EvalLoader
{
public:
    EvalLoader(){}
    ~EvalLoader(){}
    Evaluator* load(const std::string& expression, std::map<char, VarIndice>& varIndiceMap, unsigned int depth);
    Evaluator* load(const float value);
private:
    std::string m_expression;

	void printout(const std::vector<Number*> &numList, std::vector< void(*)(std::stack<float>&) > &opList, const std::vector<bool> &popList);

    static std::map<char,void(*)(std::stack<float>&)> binaryOpMap;
	static std::map<char,void(*)(std::stack<float>&)> unaryOpMap;
	static std::vector<std::pair<std::string, char> > tokenStringReplaceVector;
	static std::map<char, std::string> reverseTokenMapToCFunction;
	static std::map<void(*)(std::stack<float>&), std::string> reverseOpMap;
	static std::map<char, bool> opLeftAssociativityMap;
	static std::map<char, int> opPriorityMap;
    bool simplifyBinary(void(*)(std::stack<float>&));
	bool simplifyUnary (void(*)(std::stack<float>&));
	void evaluate(std::stack<char>& stackOp);
	bool nextOpHasPriority(char nextOp, char lastOp);
	bool isAnOp(const char& op);
	bool isUnary(const char& op);

	std::stack<float> stackNum;

	std::stack< std::vector<bool> > isNextStepAPopStack;//instruction list
	std::stack< std::vector<Number*> > numListStack;//numbers used in expression
	std::stack< std::vector< void(*)(std::stack<float>&) > > opListStack;
};

//Interface class
class Evaluator
{
    friend class EvalLoader;
public:
    Evaluator(const std::string& exp, unsigned int depth) : arrayDepth(depth), expression(exp){}
    virtual ~Evaluator(){}
    virtual float evaluate(const float *v) = 0;//{return 0.f;}
    const unsigned int arrayDepth;
    const std::string expression;
protected:
	static void add(std::stack<float>&);
	static void subtract(std::stack<float>&);
	static void multiply(std::stack<float>&);
	static void divide(std::stack<float>&);
	static void raiseByExponent(std::stack<float>&);
	static void sin(std::stack<float>&);
	static void cos(std::stack<float>&);
	static void tan(std::stack<float>&);
	static void cosecant(std::stack<float>&);
	static void secant(std::stack<float>&);
	static void cotangent(std::stack<float>&);
	static void random(std::stack<float>&);
	static void negate(std::stack<float>&);
	static void testIfEqual(std::stack<float>&);
	static void testIfGreaterThan(std::stack<float>&);
	static void testIfLessThan(std::stack<float>&);
	static void testIfGreaterOrEqualThan(std::stack<float>&);
	static void testIfLessThanOrEqual(std::stack<float>&);
};

//needs a valHolder for variables duh
class SimpleEvaluator:public Evaluator
{
public:
    SimpleEvaluator(const std::string& exp, unsigned int depth, Number* n):Evaluator(exp,depth),m_number(n){}
    ~SimpleEvaluator(){delete m_number;}
    float evaluate(const float *v){return m_number->getVal(v, arrayDepth);}
private:
    const Number* const m_number;
};

class ComplexEvaluator:public Evaluator
{
public:
    ComplexEvaluator(const std::string& expression, unsigned int depth, std::vector<Number*>& numbers, std::vector< void(*)(std::stack<float>&) >& ops, std::vector<bool>& steps);
    ~ComplexEvaluator();
	float evaluate(const float *v);
private:
	std::stack<float> stackNum;

	std::vector<Number*> m_numList;//numbers used in expression
	std::vector< void(*)(std::stack<float>&) > opList;//operators used in expression
	std::vector<bool> isNextStepAPop;//instruction list
};

#endif //EVALUATOR_HPP
