#include "Production.hpp"
#include <stack>

namespace LSYSTEM
{

Product::Product(const std::string &_productString, float w) : product(_productString), m_stochasticWeight(w)
{}

Product::Product(ParametricProduct *P, const std::string &_productString) : product(_productString) {}

//non parametric
void Product::apply(LSentence &lsentence, const float * V)
{
    for(auto c : product)
    {
        lsentence.push_back(c,0);
    }
}

bool Product::isValid(const float * V){return true;}
const float Product::getWeight(){return m_stochasticWeight;}
const float Product::calcWeight(const float * V){return m_stochasticWeight;}

BasicProduction::BasicProduction(const std::vector<ProductData> &pds)
{
    Product *P_temp;
    for(const ProductData &pd : pds)
    {
        if(pd.productWeight.size()) P_temp = new Product(pd.product, std::stof(pd.productWeight));
        else P_temp = new Product(pd.product);
        m_products.push_back(P_temp);
    }
    if(m_products.size() == 1) m_chooser = new ProductChooser(m_products);
    else m_chooser = new StochasticProductChooser(m_products);
}

BasicProduction::~BasicProduction()
{
    for(Product *P : m_products) delete P;
    //delete m_chooser;
}

ProductChooser *BasicProduction::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    return m_chooser;
}

Product *BasicProduction::getProduct(){return m_chooser->choose(nullptr);}

//FIX THIS! STATE SHOULD NOT BE HELD HERE
ProductionContext::ProductionContext(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters)
                                    : lContext(lc), rContext(rc), skippableLetters(_skippableLetters)
{
    lContextIndices = new unsigned int[lc.size()];
    rContextIndices = new unsigned int[rc.size()];
}

ProductionContext::~ProductionContext()
{
    delete[] lContextIndices;
    delete[] rContextIndices;
}

bool isSkippable(char c, const std::set<char> &skippableLetters)
{
    return skippableLetters.count(c) > 0;
}

bool ProductionContext::passLContext(const LSentence &lsentence, const unsigned int curLetterIndex)
{
    unsigned int i = lContext.size() - 1;
    int curLvl = 0;
    unsigned int curIndex = curLetterIndex;
    char curChar = lsentence[curIndex].id;

    if(lContext.size() == 0) return true;
    if(lContext.size() > curLetterIndex) return false;//not enough space to the left

    while((curIndex = lsentence.last(curIndex)) < lsentence.size())//lsentence.last(curIndex, curIndex))
    {
        if ( curIndex < i ) return false;//not enough space to left
        curChar = lsentence[curIndex].id;

        if( isSkippable(curChar, skippableLetters) )
        {
            continue;
        }
        else if( (curChar == ']') || (curChar == '[') )
        {
            if( curChar == ']' ) curLvl++;//skip over to left
            while(curLvl > 0)
            {
                curIndex = lsentence.last(curIndex);
                if(lsentence[curIndex].id == ']') curLvl++;
                else if(lsentence[curIndex].id == '[') curLvl--;
            }
            continue;//go past '['
        }
        else if(curChar != lContext[i]) return false;
        //passes checks so far
        lContextIndices[i] = curIndex;
        if( i == 0 ) break;
        --i;
    }
    //could it go here if out of bounds???
    if(i > 0) return false;

    return true;
}

//matches rcontext [x][y] to letter 'a' in a[d][x][c][y]b but not a[y][x]b, fixed order
//also ax[y] not equivalent to a[x][y]
bool ProductionContext::passRContext(const LSentence &lsentence, const unsigned int curLetterIndex)
{
//no numSkipStack this time!
    unsigned int i = 0;//going right
    unsigned int curLvl = 0;
    unsigned int curIndex = curLetterIndex;
    char curChar = lsentence[curIndex].id;

    if( rContext.size() == 0 ) return true;
    if( (curLetterIndex + rContext.size()) >= lsentence.size() ) return false;
    
    std::stack<unsigned int> indexStack;//for the index

    auto advanceToEndOfBranch = [&]()->void
    {
        curLvl++;
        while(curLvl > 0)
        {
            curIndex = lsentence.next(curIndex);
            if(lsentence[curIndex].id == ']') --curLvl;
            else if(lsentence[curIndex].id == '[') ++curLvl;
        }
    };

    while((curIndex = lsentence.next(curIndex)) < lsentence.size())// lsentence.next(curIndex, curIndex))
    {
        curChar = lsentence[curIndex].id;
        if( isSkippable(curChar, skippableLetters) )
        {
            continue;
        }
        else if(curChar == '[')//new branch
        {
            //either matches context, or skip over it 
            if(rContext[i] == '[')//match?
            {
                indexStack.push(i);
                //pass
            }
            else
            {
                //skip the branch
                advanceToEndOfBranch();
                continue;
            }
        }
        else if( rContext[i] == ']')//try going back to main branch, finished checking this branch context
        {
            indexStack.pop();//passed no need to go back
            advanceToEndOfBranch();//no continue
        }
        else if( curChar != rContext[i])//no match
        {
            if(indexStack.size() == 0) return false;//can't try other branches
            i = indexStack.top();
            indexStack.pop();
            
            //go to next branch
            advanceToEndOfBranch();
            continue;
        }
        //is A MATCH so save index
        rContextIndices[i] = curIndex;
        ++i;
        
        if(i == rContext.size()) break;
    }
    if(i != rContext.size())
        return false;

    return true;
}

Production::Production(const std::vector<ProductData> &pds, const Alphabet &abc, const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters)
                        : BasicProduction(pds), ProductionContext(lc, rc, _skippableLetters)
{}

Production::~Production()
{}

ProductChooser* Production::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    if(!passLContext(lsentence,i))
        return nullptr;
    if(!passRContext(lsentence,i))
        return nullptr;
    return m_chooser;
}

} // namespace LSYSTEM