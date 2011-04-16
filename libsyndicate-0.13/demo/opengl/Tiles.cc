#include "Tiles.h"

const GLfloat tiles_vertices[] = 
  {
    0,          0,          -0.5,  // 0
    0,          1.0,        -0.5,  // 1
    1.0,        1.0,        -0.5,  // 2
    1.0,        0,          -0.5,  // 3
    0,          0,          0,     // 4
    0,          1.0,        0,     // 5
    1.0,        1.0,        0,     // 6
    1.0,        0,          0,     // 7 
    0.65,       0.65,       -0.5,  // 8
    0.65,       0.65,       -0.25, // 9
    0.65,       0.65,       0.0,   // 10
    0.65,       0.35,       -0.5,  // 11
    0.65,       0.35,       -0.25, // 12
    0.65,       0.35,       0.0,   // 13
    0.35,       0.65,       -0.5,  // 14
    0.35,       0.65,       -0.25, // 15
    0.35,       0.65,       0.0,   // 16
    0.35,       0.35,       -0.5,  // 17
    0.35,       0.35,       -0.25, // 18
    0.35,       0.35,       0.0,   // 19
    0,          0,          -1.0,  // 20
    0,          1.0,        -1.0,  // 21
    1.0,        1.0,        -1.0,  // 22
    1.0,        0,          -1.0   // 23
  };

const GLubyte default_tile[] = 
  {
    0, 1, 2, 3,
    1, 5, 6, 2,
    2, 6, 7, 3
  };	

const GLubyte big_cube[] = 
  {
    20, 21, 22, 23,
    21,  5,  6, 22,
    22,  6,  7, 23
  };	

const GLubyte column_base[] = 
  {
    4,  5,  6,  7,
    10, 8, 14, 16,
    10, 13, 11, 8,
    8, 11, 17, 14 
  };	

const GLubyte column_base_slope[] = 
  {
    5,  6,  9, 15,
    6,  7, 12,  9,
    7,  4, 18, 12,
    4,  5, 15, 18,
    9,  8, 14, 15,
    9, 12, 11,  8,
    8, 11, 17, 14
  };	

const GLubyte column[] = 
  {
    10,  8, 14, 16,
    10, 13, 11,  8,
    8,  11, 17, 14,
  };	

const GLubyte slope_west_east [] = 
  {
    4,  5, 2, 3,
    6,  7, 3, 2,
    5,  6, 2, 2,
  };	

const GLubyte slope_north_south [] = 
  {
    7, 4, 1, 2,
    6, 2, 1, 5,
    7, 2, 6, 6,
  };	

const GLubyte slope_south_north [] = 
  {
    5, 6, 3, 0,
    0, 3, 7, 4,
    6, 7, 3, 3
  };	

const GLubyte slope_east_west [] = 
  {
    6, 7, 0, 1,
    0, 4, 5, 1,
    6, 1, 5, 5,
  };	

const GLubyte barrier_north [] = 
  {
    0, 3, 7, 4
  };	

const GLubyte barrier_west [] = 
  {
    0, 4, 5, 1
  };	

const GLubyte barrier_south [] = 
  {
    1, 5, 6, 2
  };	

const GLubyte barrier_east [] = 
  {
    3, 2, 6, 7
  };	

const GLubyte barrier_north_west [] = 
  {
    0, 3, 7, 4,
    0, 4, 5, 1
  };	

const GLubyte barrier_west_south [] = 
  {
    0, 4, 5, 1,
    1, 5, 6, 2
  };	

const GLubyte barrier_south_east [] = 
  {
    1, 5, 6, 2,
    3, 2, 6, 7
  };	

const GLubyte barrier_east_north [] = 
  {
    3, 2, 6, 7,
    0, 3, 7, 4
  };

const GLubyte barrier_east_west [] = 
  {
    3, 2, 6, 7,
    0, 4, 5, 1
  };

const GLubyte barrier_north_south [] = 
  {
    0, 3, 7, 4,
    1, 5, 6, 2
  };

const Tile tiles[] = 
{
    {12, default_tile},  // 0
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {16, column_base},
    {12, slope_west_east},
    {12, slope_west_east},
    {12, slope_west_east},
    {12, slope_north_south},
    {12, slope_north_south},  // 10
    {12, slope_north_south},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, big_cube},
    {12, big_cube},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 20
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 30
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, slope_north_south},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 40
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 50
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 60
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 70
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, slope_south_north},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 80
    {12, default_tile},
    {12, slope_north_south},
    {12, slope_west_east},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 90
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 100
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 110
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 120
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, slope_east_west},
    {12, default_tile},
    {12, default_tile},
    { 4, barrier_west},  // 130
    { 4, barrier_north},
    { 4, barrier_south},
    { 4, barrier_east},
    { 8, barrier_north_west},
    { 8, barrier_east_north},
    { 8, barrier_west_south},
    { 8, barrier_south_east},
    { 8, barrier_east_west},
    { 8, barrier_north_south},
    {12, slope_west_east},  // 140
    { 4, barrier_east},
    { 4, barrier_west},
    { 4, barrier_west},
    { 4, barrier_south},
    {12, default_tile},
    { 4, barrier_north},
    { 4, barrier_north},
    { 4, barrier_east},
    {12, default_tile},
    { 4, barrier_west},  // 150
    { 4, barrier_west},
    { 4, barrier_south},
    {12, default_tile},
    { 4, barrier_north},
    { 4, barrier_north},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 160
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 170
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, slope_south_north},
    {12, slope_east_west},
    {12, default_tile},  // 180
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {28, column_base_slope},  // 190
    {12, column},
    {12, column},
    {12, column},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 200
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 210
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 220
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, slope_south_north},
    {12, default_tile},
    {12, default_tile},  // 230
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, slope_south_north},  // 240
    {12, slope_east_west},
    {12, slope_south_north},
    {12, slope_east_west},
    {12, slope_east_west},
    {12, slope_south_north},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},
    {12, default_tile},  // 250
    {12, default_tile},
    {12, default_tile},
    {12, slope_east_west},
    {12, slope_east_west},
    {12, slope_south_north}  // 255
};
        
        
