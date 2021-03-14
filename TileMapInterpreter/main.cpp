#include "../Lib/AllLibs.hpp"
#include "../Containers/OstreamOperators.hpp"
#include "GridInterpreter.hpp"
#include "EnvironmentalGridMap.hpp"
#include <iostream>
#include <cassert>

#include <fstream>

const std::string PieceToString(sf::Vector2f V) 
{
    if(V==TEX2D_STRAIGHT                        ) return "TEX2D_STRAIGHT                        ";
    if(V==TEX2D_STRAIGHT_END_CAP                ) return "TEX2D_STRAIGHT_END_CAP                ";
    if(V==TEX2D_STRAIGHT_ALL_CAP                ) return "TEX2D_STRAIGHT_ALL_CAP                ";
    if(V==TEX2D_ANGLE                           ) return "TEX2D_ANGLE                           ";
    if(V==TEX2D_ANGLE_END_CAP                   ) return "TEX2D_ANGLE_END_CAP                   ";
    if(V==TEX2D_ANGLE_ALL_CAP                   ) return "TEX2D_ANGLE_ALL_CAP                   ";
    if(V==TEX2D_SINGLE_BRANCH                   ) return "TEX2D_SINGLE_BRANCH                   ";
    if(V==TEX2D_SINGLE_BRANCH_BRANCH_CAP        ) return "TEX2D_SINGLE_BRANCH_BRANCH_CAP        ";
    if(V==TEX2D_SINGLE_BRANCH_END_CAP           ) return "TEX2D_SINGLE_BRANCH_END_CAP           ";
    if(V==TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP) return "TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP";
    if(V==TEX2D_SINGLE_BRANCH_BOTH_ENDS_CAPPED  ) return "TEX2D_SINGLE_BRANCH_BOTH_ENDS_CAPPED  ";
    if(V==TEX2D_SINGLE_BRANCH_ALL_CAPPED        ) return "TEX2D_SINGLE_BRANCH_ALL_CAPPED        ";
    if(V==TEX2D_DOUBLE_BRANCH                   ) return "TEX2D_DOUBLE_BRANCH =                 ";
    if(V==TEX2D_DOUBLE_BRANCH_END_CAP           ) return "TEX2D_DOUBLE_BRANCH_END_CAP           ";
    if(V==TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP) return "TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP";
    if(V==TEX2D_DOUBLE_BRANCH_BOTH_BRANCH_CAPPED) return "TEX2D_DOUBLE_BRANCH_BOTH_BRANCH_CAPPED";
    if(V==TEX2D_DOUBLE_BRANCH_TRIPLE_CAP        ) return "TEX2D_DOUBLE_BRANCH_TRIPLE_CAP        ";
    if(V==TEX2D_DOUBLE_BRANCH_ALL_CAP           ) return "TEX2D_DOUBLE_BRANCH_ALL_CAP           ";
    return "";
};

int main()
{
    srand(time(NULL));
    uint mapWidth = 25;//2;// 8;// 25;
    uint mapHeight = 20;//2;// 8;//15;
    Grid G(mapWidth,mapHeight,48,48);//96,96);

    sf::Texture texture;
    texture.loadFromFile(TILESET_FILE_NAME);
    G.setTexMap(&texture);

    sf::RenderWindow window(sf::VideoMode(G.getSize().x, G.getSize().y),"LS Grid");

    LSParser lsparser;
    LSYSTEM::LSFileData fd;
    LSYSTEM::LSystemData lsData;
    lsparser.parseFile("vine.ls", fd, lsData);

    uint numVines = 33;//5+rand()%5;//1;//4+rand()%5;

    std::vector<LSYSTEM::VLSentence> sentences(numVines*2);
    std::vector<LSYSTEM::VLSentence*> sentence_ptrs(numVines*2);
    std::vector<GridTurtle> startTurtles(numVines);
    std::vector<sf::Color> colors(numVines);

    EnviroGridMap Map(G.getMapper(), {0,0});

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
        sf::Vector2i pos = {rand()%mapWidth, rand()%mapHeight};
        while (Map.collide(pos))
        {
            pos = {rand()%mapWidth, rand()%mapHeight};
        }
        Map.mark(pos);
        startTurtles[i].position = pos;
        startTurtles[i].heading = headings[rand()%4];
        colors[i] = sf::Color(rand()%256,rand()%256,rand()%256);
    }
    Map.clear();

    LSYSTEM::LSystem L(lsData);

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
            for(uint j = 0; j < I.data().size(); ++j)//auto &info : I.data())
            {
                auto &info = I.data()[j];
                Coord C(info.c.x - 0, info.c.y - 0);
                G.setCellTexture(C, info.texCoord, {TILESET_TILE_SIZE,TILESET_TILE_SIZE}, info.o);
                G.setCellColor(C, colors[i]);
                //if(j == 0) G.setCellColor(C, sf::Color::White);
                //std::cout<<PieceToString(info.texCoord)<<"\n";
            }
            //std::cout<<sentence_ptrs[2*i]->getLSentence()<<"\n";
        }
        //std::cout<<"---------------\n";
    };

    auto update = [&]()
    {
        E.apply();
        Map.clear();
    };

    //std::cout<<oldSentence->getLSentence()<<"\n";
    interpret();
    update();
/*
    LSYSTEM::VLSentence T("[+x][-Fx]x");
    GridTurtle gridt = {{1,0},{0,-1}};
    I.setStartTurtle(gridt);
    L.interpret(T,I);
    for(auto &info : I.data())
    {
        Coord C(info.c.x - 0, info.c.y - 0);
        G.setCellTexture(C, info.texCoord, {TILESET_TILE_SIZE,TILESET_TILE_SIZE}, info.o);
        G.setCellColor(C, sf::Color::Yellow);
    }
*/
    sf::Event event;
    while (window.isOpen())
    {
        window.clear(sf::Color(19,38,58));
        G.render(window);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape) window.close();
            iterate();
            //std::cout<<oldSentence->getLSentence()<<"\n";
            interpret();//homomorphisms, mark collide areas...
            update();//see where ... will collide.
        }
    }
    
    return 0;
}