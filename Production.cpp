#include "Production.hpp"
#include <stack>

namespace LSYSTEM
{

Product::Product(const std::string &_productString, float w) : productString(_productString), m_stochasticWeight(w)
{
    product = new LSentence;
    for(const char c : productString) product->push_back(c);
}

const LSentence& Product::get(const float * V)
{
    return *product;
}

void Product::apply(LSentence &newlsentence, const float * V)
{
    newlsentence.append(*product);
}

void adjustWeights(std::vector<Product*> &products)
{
    float runningTotal = 0;
    for(Product *P : products) runningTotal += P->m_stochasticWeight;
    for(Product *P : products) P->m_stochasticWeight *= (100.f/runningTotal);
}

Product* StochasticProductChooser::choose(const float *V)
{
    float total;
    for(Product *P : products) total += P->calcWeight(V);
    
    float randomNum = float(rand()%100)*total/100.f;
    float runningTotal = 0.f;
    for(Product *P : products)
    {
        runningTotal += P->calcWeight(V);//getWeight();
        if(runningTotal > randomNum) return P;
    }
    return nullptr;
}

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
    delete m_chooser;
}

ProductionContext::ProductionContext(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters)
                                    : lContext(lc), rContext(rc), skippableLetters(_skippableLetters)
{
    lContextIndices = new unsigned int[lContext.size()];
    rContextIndices = new unsigned int[rContext.size()];
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

bool last(const std::vector<char> &lsentence, const unsigned int loc, const std::set<char> &skippableLetters, unsigned int &indice)
{
    if(lsentence.size() <= loc) return false;//invalid index

    unsigned int i = 1;
    while(loc >= i)
    {
        indice = loc - i;
        char c_last = lsentence[loc - i];
        if(isSkippable(c_last, skippableLetters))
            i++;
        else return true;
    }
    return false;
}
/*
bool ProductionContext::isSkippable(char c)
{
    return skippableLetters.count(c) > 0;
}*/

bool ProductionContext::passLContext(const std::vector<char> &lsentence, const unsigned int loc)
{
    unsigned int i = 0;
    unsigned int numSkip = 0;
    int curLvl = 0;

    if( lContext.size() > loc ) return false; //less variables to the left
    i = lContext.size() - 1;//fn not called if lContext.size() == 0

    while(true)
    {
        if ( (loc + i + numSkip) < lContext.size() ) return false;//doesn't fit
        unsigned int curIndice = loc + i + numSkip - lContext.size();
        char curChar = lsentence[curIndice];
        if ( isSkippable(curChar, skippableLetters))
        {
            numSkip--;
            continue;
        }
        else if( ( curChar == ']' ) || ( curChar == '[' ) )
        {
            if( curChar == ']' ) curLvl++;//skip over to left
            while(curLvl > 0)
            {
                numSkip--;
                curIndice = loc + i + numSkip - lContext.size();
                if(lsentence[curIndice] == ']') curLvl++;
                else if(lsentence[curIndice] == '[') curLvl--;
            }
            numSkip--;//go past '['
            continue;
        }
        else if(curChar != lContext[i]) return false;
        //passes check so far...
        lContextIndices[i] = curIndice;
        if( i == 0 ) break;
        i--;
    }

    return true;
}

bool ProductionContext::passRContext(const std::vector<char> &lsentence, const unsigned int loc)
{
    unsigned int i = 0;
    unsigned int numSkip = 0;
    unsigned int curLvl = 0;

    if( (loc + rContext.size()) >= lsentence.size() ) return false;

    std::stack<unsigned int> indexStack;
    std::stack<unsigned int> branchSizeStack;
    std::stack<unsigned int> numSkipStack;

    while(true)
    {
        unsigned int activeIndice = loc + 1 + i + numSkip;
        if( activeIndice >= lsentence.size() ) return false;
        if( isSkippable(lsentence[activeIndice], skippableLetters) )
        {
            numSkip++;
            continue;
        }
        else if(lsentence[activeIndice] == '[')//new branch
        {
            //get branch size
            unsigned int k = 0;
            curLvl++;
            while(curLvl > 0)
            {
                k++;
                if( lsentence[activeIndice + k] == ']') curLvl--;
                else if( lsentence[activeIndice + k] == '[') curLvl++;
            }

            unsigned int branchSize = k + 1;//inclusive of end ] bracket

            if(rContext[i] == '[')
            {//test branch context and save (push) branch for future popping
                indexStack.push(i);
                branchSizeStack.push(branchSize);
                numSkipStack.push(numSkip);
                //is a pass
            }
            else//just skip the branch, maybe the next letter will match
            {
                numSkip += branchSize;
                continue;
            }
        }
        else if( rContext[i] == ']' )//finished the branch context check, back to main branch
        {
            unsigned int oldIndice = indexStack.top();//where the branch started
            indexStack.pop();
            unsigned int delta = i + 1 - oldIndice;//size of branch in rcontext string
            unsigned int branchSize = branchSizeStack.top();
            branchSizeStack.pop();
            numSkip = numSkipStack.top();//reset
            numSkipStack.pop();
            numSkip = numSkip + branchSize - delta;
            //is a pass
        }
        else if( lsentence[activeIndice] != rContext[i] )//can't be this branch(?) then, maybe another branch that directly follows?
        {
            if( indexStack.size() == 0 ) return false;//is the main root! can't skip branches
            i = indexStack.top();//index of the '[' corresponding
            indexStack.pop();
            unsigned int branchSize = branchSizeStack.top();
            branchSizeStack.pop();
            numSkip = numSkipStack.top();
            numSkipStack.pop();
            numSkip += branchSize;//just skip over to the '[' (potentially) of the next branch, or it will just fail context check
            continue;
        }
        
        rContextIndices[i] = loc + 1 + i + numSkip;
        i++;
        if(i == rContext.size()) break;
    }

    return true;
}

Production::Production(const std::vector<ProductData> &pds, const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters)
                        : BasicProduction(pds), ProductionContext(lc, rc, _skippableLetters)
{}

Production::~Production()
{}

ProductChooser* Production::pass(LSentence *const lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    if(lContext.size())
        if(!passLContext(lsentence->getSentence(),i))
            return nullptr;
    if(rContext.size())
        if(!passRContext(lsentence->getSentence(),i))
            return nullptr;
    return m_chooser;
}

} // namespace LSYSTEM