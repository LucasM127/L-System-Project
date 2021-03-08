#ifndef PIPE_GRID_COORDS_HPP
#define PIPE_GRID_COORDS_HPP

#include <SFML/System/Vector2.hpp>
#include <string>

const float TILESET_TILE_SIZE = 32;
const std::string TILESET_FILE_NAME = "VinesTileset256x256.png";

const sf::Vector2f TEX2D_SEED                             = { 32+128,  0};
const sf::Vector2f TEX2D_APEX                             = { 64+128,  0};
const sf::Vector2f TEX2D_FEELER                           = { 96+128,  0};

const sf::Vector2f TEX2D_STRAIGHT                         = { 64+128, 32};
const sf::Vector2f TEX2D_STRAIGHT_END_CAP                 = { 64+128, 64};
const sf::Vector2f TEX2D_STRAIGHT_ALL_CAP                 = { 64+128, 96};

const sf::Vector2f TEX2D_ANGLE                            = { 96+128, 32};
const sf::Vector2f TEX2D_ANGLE_END_CAP                    = { 96+128, 64};
const sf::Vector2f TEX2D_ANGLE_ALL_CAP                    = { 96+128, 96};

const sf::Vector2f TEX2D_SINGLE_BRANCH                    = { 32+128, 32};
const sf::Vector2f TEX2D_SINGLE_BRANCH_BRANCH_CAP         = { 32+128, 64};
const sf::Vector2f TEX2D_SINGLE_BRANCH_END_CAP            = { 32+128, 96};
const sf::Vector2f TEX2D_SINGLE_BRANCH_BRANCH_CAP_END_CAP = { 32+128,128};
const sf::Vector2f TEX2D_SINGLE_BRANCH_BOTH_ENDS_CAPPED   = { 32+128,160};
const sf::Vector2f TEX2D_SINGLE_BRANCH_ALL_CAPPED         = { 32+128,192};

const sf::Vector2f TEX2D_DOUBLE_BRANCH =                    {  0+128, 32};
const sf::Vector2f TEX2D_DOUBLE_BRANCH_END_CAP            = {  0+128, 64};
const sf::Vector2f TEX2D_DOUBLE_BRANCH_BRANCH_CAP_END_CAP = {  0+128, 96};
const sf::Vector2f TEX2D_DOUBLE_BRANCH_BOTH_BRANCH_CAPPED = {  0+128,128};
const sf::Vector2f TEX2D_DOUBLE_BRANCH_TRIPLE_CAP         = {  0+128,160};
const sf::Vector2f TEX2D_DOUBLE_BRANCH_ALL_CAP            = {  0+128,192};

/*
const float                                      TILESET_TILE_SIZE = 64;
const std::string                TILESET_FILE_NAME = "Pipes256x320.png";

const sf::Vector2f                             APEX_SEGMENT = {128,256};

const sf::Vector2f                    FULL_STRAIGHT_SEGMENT = { 64,  0};
const sf::Vector2f              SINGLE_CAP_STRAIGHT_SEGMENT = { 64, 64};
const sf::Vector2f              ALL_CAPPED_STRAIGHT_SEGMENT = {128, 64};

const sf::Vector2f                       FULL_ANGLE_SEGMENT = {192,  0};
const sf::Vector2f              SINGLE_CAPPED_ANGLE_SEGMENT = {192, 64};
const sf::Vector2f                 ALL_CAPPED_ANGLE_SEGMENT = { 64,128};

const sf::Vector2f               FULL_SINGLE_BRANCH_SEGMENT = {128,  0};
const sf::Vector2f     SINGLE_END_CAP_SINGLE_BRANCH_SEGMENT = {192,128};
const sf::Vector2f  SINGLE_BRANCH_CAP_SINGLE_BRANCH_SEGMENT = {128,128};
const sf::Vector2f BRANCH_CAP_END_CAP_SINGLE_BRANCH_SEGMENT = {  0,128};
const sf::Vector2f     DOUBLE_END_CAP_SINGLE_BRANCH_SEGMENT = {192,256};
const sf::Vector2f         ALL_CAPPED_SINGLE_BRANCH_SEGMENT = { 64,256};

const sf::Vector2f               FULL_DOUBLE_BRANCH_SEGMENT = {  0,192};
const sf::Vector2f         SINGLE_CAP_DOUBLE_BRANCH_SEGMENT = { 64,192};
const sf::Vector2f  DOUBLE_BRANCH_CAP_DOUBLE_BRANCH_SEGMENT = {128,192};
const sf::Vector2f END_CAP_BRANCH_CAP_DOUBLE_BRANCH_SEGMENT = {192,192};
const sf::Vector2f      TRIPLE_CAPPED_DOUBLE_BRANCH_SEGMENT = {  0, 64};
const sf::Vector2f         ALL_CAPPED_DOUBLE_BRANCH_SEGMENT = {  0,256};
*/

#endif //PIPE_GRID_COORDS_HPP