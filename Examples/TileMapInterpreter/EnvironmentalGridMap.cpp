#include "EnvironmentalGridMap.hpp"

#include <random>
//could I make it smaller tiles???
EnviroGridMap::EnviroGridMap(const CoordMapper &mapper, sf::Vector2i origin) : m_mapper(mapper), m_origin(origin)
{
    m_collisionMap.resize(m_mapper.sz(), 0);
}

void EnviroGridMap::mark(sf::Vector2i pos)
{
    pos = pos - m_origin;
    Coord C(pos.x, pos.y);
    if(m_mapper.isValid(C))
        m_collisionMap[m_mapper.mapID(C)] = 1;
}

bool EnviroGridMap::collide(sf::Vector2i pos)
{
    pos = pos - m_origin;
    Coord C(pos.x, pos.y);
    if(m_mapper.isValid(C))
        return m_collisionMap[m_mapper.mapID(C)] == 1;
    return true;//collide with walls
}

void EnviroGridMap::clear()
{
    for(auto &i : m_collisionMap)
        i = 0;
}

EnviroProgram::EnviroProgram(EnviroGridMap &map, GridInterpreter &_I) : LSYSTEM::LSReinterpreter(
                                {{'X',0},{'w',0},{'a',0},{'s',0},{'d',0},{'i',0},{'j',0},{'k',0},{'l',0}}
                                ), m_refMap(map), I(_I)
{
    m_randomEngine.seed(time(NULL));
}

//is the same sentence...
void EnviroProgram::reinterpret(const LSYSTEM::LSentence &sentence, const uint i, LSYSTEM::VLSentence &vlsentence)
{
    if(sentence[i].id == 'A')// || sentence[i].id == 'B')
    {
        //save vlsentence, i, and turtlePosition...
        m_queries.emplace_back(I.queryTurtle().position, i, vlsentence);
    }
    if(sentence[i].id == 'I')
    {
        auto &h = I.queryTurtle().heading;
        if(h.y == -1)
            modify(vlsentence, i, {'w'});
        else if(h.y == 1)
            modify(vlsentence, i, {'s'});
        else if(h.x == -1)
            modify(vlsentence, i, {'a'});
        else
            modify(vlsentence, i, {'d'});
    }
}

void EnviroProgram::apply()
{
    std::vector<uint> indices(m_queries.size());
    for(uint i = 0; i < m_queries.size(); ++i)
        indices[i] = i;
    
    std::shuffle(indices.begin(), indices.end(),m_randomEngine);

    for(uint i = 0; i < indices.size(); ++i)
    {
        QueryData &query = m_queries[indices[i]];
        if(m_refMap.collide(query.pos))
        {
            modify(query.vls, query.i, {'X'});
        }
        else
        {
            m_refMap.mark(query.pos);
        }
    }

    m_queries.clear();
}