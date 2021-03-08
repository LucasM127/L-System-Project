#ifndef PARSE_PRODUCTION_STRING_HPP
#define PARSE_PRODUCTION_STRING_HPP

#include "../Containers/ProductionData.hpp"
#include <sstream>
#include "../Logger/Logger.hpp"

namespace LSYSTEM
{

bool parseProductionString(const std::string &productionString, ProductionData &pd)
{
    pd.products.clear();

    pd.rawStatement = productionString;

    std::string testSentence = productionString;
    std::size_t stringPos;

    ProductData tempProductData;

    //Get Product & Stochastic Weight
    {
        std::string tempProductString;
        stringPos = testSentence.find("=>");
        if(stringPos == std::string::npos)
        {
            std::stringstream s;
            s<<"Production '"<<productionString<<"' doesn't Produce anything!  Missing '=>'"<<std::endl;
            throw std::runtime_error(s.str());
        }
        tempProductString = testSentence.substr(stringPos+2);//from the position to the end copied over
        testSentence.erase(stringPos);

        //check for ':' for stochastic Weight expression / value in our product
        std::string tempProductStochasticWeightString;
        stringPos = tempProductString.find(":");
        if(stringPos != std::string::npos)
        {
            tempProductStochasticWeightString = tempProductString.substr(stringPos+1);
            tempProductString.erase(stringPos);
        }

        tempProductData.product = tempProductString;
        tempProductData.productWeight = tempProductStochasticWeightString;
    }//this holds the stochastic weight here above

    //Get Conditional Expression Statement
    {
        std::string tempConditionalString;
        stringPos = testSentence.find(":");
        if(stringPos != std::string::npos)
        {
            tempConditionalString = testSentence.substr(stringPos+1);
            testSentence.erase(stringPos);
        }
        tempProductData.conditionalExpression = tempConditionalString;
    }

    pd.products.push_back(tempProductData);

    //Get Right Context
    {
        std::string tempRContextString;
        stringPos = testSentence.find(">");
        if(stringPos != std::string::npos)
        {
            tempRContextString = testSentence.substr(stringPos+1);
            testSentence.erase(stringPos);
        }
        pd.rContext = tempRContextString;
    }


    //Get Left Context and Production Letter
    {
        std::string tempLContextString;
        std::string tempProductLetterString;
        stringPos = testSentence.find("<");
        if(stringPos != std::string::npos)
        {
            tempProductLetterString = testSentence.substr(stringPos+1);
            testSentence.erase(stringPos);
            tempLContextString = testSentence;
        }
        else tempProductLetterString = testSentence;
        pd.letter = tempProductLetterString;
        pd.lContext = tempLContextString;
        if(getLetters(pd.letter).size() != 1) throw std::runtime_error("Production " + productionString + " can only contain a single Predicate Letter");
    }
}

}

#endif //PARSE_PRODUCTION_STRING_HPP