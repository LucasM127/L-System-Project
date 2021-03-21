#include <SFML/Graphics.hpp>
#include "Grid.hpp"
#include "BuildingCoords.hpp"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800,800),"Building View");
    sf::View view;

    Grid grid(BUILDING_TILEMAP_WIDTH, BUILDING_TILEMAP_HEIGHT, BUILDING_TILE_SIZE, BUILDING_TILE_SIZE);
    sf::Texture texture;
    if(!texture.loadFromFile(BUILDING_TILE_SOURCE))
        throw std::runtime_error("Unable to open " + BUILDING_TILE_SOURCE);
    //grid.clear(sf::Color::White);
    grid.setTexMap(&texture);
    
    view.setSize(grid.getSize());
    view.setCenter(grid.getSize().x/2,grid.getSize().y/2);
    window.setView(view);

    const CoordMapper &mapper = grid.getMapper();

    for(uint i = 0; i < BUILDING_TILEMAP_WIDTH; ++i)
    {
        for(uint j = 0; j < BUILDING_TILEMAP_HEIGHT; ++j)
        {
            uint k = BUILDING_TILE_IDS[mapper.mapID({i,j})];
            //if(k == 8) std::cout<<"ROCKS\n";
            grid.setCellTexture({i,j},texIDMapping[k],{BUILDING_TILE_SIZE,BUILDING_TILE_SIZE});
        }
    }

    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        grid.render(window);
        window.display();
        sf::Event event;
        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
    }

    return 0;
};