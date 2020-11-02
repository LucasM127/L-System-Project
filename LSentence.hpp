#ifndef LSENTENCE_HPP
#define LSENTENCE_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

//can't mix a lsentence with a plsentence ????
//no out of bounds checking...
//string of form 'a(3.5,3.2)bc(3.5) or so...
//LSLoader ensures that is formatted properly...

namespace LSYSTEM
{

typedef std::unordered_map<char, unsigned int> ParamNumMapping;

template <class VALUE>
class PLSentence_t;

template <class VALUE>
class LSentence_t
{
    friend class PLSentence_t<VALUE>;
public:
    LSentence_t() {}
    LSentence_t(const std::string &string)
    {
        set(string);
    }
    virtual void set(const std::string &string)
    {
        for(auto &c : string)
            m_sentence.emplace_back(c);
    }
    virtual ~LSentence_t() {}
    size_t size() const { return m_sentence.size(); } const
    void append(const LSentence_t<VALUE> &ls)
    {
        m_sentence.insert(m_sentence.end(), ls.m_sentence.begin(), ls.m_sentence.end());
    }
    virtual void clear()
    {
        m_sentence.clear();
    }
    virtual void push_back(const LSentence_t<VALUE> &ls, const unsigned int i)
    {
        m_sentence.push_back(ls.m_sentence[i]);
    }
    virtual void push_back(char c)
    {
        m_sentence.push_back(c);
    }
    virtual VALUE* push_back(char c, int &numParams)
    {
        m_sentence.push_back(c);
        return nullptr;
    }
    virtual VALUE* getVals(const unsigned int i) const
    {
        return nullptr;
    }
    virtual VALUE* getVals(const unsigned int i, int &numVals) const
    {
        numVals = 0;
        return nullptr;
    }
    const std::vector<char> &getSentence() const
    {
        return m_sentence;
    }
    const char &back() const
    {
        return m_sentence.back();
    }
    virtual void pop_back()
    {
        m_sentence.pop_back();
    }
protected:
    std::vector<char> m_sentence;
    std::vector<VALUE*> m_params;
};

template <class VALUE>
class PLSentence_t : public LSentence_t<VALUE>
{
public:
    PLSentence_t( ParamNumMapping &pnm ) : paramNumMap(pnm) {}
    PLSentence_t( const std::string &s, ParamNumMapping &pnm ) : paramNumMap(pnm)
    {
        set(s);
    }
    void set(const std::string &string) override
    {
        std::stringstream sstream;
        sstream << string;
        char c;
        VALUE * p_arrayVals;
        int numParams;
        while(sstream >> c)
        {
            p_arrayVals = push_back(c, numParams);

            if(numParams)
            {
                sstream >> c; //the '(' symbol
                for (int i = 0; i < numParams; i++)
                {
                    sstream >> p_arrayVals[i];
                    sstream >> c; //the ',' symbol or the ')' symbol
                }
            }
        }
    }
    void append(const PLSentence_t<VALUE> &ls)
    {
        for(unsigned int i = 0; i < ls.size(); ++i)
        {
            push_back(ls, i);
        }
    }
    void clear() override
    {
        this->m_sentence.clear();
        for (VALUE *f : this->m_params)
        {
            if (f)
                delete[] f;
        }
        this->m_params.clear();
    }
    //HAS TO OVERRIDE SO :( -le sigh
    void push_back(const LSentence_t<VALUE> &ls, const unsigned int i) override
    {
        this->m_sentence.emplace_back(ls.m_sentence[i]);
        int numParams = paramNumMap[ls.m_sentence[i]];
        if(numParams)
        {
            VALUE *p_arrayVals = new VALUE[numParams];
            for(int j = 0; j < numParams; ++j)
                p_arrayVals[j] = ls.m_params[i][j];
            this->m_params.emplace_back(p_arrayVals);
        }
        else
            this->m_params.emplace_back(nullptr);
    }
    void push_back(char c) override
    {
        this->m_sentence.push_back(c);
        int numParams = paramNumMap[c];
        VALUE *p_arrayVals = nullptr;
        if(numParams)
            p_arrayVals = new VALUE[numParams];
        this->m_params.emplace_back(p_arrayVals);
    }
    VALUE* push_back(char c, int &numParams) override
    {
        this->m_sentence.emplace_back(c);
        numParams = paramNumMap[c];
        VALUE *p_arrayVals = nullptr;
        if(numParams)
            p_arrayVals = new VALUE[numParams];
        this->m_params.emplace_back(p_arrayVals);
        return p_arrayVals;
    }
    VALUE *getVals(unsigned int i, int &numVals) const override
    {
        numVals = paramNumMap[this->m_sentence[i]];
        return this->m_params[i];
    }
    VALUE *getVals(unsigned int i) const override
    {
        return this->m_params[i];
    }
    void pop_back() override
    {
        this->m_sentence.pop_back();
        if(this->m_params.back())
            delete[] this->m_params.back();
        this->m_params.pop_back();
    }
private:
    //std::vector<VALUE*> m_params;
    ParamNumMapping &paramNumMap;
};

typedef LSentence_t<float> LSentence;
typedef PLSentence_t<float> PLSentence;

} //namespace LSYSTEM

#endif //LSENTENCE_HPP