#ifndef GRID_INTERPRETER_HPP
#define GRID_INTERPRETER_HPP

#include "../../ls.hpp"
#include "EnvironmentalGridMap.hpp"

#include "Grid.hpp"
#include "VineCoords.hpp"

#include <stack>

class EnviroGridMap;

struct GridInfo
{
    sf::Vector2f texCoord;
    Orientation o;
    sf::Vector2i c;
};

struct GridTurtle
{
    sf::Vector2i position;
    sf::Vector2i heading;
};

enum class SkipBranchType
{
    NO_BRANCH,
    RIGHT_FULL_BRANCH,
    LEFT_FULL_BRANCH,
    RIGHT_CUT_BRANCH,
    LEFT_CUT_BRANCH,
    DOUBLE_FULL_BRANCH,
    DOUBLE_CUT_BRANCH,
    DOUBLE_LEFT_CUT_BRANCH,
    DOUBLE_RIGHT_CUT_BRANCH
};

class GridInterpreter : public LSYSTEM::LSInterpreter//LSInterpreterSimple<std::vector<GridInfo> >
{
public:
    GridInterpreter(GridTurtle T, EnviroGridMap &map);
    ~GridInterpreter();
    std::vector<GridInfo> &data(){return m_data;}
    void setStartTurtle(GridTurtle &T){m_startTurtle = T;}
    const GridTurtle &queryTurtle() const {return m_turtleStack.top();}
private:
    void reset() override;
    void interpret(const LSYSTEM::LModule &&M) override;//const LSYSTEM::LSentence &sentence, const uint i) override;

    GridTurtle m_startTurtle;
    EnviroGridMap &r_map;
    
    std::stack<GridTurtle> m_turtleStack;
    std::stack<sf::Vector2i> m_lastTurtleHeadingStack;

    std::stack<bool> amFirstInBranchStack;
    bool amFirstAfterBranch;
    char m_lastDirChar;
    std::stack<SkipBranchType> m_skipBranchTypeStack;//zero
    SkipBranchType m_skipBranchType;

    std::vector<GridInfo> m_data;
};

#endif //GRID_INTERPRETER_HPP