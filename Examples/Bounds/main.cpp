#include <SFML/Graphics.hpp>
#include <stack>
#include <iostream>
#include "../../ls.hpp"
#include "../../Parsing/LSFileParser.hpp"

const float thickness = 1.f;

struct Bounds
{
    sf::Vector2f pos;
    sf::Vector2f size;
};

void setToBounds(sf::RectangleShape &shape, const Bounds &bounds)
{
    shape.setPosition(bounds.pos);
    shape.setSize(bounds.size);
}

void splitHorizontally(const Bounds &bounds, Bounds &left, Bounds &right)//makes 2 new bounds
{
    //smaller by 10
    Bounds innerBounds = bounds;
    //innerBounds.size = innerBounds.size - sf::Vector2f(2*thickness,2*thickness);
    //innerBounds.pos = innerBounds.pos + sf::Vector2f(thickness,thickness);
    left.pos = innerBounds.pos;
    innerBounds.size.x /= 2.f;
    left.size = innerBounds.size;
    right.pos.y = left.pos.y;
    right.pos.x = left.pos.x + left.size.x;
    right.size = left.size;
}

void splitVertically(const Bounds &bounds, Bounds &left, Bounds &right)//makes 2 new bounds
{
    //smaller by 10
    Bounds innerBounds = bounds;
    //innerBounds.size = innerBounds.size - sf::Vector2f(2*thickness,2*thickness);
    //innerBounds.pos = innerBounds.pos + sf::Vector2f(thickness,thickness);
    left.pos = innerBounds.pos;
    innerBounds.size.y /= 2.f;
    left.size = innerBounds.size;
    right.pos.x = left.pos.x;
    right.pos.y = left.pos.y + left.size.y;
    right.size = left.size;
}

class BoundsInterpreter : public LSYSTEM::LSInterpreter
{
public:
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL
    };
    struct Info
    {
        Bounds bounds;
        char id;
    };
    BoundsInterpreter(Bounds startBounds) : m_startBounds(startBounds){reset();}
    void interpret(const LSYSTEM::LModule &&M) override;
    const std::vector<Info> &getData(){return m_data;}
    void reset() override
    {
        m_data.clear();
        m_outerBounds = m_startBounds;
        m_ctr = 0;
    }
private:
    Bounds m_startBounds;
    std::vector<Info> m_data;
    std::vector<sf::Color> m_colors;
    std::stack<Orientation> m_orientationStack;
    std::stack<Bounds> m_outerBoundsStack;
    std::stack<std::pair<Bounds,Bounds> > m_boundPairsStack;
    std::stack<int> m_ctrStack;
    int m_ctr;
    Bounds m_outerBounds;
    Bounds A, B;
    Orientation m_orientation;
};
//draw a rectangle shape... filled....
void BoundsInterpreter::interpret(const LSYSTEM::LModule &&M)
{
    switch (M.id)
    {
    case '+':
        m_orientation = Orientation::HORIZONTAL;
        splitHorizontally(m_outerBounds, A, B);
        break;
    case '-':
        m_orientation = Orientation::VERTICAL;
        splitVertically(m_outerBounds, A, B);
        break;
    case '[':
        m_outerBounds = m_ctr == 0 ? A : B;//reset outer bounds
        m_boundPairsStack.push({A,B});//push
        m_ctrStack.push(++m_ctr);//save so know next
        m_ctr = 0;//reset to zero
        break;
    case ']':
        m_ctr = m_ctrStack.top();
        m_ctrStack.pop();
        A = m_boundPairsStack.top().first;
        B = m_boundPairsStack.top().second;
        m_boundPairsStack.pop();
        break;
    default:
        m_data.push_back({m_outerBounds, M.id});//for [X] whatever it is...
        break;
    }
}

int main()
{
    uint n = 15;//11;//9;
    uint sz = 3;
    for(uint i = 0; i < n; ++i)
    {
        sz = 2 * sz;// + 2;
    }
    sz /= 128;//2;
    
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::White);
    //rect.setOutlineColor(sf::Color::Black);
    //rect.setOutlineThickness(-thickness);//to go 'inside'

    Bounds outerBounds;
    outerBounds.pos = {0,0};
    outerBounds.size = {sz,sz};

    BoundsInterpreter BI(outerBounds);

    LSFile lsfile;
    lsfile.loadFile("Box.ls");
    LSYSTEM::LSystem ls(lsfile.lsData());
    
    LSYSTEM::VLSentence A(lsfile.axiom());
    LSYSTEM::VLSentence B;

    LSYSTEM::VLSentence *oldSentence = &A;
    LSYSTEM::VLSentence *newSentence = &B;

    for(uint i = 0; i < n; ++i)//max for 512 resolution
    {
        ls.iterate(*oldSentence, *newSentence);
        oldSentence->clear();
        std::swap(oldSentence, newSentence);
    }
    
    ls.interpret(*oldSentence, BI);

    std::unordered_map<char, sf::Color> colorMap;
    srand(time(nullptr));

    sf::RenderTexture T;
    T.create(sz,sz);
    T.clear(sf::Color::White);
    for(auto &data : BI.getData())
    {
        if(colorMap.find(data.id) == colorMap.end())
            colorMap[data.id] = sf::Color(rand()%256,rand()%256,rand()%256);
        rect.setFillColor(colorMap[data.id]);
        setToBounds(rect,data.bounds);
        T.draw(rect);
    }
    T.display();

    T.getTexture().copyToImage().saveToFile("img.png");
    
    return 0;
}