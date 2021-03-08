#include "GridInterpreter.hpp"

#include <cassert>
#include <iostream>

GridInterpreter::GridInterpreter(GridTurtle T, const CoordMapper &_mapper) : m_startTurtle(T), mapper(_mapper)
{
    m_turtleStack.push(m_startTurtle);
    m_lastTurtleHeadingStack.push(m_startTurtle.heading);
    amFirstInBranchStack.push(true);

    reset();
}

GridInterpreter::~GridInterpreter(){}

void GridInterpreter::reset()
{
    m_turtleStack.pop();
    m_lastTurtleHeadingStack.pop();
    amFirstInBranchStack.pop();

    amFirstInBranchStack.push(true);
    m_turtleStack.push(m_startTurtle);
    m_lastTurtleHeadingStack.push(m_startTurtle.heading);
    m_data.clear();
    //rename these so i know what they do.
    
    amFirstAfterBranch = false;
    m_skipBranchType = SkipBranchType::NO_BRANCH;
}

//GOT THIS TO WORK FINALLY JUST ONLY TOOK FOREVER
//F and x are the 'same' logic ... just the tiles are different is all.
void GridInterpreter::interpret(const LSYSTEM::LSentence &sentence, const uint i)
{
    GridInfo info;
    bool skipTile = false;
    char c = sentence[i].id;

    sf::Vector2i &turtleHeading = m_turtleStack.top().heading;
    sf::Vector2i &turtlePosition = m_turtleStack.top().position;
    sf::Vector2i &lastTurtleHeading = m_lastTurtleHeadingStack.top();
    bool &isFirstInBranch = amFirstInBranchStack.top();

    info.c = turtlePosition;

    auto advanceTurtle = [&]()->void
    {
        turtlePosition = turtlePosition + turtleHeading;
        lastTurtleHeading = turtleHeading;
    };
    switch (c)
    {
    case '+':
    {
        m_lastDirChar = c;
        std::swap(turtleHeading.x,turtleHeading.y);
        turtleHeading.x *= -1;
    } break;
    case '-':
    {
        m_lastDirChar = c;
        std::swap(turtleHeading.x,turtleHeading.y);
        turtleHeading.y *= -1;
    } break;
    case '[':
    {
        m_turtleStack.push(m_turtleStack.top());
        m_lastTurtleHeadingStack.push(m_lastTurtleHeadingStack.top());
        amFirstInBranchStack.push(true);
        m_skipBranchTypeStack.push(m_skipBranchType);
        m_skipBranchType = SkipBranchType::NO_BRANCH;
        amFirstAfterBranch = false;
    } break;
    case ']':
    {
        amFirstInBranchStack.pop();
        m_turtleStack.pop();
        m_lastTurtleHeadingStack.pop();
        m_skipBranchType = m_skipBranchTypeStack.top();
        m_skipBranchTypeStack.pop();
        amFirstAfterBranch = true;
    } break;
    case 'F':
    {
        if(isFirstInBranch)
        {
            isFirstInBranch = false;

            if(m_turtleStack.size()>1)
            {
                SkipBranchType &lastSkipBranch = m_skipBranchTypeStack.top();
                if(m_lastDirChar == '+')//Right Full Branch
                {
                    if(lastSkipBranch == SkipBranchType::LEFT_FULL_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_FULL_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::LEFT_CUT_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_LEFT_CUT_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::NO_BRANCH)
                        lastSkipBranch = SkipBranchType::RIGHT_FULL_BRANCH;
                    else
                        throw std::runtime_error("This interpreter can only support up to 1 right and 1 left branch!");
                }
                else if(m_lastDirChar == '-')//Left Full Branch
                {
                    if(lastSkipBranch == SkipBranchType::RIGHT_FULL_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_FULL_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::RIGHT_CUT_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_RIGHT_CUT_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::NO_BRANCH)
                        lastSkipBranch = SkipBranchType::LEFT_FULL_BRANCH;
                    else
                        throw std::runtime_error("This interpreter can only support up to 1 right and 1 left branch!");
                }
                else
                    throw std::runtime_error("This interpreter can only support branches going right or left.  Did you forget '+' or '-'?");
                
                skipTile = true;
            }//in branch off of main
            else if(amFirstAfterBranch)//first in main branch that had been branched, cap - continuation with appropiate branching
            {
                amFirstAfterBranch = false;
                switch(m_skipBranchType)
                {
                    case SkipBranchType::RIGHT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_END_CAP;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::LEFT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_END_CAP;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::DOUBLE_FULL_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_END_CAP;
                        info.o = Orientation::R90CCW;
                        break;
                    case SkipBranchType::DOUBLE_RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::DOUBLE_LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::DOUBLE_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_TRIPLE_CAP;
                        info.o = Orientation::R180;
                        break;
                    default:
                        throw;
                }
                m_skipBranchType = SkipBranchType::NO_BRANCH;
            }
            else//first in the main branch with no offbranches to a continuation F
            {
                info.texCoord = TEX2D_STRAIGHT_END_CAP;
                info.o = Orientation::R180;
            }
        }//isFirstInBranch
        else//not the first in branch, a Full continuation piece
        {
            if(amFirstAfterBranch)//continuation piece that had been branched, continuation - continuation with appropiate branching
            {
                assert(lastTurtleHeading == turtleHeading);
                amFirstAfterBranch = false;
                switch(m_skipBranchType)
                {
                    case SkipBranchType::RIGHT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::LEFT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BRANCH_CAP;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BRANCH_CAP;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::DOUBLE_FULL_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH;
                        info.o = Orientation::R90CCW;
                        break;
                    case SkipBranchType::DOUBLE_RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_END_CAP;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::DOUBLE_LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_END_CAP;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::DOUBLE_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_BOTH_BRANCH_CAPPED;
                        info.o = Orientation::R180;
                        break;
                    default:
                        throw;
                }
                m_skipBranchType = SkipBranchType::NO_BRANCH;
            }
            else
            {
                //continuation piece that has not been branched
                if(turtleHeading == lastTurtleHeading)
                {
                    info.texCoord = TEX2D_STRAIGHT;
                    info.o = Orientation::DEFAULT;
                }
                else
                {
                    info.texCoord = TEX2D_ANGLE;
                    bool isRightTurn = ((lastTurtleHeading.x == 1 && turtleHeading.y == 1) ||
                                        (lastTurtleHeading.y == 1 && turtleHeading.x ==-1) ||
                                        (lastTurtleHeading.x ==-1 && turtleHeading.y ==-1) ||
                                        (lastTurtleHeading.y ==-1 && turtleHeading.x == 1));
                    if(isRightTurn) info.o = Orientation::R90CCW;
                    else info.o = Orientation::R180;
                }
            }
        }
        
        advanceTurtle();

        if(!skipTile)
        {
            if(turtleHeading.y ==-1) info.o = rotate(info.o, Orientation::R90CCW);
            else if(turtleHeading.x ==-1) info.o = rotate(info.o, Orientation::R180);
            else if(turtleHeading.y == 1) info.o = rotate(info.o, Orientation::R90CW);
            
            m_data.push_back(info);
        }
    } break;
    case 'x':
    {
        if(isFirstInBranch)
        {
            isFirstInBranch = false;

            if(m_turtleStack.size()>1)
            {
                SkipBranchType &lastSkipBranch = m_skipBranchTypeStack.top();
                if(m_lastDirChar == '+')//Right Cut Branch
                {
                    if(lastSkipBranch == SkipBranchType::LEFT_FULL_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_RIGHT_CUT_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::LEFT_CUT_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_CUT_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::NO_BRANCH)
                        lastSkipBranch = SkipBranchType::RIGHT_CUT_BRANCH;
                    else
                        throw std::runtime_error("This interpreter can only support up to 1 right and 1 left branch!");
                }
                else if(m_lastDirChar == '-')//Left Cut Branch
                {
                    if(lastSkipBranch == SkipBranchType::RIGHT_FULL_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_LEFT_CUT_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::RIGHT_CUT_BRANCH)
                        lastSkipBranch = SkipBranchType::DOUBLE_CUT_BRANCH;
                    else if(lastSkipBranch == SkipBranchType::NO_BRANCH)
                        lastSkipBranch = SkipBranchType::LEFT_CUT_BRANCH;
                    else
                        throw std::runtime_error("This interpreter can only support up to 1 right and 1 left branch!");
                }
                else
                    throw std::runtime_error("This interpreter can only support branches going right or left.  Did you forget '+' or '-'?");
                
                skipTile = true;
            }//in branch off of main
            else if(amFirstAfterBranch)//first in main branch that had been branched, cap - cap with appropiate branching
            {
                amFirstAfterBranch = false;
                switch(m_skipBranchType)
                {
                    case SkipBranchType::RIGHT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BOTH_ENDS_CAPPED;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::LEFT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BOTH_ENDS_CAPPED;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_ALL_CAPPED;
                        info.o = Orientation::FLIPPED_DEFAULT;
                        break;
                    case SkipBranchType::LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_ALL_CAPPED;
                        info.o = Orientation::R180;
                        break;
                    case SkipBranchType::DOUBLE_FULL_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_BOTH_BRANCH_CAPPED;
                        info.o = Orientation::R90CCW;
                        break;
                    case SkipBranchType::DOUBLE_RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_TRIPLE_CAP;
                        info.o = Orientation::R90CCW;
                        break;
                    case SkipBranchType::DOUBLE_LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_TRIPLE_CAP;
                        info.o = Orientation::R90CW;
                        break;
                    case SkipBranchType::DOUBLE_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_ALL_CAP;
                        info.o = Orientation::R180;
                        break;
                    default:
                        throw;
                }
                m_skipBranchType = SkipBranchType::NO_BRANCH;
            }
            else//first in the main branch with no offbranches to a cap
            {
                info.texCoord = TEX2D_STRAIGHT_ALL_CAP;
                info.o = Orientation::R180;
            }
        }//isFirstInBranch
        else//not the first in branch, a cap piece
        {
            if(amFirstAfterBranch)//cap piece that had been branched, continuation - cap with appropiate branching
            {
                assert(lastTurtleHeading == turtleHeading);
                amFirstAfterBranch = false;
                switch(m_skipBranchType)
                {
                    case SkipBranchType::RIGHT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_END_CAP;
                        info.o = Orientation::DEFAULT;
                        break;
                    case SkipBranchType::LEFT_FULL_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_END_CAP;
                        info.o = Orientation::FLIPPED_R180;
                        break;
                    case SkipBranchType::RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::DEFAULT;
                        break;
                    case SkipBranchType::LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::FLIPPED_R180;
                        break;
                    case SkipBranchType::DOUBLE_FULL_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_END_CAP;
                        info.o = Orientation::R90CW;
                        break;
                    case SkipBranchType::DOUBLE_RIGHT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::FLIPPED_R180;
                        break;
                    case SkipBranchType::DOUBLE_LEFT_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP;
                        info.o = Orientation::DEFAULT;
                        break;
                    case SkipBranchType::DOUBLE_CUT_BRANCH:
                        info.texCoord = TEX2D_DOUBLE_BRANCH_TRIPLE_CAP;
                        info.o = Orientation::DEFAULT;
                        break;
                    default:
                        throw;
                }
                m_skipBranchType = SkipBranchType::NO_BRANCH;
            }
            else
            {
                //continuation piece that has not been branched coming to a cap
                if(turtleHeading == lastTurtleHeading)
                {
                    info.texCoord = TEX2D_STRAIGHT_END_CAP;
                    info.o = Orientation::DEFAULT;
                }
                else
                {
                    info.texCoord = TEX2D_ANGLE_END_CAP;
                    //Buggy???
                    bool isRightTurn = ((lastTurtleHeading.x == 1 && turtleHeading.y == 1) ||
                                        (lastTurtleHeading.y == 1 && turtleHeading.x ==-1) ||
                                        (lastTurtleHeading.x ==-1 && turtleHeading.y ==-1) ||
                                        (lastTurtleHeading.y ==-1 && turtleHeading.x == 1));
                    if(isRightTurn) info.o = Orientation::R90CCW;
                    else info.o = Orientation::FLIPPED_R90CW;
//                    if(isRightTurn) info.o = Orientation::FLIPPED_R90CW;
//                    else info.o = Orientation::FLIPPED_DEFAULT;
                    if(isRightTurn) std::cout<<"IS RIGHT TURN\n";
                    else std::cout<<"IS LEFT TURN\n";
                }
            }
        }
        
        advanceTurtle();

        if(!skipTile)
        {
            if(turtleHeading.y ==-1) info.o = rotate(info.o, Orientation::R90CCW);
            else if(turtleHeading.x ==-1) info.o = rotate(info.o, Orientation::R180);
            else if(turtleHeading.y == 1) info.o = rotate(info.o, Orientation::R90CW);
            
            m_data.push_back(info);
        }
    } break;
    
    default:
        break;
    }
}