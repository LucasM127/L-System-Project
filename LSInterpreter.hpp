#ifndef LSINTERPRETER_HPP
#define LSINTERPRETER_HPP

#include <iostream>

class LSInterpreter
{
public:
    LSInterpreter(){}
    virtual ~LSInterpreter(){}
    virtual void draw() = 0;//draws it thinking...
    virtual void reset() = 0;
    virtual void feed(const char,float*,int& numVals) = 0;
};
/* //https://stackoverflow.com/questions/16605967/set-precision-of-stdto-string-when-converting-floating-point-values
std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
*/
//append becomes move, unit test speed????
class TextInterpreter:public LSInterpreter
{
public:
    TextInterpreter(std::ostream& O):os(O){}//ostream is our canvas
    ~TextInterpreter(){}
    void draw()
    {
        os<<m_data<<std::endl;
    }
    void reset() override {m_data.clear();}
    //void setPrecision(unsigned int n){m_precision = n;}
    void feed(const char c, float* vals, int& numVals) override
    {//need to assert that the paramNumMap here is the same as there... somehow.
        m_data.push_back(c);
        if(numVals)
        {
            m_data.push_back('(');
            for(int j = 0;j<(numVals-1);j++)
            {
                m_data.append(std::to_string(vals[j]),0,4);
                m_data.push_back(',');
            }
            m_data.append(std::to_string(vals[numVals-1]),0,4);
            m_data.push_back(')');
        }
    }
private:
    std::ostream& os;
    std::string m_data;
};

#endif // LSINTERPRETER_HPP