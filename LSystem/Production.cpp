#include "Production.hpp"
#include <stack>

namespace LSYSTEM
{

BasicProduction::BasicProduction(const ProductionData &ppd, EVAL::Loader &ev, const unsigned int arrayDepth) : m_arrayDepth(arrayDepth)
{
    bool isConditional = false;
    bool isVariableWeight = false;
    for(const ProductData &pd : ppd.products)
    {
        m_products.emplace_back(pd, ev, arrayDepth);
        if(pd.conditional.size()>0)
            isConditional = true;//has a conditional statement
        if(!m_products.back().isFixedWeight())
            isVariableWeight = true;//weights vary on variables
    }
    if(m_products.size()==1)
    {
        if(isConditional)
            m_chooser = new ConditionalProductChooser(m_products);
        else
            m_chooser = new ProductChooser(m_products);
    }
    else
    {
        if(isVariableWeight)
        {
            if(isConditional)
                m_chooser = new VariableStochasticConditionalProductChooser(m_products);
            else
                m_chooser = new VariableStochasticProductChooser(m_products);
        }
        else
        {
            if(isConditional)
                m_chooser = new StochasticConditionalProductChooser(m_products);
            else
                m_chooser = new StochasticProductChooser(m_products);
        }
        
    }
}

BasicProduction::~BasicProduction()
{
    delete m_chooser;
}

void BasicProduction::update()
{
    m_chooser->update();//may have to recalculate stochastic weights
}

Product *BasicProduction::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder)
{
    return m_chooser->choose(V);
}

ProductionContext::ProductionContext(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters)
                                    : lContext(lc), rContext(rc), skippableLetters(_skippableLetters)
{}

ProductionContext::~ProductionContext()
{}

bool isSkippable(char c, const std::set<char> &skippableLetters)
{
    return skippableLetters.count(c) > 0;
}

bool ProductionContext::passLContext(const LSentence &lsentence, const unsigned int curLetterIndex, unsigned int *lContextIndices)
{
    unsigned int i = lContext.size() - 1;//start at the right, go left
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
            if( curChar == ']' ) curLvl++;//skip over [] to left
            while(curLvl > 0)
            {
                curIndex = lsentence.last(curIndex);
                if(lsentence[curIndex].id == ']') curLvl++;
                else if(lsentence[curIndex].id == '[') curLvl--;
            }
            //Not sure if this makes sense though.
            //if(curChar == '[' && curChar == lContext[i]))//AM checking for beginning of branch, 'optional'
            //    ;//is A MATCH
            //else
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
bool ProductionContext::passRContext(const LSentence &lsentence, const unsigned int curLetterIndex, unsigned int *rContextIndices)
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
            if(curChar != ']')
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

Production::Production(const ProductionData &ppd, EVAL::Loader &ev, const unsigned int arrayDepth,
                        const std::set<char> &skippableLetters)
                        : BasicProduction(ppd, ev, arrayDepth), ProductionContext(ppd.lContext, ppd.rContext, skippableLetters)
{}

Production::~Production()
{}

Product *Production::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder)
{
    if(!passLContext(lsentence,i,indiceHolder))
        return nullptr;
    if(!passRContext(lsentence,i,&indiceHolder[lContext.size()]))
        return nullptr;
    return m_chooser->choose(V);
}

} // namespace LSYSTEM