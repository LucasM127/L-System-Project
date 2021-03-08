#ifndef GRID2D_SFML_HPP
#define GRID2D_SFML_HPP

//A square grid (not triangular, not hexagonal)
//adding a vertexBuffer????

#include <SFML/Graphics.hpp>
#include <vector>

typedef unsigned int uint;

extern bool renderTestBool;

struct Coord
{
    Coord(unsigned int a = 0, unsigned int b = 0) : i(a), j(b) {}
    Coord operator + (const Coord &C) {return Coord(i + C.i, j + C.j);}
    bool operator != (const Coord &C) {return i != C.i || j != C.j;}
    unsigned int i, j;
};

enum Orientation : uint
{
    DEFAULT = 0,
    R90CCW,
    R180,
    R90CW,
    FLIPPED_DEFAULT,
    FLIPPED_R90CCW,
    FLIPPED_R180,
    FLIPPED_R90CW
};

//rotate o by r amount (not flipped though) can't rotate by a flipped amount or my brain can't process it.
inline Orientation rotate(Orientation o, Orientation r)
{
    return Orientation(o < 4 ? (o+r)%4 : (o+(4-r))%4+4);
}
/*
{
    switch (r)
    {
    case DEFAULT:
        return o;
        break;
    case R90CCW:
        return Orientation(o < 4 ? (o+1)%4 : (o+3)%4+4);
        break;
    case R180:
        return Orientation(o < 4 ? (o+2)%4 : (o+2)%4+4);
        break;
    case R90CW:
        return Orientation(o < 4 ? (o+3)%4 : (o+1)%4+4);
        break;
    default:
        break;
    }
    return o;
}*/

typedef sf::Vector2f Vec2;

//map a coord to a vector
struct CoordMapper
{
    CoordMapper(unsigned int w=0, unsigned int h=0) : width(w), height(h) {}
    unsigned int width, height;
    inline unsigned int mapID(Coord C) const {return C.i + C.j * width;}
    inline bool isValid(Coord C) const {return (C.i < width && C.j < height);}
    inline unsigned int sz() const {return width * height;}
};

class Grid
{
public:
    Grid(unsigned int width = 0, unsigned int height = 0, float cellSize_x = 0.f, float cellSize_y = 0.f);
    void resize(unsigned int width, unsigned int height, float cellSize_x, float cellSize_y);
    void setOffset(float x, float y){m_x_offset = x; m_y_offset = y;}
    const unsigned int getWidth() const { return m_mapper.width; }
    const unsigned int getHeight() const { return m_mapper.height; }
    const CoordMapper &getMapper() const { return m_mapper; }
    const sf::Vector2f getSize() const { return sf::Vector2f(m_mapper.width * m_cellSize.x, m_mapper.height * m_cellSize.y); }
    const sf::Vector2f getOffset() const { return sf::Vector2f(m_x_offset, m_y_offset); }
    const sf::Vector2f getCellSize() const {return m_cellSize;}
    Coord getCoordinate(const sf::Vector2f &pos);
    void render(sf::RenderTarget &target);
    void setTexMap(sf::Texture *tex) {m_texMap = tex;}
    void setDefaultTexCoord(sf::Vector2f v) {m_defaultTexCoord = v;}
    void setCellColor(Coord C, sf::Color color);
    void rotateRight(Coord C);//or set rotation...
    void rotateLeft(Coord C);
    void setCellTexture(Coord C, sf::Vector2f uv, sf::Vector2f sz, Orientation orientation = DEFAULT);
    void flip(Coord C);//y-axis
    void clear(sf::Color color);

private:
    sf::VertexBuffer m_vbuffer;
    bool amModified;
    std::vector<sf::Vertex> m_cellShapes;
    sf::Vector2f m_cellSize;
    const sf::Texture *m_texMap;
    float m_texSz;
    uint32_t m_randSeed;
    CoordMapper m_mapper;
    sf::Vector2f m_defaultTexCoord;

    float m_x_offset, m_y_offset;
};

#endif //GRID2D_SFML_HPP