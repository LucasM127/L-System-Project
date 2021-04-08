//Test Building With Collision...
#include "GridInterpreter.hpp"
#include "EnvironmentalGridMap.hpp"
#include "BuildingCoords.hpp"
#include <iostream>

void setup(Grid &tileMap, Grid &vineMap, EnviroGridMap **collisionMap)
{
    tileMap.resize(BUILDING_TILEMAP_WIDTH, BUILDING_TILEMAP_HEIGHT, BUILDING_TILE_SIZE, BUILDING_TILE_SIZE);
    sf::Texture *texture = new sf::Texture;
    if(!texture->loadFromFile(BUILDING_TILE_SOURCE))
        throw std::runtime_error("Unable to open " + BUILDING_TILE_SOURCE);
    tileMap.setTexMap(texture);

    texture = new sf::Texture;
    if(!texture->loadFromFile(TILESET_FILE_NAME))
        throw std::runtime_error("Unable to open " + TILESET_FILE_NAME);
    vineMap.setTexMap(texture);
    vineMap.resize(BUILDING_TILEMAP_WIDTH, BUILDING_TILEMAP_HEIGHT, BUILDING_TILE_SIZE, BUILDING_TILE_SIZE);//2x as big ... figure this out...

    *collisionMap = new EnviroGridMap(tileMap.getMapper(),{0,0});//Remove origin, if for use with a tileMap
    (*collisionMap)->clear();

    const CoordMapper &mapper = tileMap.getMapper();
    //set Tile Map and collisionMap
    for(uint i = 0; i < BUILDING_TILEMAP_WIDTH; ++i)
    {
        for(uint j = 0; j < BUILDING_TILEMAP_HEIGHT; ++j)
        {
            uint id = BUILDING_TILE_IDS[mapper.mapID({i,j})];
            tileMap.setCellTexture({i,j},texIDMapping[id],{BUILDING_TILE_SIZE,BUILDING_TILE_SIZE});
            if(collideIDMapping[id]==1) (*collisionMap)->mark({(int)i,(int)j});
        }
    }
}

void updateVineMap(Grid &vineMap, GridInterpreter &I)
{
    vineMap.clear(sf::Color::Green);//for green vines
    for(auto &gridInfo : I.data())
    {
        Coord C(gridInfo.c.x, gridInfo.c.y);
        vineMap.setCellTexture(C, gridInfo.texCoord, {TILESET_TILE_SIZE,TILESET_TILE_SIZE}, gridInfo.o);
        //vineMap.setCellColor(C, sf::Color::Black);
    }
}

int main()
{
    srand(time(NULL));
    Grid tileMap;
    Grid vineMap;
    EnviroGridMap *collisionMap = nullptr;

    setup(tileMap,vineMap,&collisionMap);

    LSFile lsfile;
    lsfile.loadFile("vine.ls");
    LSYSTEM::LSystem L(lsfile.lsData());

    GridTurtle T;
    T.heading = {1,0};
    T.position = {7,(int)tileMap.getHeight()-5};//bottom left corner should be ok, as grass...

    GridInterpreter I(T, *collisionMap);
    EnviroProgram E(*collisionMap, I);//don't clear the collision map this time...

    LSYSTEM::VLSentence A(lsfile.axiom());
    LSYSTEM::VLSentence B;
    LSYSTEM::VLSentence *oldSentence = &A;
    LSYSTEM::VLSentence *newSentence = &B;

    L.interpret(*oldSentence,I,E);
    updateVineMap(vineMap, I);
    std::cout<<oldSentence->getLSentence()<<"\n";

    sf::RenderWindow window(sf::VideoMode(800,800),"Vines on a Building?");
    sf::View view;
    view.setSize(tileMap.getSize());
    view.setCenter(tileMap.getSize().x/2,tileMap.getSize().y/2);
    window.setView(view);
/*
    for(int i = 0; i < 100; ++i)
    {
        L.iterate(*oldSentence, *newSentence);
        std::swap(oldSentence,newSentence);
        L.interpret(*oldSentence,I,E);
        newSentence->clear();
        E.apply();//don't clear the map this time.
    }
    updateVineMap(vineMap, I);
*/
    sf::Event event;
    while(window.isOpen())
    {
        window.clear(sf::Color::White);
        tileMap.render(window);
        vineMap.render(window);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Escape) window.close();
            L.iterate(*oldSentence, *newSentence);
            std::swap(oldSentence,newSentence);
            std::cout<<oldSentence->getLSentence()<<"\n";
            L.interpret(*oldSentence,I,E);
            newSentence->clear();
            E.apply();//don't clear the map this time.
            //collisionMap->clear();//?
            updateVineMap(vineMap, I);
            std::cout<<oldSentence->getLSentence()<<"\n\n";
        }
    }

    delete vineMap.getTexture();
    delete tileMap.getTexture();
    delete collisionMap;
    return 0;
}