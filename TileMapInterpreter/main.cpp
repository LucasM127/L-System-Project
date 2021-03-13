#include "../Lib/AllLibs.hpp"
#include "../Containers/OstreamOperators.hpp"
#include "GridInterpreter.hpp"
#include "EnvironmentalGridMap.hpp"
#include <iostream>
#include <cassert>

#include <fstream>

int main()
{
    srand(time(NULL));
    uint mapWidth = 25;
    uint mapHeight = 15;
    Grid G(mapWidth,mapHeight,48,48);

    sf::Texture texture;
    texture.loadFromFile(TILESET_FILE_NAME);
    G.setTexMap(&texture);

    sf::RenderWindow window(sf::VideoMode(G.getSize().x, G.getSize().y),"LS Grid");

    LSParser lsparser;
    LSYSTEM::LSFileData fd;
    LSYSTEM::LSystemData lsData;
    lsparser.parseFile("vine.ls", fd, lsData);

    uint numVines = 1+rand()%9;

    std::vector<LSYSTEM::VLSentence> sentences(numVines*2);
    std::vector<LSYSTEM::VLSentence*> sentence_ptrs(numVines*2);
    std::vector<GridTurtle> startTurtles(numVines);
    std::vector<sf::Color> colors(numVines);

    sf::Vector2i headings[4] =
    {
        {-1, 0},
        { 0,-1},
        { 1, 0},
        { 0, 1}
    };

    for(uint i = 0; i < numVines; ++i)
    {
        sentences[2*i] = "S";
        sentence_ptrs[2*i] = &sentences[2*i];//oldsentence
        sentence_ptrs[2*i+1] = &sentences[2*i+1];//newSentence
        startTurtles[i].position = {rand()%mapWidth, rand()%mapHeight};
        startTurtles[i].heading = headings[rand()%4];
        colors[i] = sf::Color(rand()%256,rand()%256,rand()%256);
    }

    LSYSTEM::LSystem L(lsData);

    EnviroGridMap Map(G.getMapper(), {0,0});
    GridInterpreter I(startTurtles[0], Map);
    EnviroProgram E(Map, I);

    auto iterate = [&]()
    {
        for(uint i = 0; i < numVines; ++i)
        {
            L.iterate(*sentence_ptrs[2*i],*sentence_ptrs[2*i+1]);
            std::swap(sentence_ptrs[2*i],sentence_ptrs[2*i+1]);
            sentence_ptrs[2*i+1]->clear();
        }
    };

    auto interpret = [&]()
    {
        G.clear(sf::Color::White);

        for(uint i = 0; i < numVines; ++i)
        {
            I.setStartTurtle(startTurtles[i]);
            L.interpret(*sentence_ptrs[2*i], I, E);
            for(auto &info : I.data())
            {
                Coord C(info.c.x - 0, info.c.y - 0);
                G.setCellTexture(C, info.texCoord, {TILESET_TILE_SIZE,TILESET_TILE_SIZE}, info.o);
                G.setCellColor(C, colors[i]);
            }
        }
    };

    auto update = [&]()
    {
        E.apply();
        Map.clear();
    };

    //std::cout<<oldSentence->getLSentence()<<"\n";
    interpret();
    update();

    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(85,167,177));
        G.render(window);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            iterate();
            //std::cout<<oldSentence->getLSentence()<<"\n";
            interpret();//homomorphisms, mark collide areas...
            update();//see where ... will collide.
        }
    }
    
    return 0;
}