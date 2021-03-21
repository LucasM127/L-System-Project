#ifndef ENVIRONMENTAL_GRID_MAP_HPP
#define ENVIRONMENTAL_GRID_MAP_HPP

#include "Grid.hpp"//mapper coord -> position
#include "GridInterpreter.hpp"
#include <random>
//#include "../LSystem/LSInterpreter.hpp"

class GridInterpreter;

class EnviroGridMap
{
public:
    EnviroGridMap(const CoordMapper &mapper, sf::Vector2i origin);
    void mark(sf::Vector2i pos);
    bool collide(sf::Vector2i pos);
    void clear();
private:
    const CoordMapper &m_mapper;
    const sf::Vector2i m_origin;//THINKING...
    std::vector<int> m_collisionMap;
};

#include "GridInterpreter.hpp"

class EnviroProgram : public LSYSTEM::LSReinterpreter
{
    struct QueryData
    {
        QueryData(sf::Vector2i p, uint index, LSYSTEM::VLSentence &s) : pos(p), i(index), vls(s){}
        const sf::Vector2i pos;
        const uint i;
        LSYSTEM::VLSentence &vls;
    };
public:
    EnviroProgram(EnviroGridMap &map, GridInterpreter &I_);
    void apply();
private:
    EnviroGridMap &m_refMap;
    GridInterpreter &I;
    void reinterpret(const LSYSTEM::LSentence &sentence, const uint i, LSYSTEM::VLSentence &vlsentence) override;
    std::vector<QueryData> m_queries;
    std::default_random_engine m_randomEngine;
};

#endif //ENVIRONMENTAL_GRID_MAP_HPP