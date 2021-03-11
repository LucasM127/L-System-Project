#include "../Lib/AllLibs.hpp"
#include "GridInterpreter.hpp"
#include <iostream>
#include <cassert>

#include <fstream>

int main()
{
    uint mapWidth = 9;
    uint mapHeight = 9;
    Grid G(mapWidth,mapHeight,96,96);

    sf::Texture texture;
    texture.loadFromFile(TILESET_FILE_NAME);
    G.setTexMap(&texture);

    sf::RenderWindow window(sf::VideoMode(G.getSize().x, G.getSize().y),"LS Grid");
    std::string lstring;
/*
    std::ifstream ifstream;
    ifstream.open("foo");
    
    std::getline(ifstream, lstring);
    ifstream.close();
*/
    lstring = "F[-F[-FF-x][+FF+x]F[-F+x][+F-x]Fx]F+FFFF+FFFF+FFF+FFF+FF+FF+F[+F+x]x";

    LSYSTEM::Alphabet abc;
    abc['F'] = 0;
    abc['f'] = 0;
    abc['+'] = 0;
    abc['-'] = 0;
    abc['['] = 0;
    abc[']'] = 0;
    abc['x'] = 0;

    //make an empty lsystem?
    LSYSTEM::LSystemData lsData;
    LSYSTEM::LSystem ls(lsData);//should use default productions for it all...

//    LSYSTEM::LSentence StraightLineSentence(abc);
//    StraightLineSentence.loadSimple(lstring);//eh.. what'evs
    LSYSTEM::VLSentence vlsentence(lstring);

    GridTurtle T;
    T.position = {(int)mapWidth/2, (int)mapHeight/2};
    T.heading = {1,0};

    GridInterpreter I(T, G.getMapper());

//    LSInterpreterSimple<std::vector<GridInfo> > * I = new GridInterpreter(T, G.getMapper());

    G.clear(sf::Color::White);
    
    ls.interpret(vlsentence, I);
//    I->interpret(StraightLineSentence);

    for(auto &info : I.data())
    {
        assert(info.c.x > 0 && info.c.y > 0);
        Coord C(info.c.x, info.c.y);
        G.setCellTexture(C, info.texCoord, {32,32}, info.o);
    }

    std::cout<<"v size: " << I.data().size() << std::endl;
    std::cout<<vlsentence<<std::endl;

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(173,235,173));//(80,80,80));
        G.render(window);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed) window.close();
    }
    
    return 0;
}