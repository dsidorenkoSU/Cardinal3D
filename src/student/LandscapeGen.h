#pragma once

#include <vector>
#include <string>

/* #define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12*/ /* 2^N */
//#define NM 0xfff

// Landscape generator class
class LandscapeGen {
public:    
    
    LandscapeGen();

    std::vector<float> generate(int nOct);

    void generateOctaves(int nOct);

    void writeToFile(const std::string& path);

    void setSize(int size);

    void setGridSizeMin(float ogs);

    std::vector<float> generateOctave(float _grid_size);

    void init();

private: 

   
    /* int p[B + B + 2];
    float g3[B + B + 2][3];
    float g2[B + B + 2][2];
    float g1[B + B + 2];
    int start;*/

	int out_w, out_h;
    float grid_size_min;
    std::vector<unsigned char> data;
};
