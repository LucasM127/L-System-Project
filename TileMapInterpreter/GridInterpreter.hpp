#ifndef GRID_INTERPRETER_HPP
#define GRID_INTERPRETER_HPP

#include "../LSystem/LSInterpreter.hpp"

#include "Grid.hpp"
#include "PipeGridCoords.hpp"

#include <stack>

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

class GridInterpreter : public LSInterpreterSimple<std::vector<GridInfo> >
{
public:
    GridInterpreter(GridTurtle T, const CoordMapper &mapper);
    ~GridInterpreter();
private:
    void reset() override;
    void interpret(const LSYSTEM::LSentence &sentence, const uint i) override;

    const GridTurtle m_startTurtle;
    
    std::stack<GridTurtle> m_turtleStack;
    std::stack<sf::Vector2i> m_lastTurtleHeadingStack;

    const CoordMapper &mapper;

    std::stack<bool> amFirstInBranchStack;
    bool amFirstAfterBranch;
    char m_lastDirChar;
    std::stack<SkipBranchType> m_skipBranchTypeStack;//zero
    SkipBranchType m_skipBranchType;
};

#endif //GRID_INTERPRETER_HPP