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

    std::vector<float>& generate(int nOct);

    std::vector<float>& grassDensity();

    void generateOctaves(int nOct);

    void writeToFile(const std::string& path);

    void setSize(int size);

    void setGridSizeMin(float ogs);

    std::vector<float> generateOctave(float _grid_size);

    void calcGrassDensity(float threshold);

private: 

    float avgHeightAt(int x, int y);

    float sigmaAt(int x, int y);

	int out_w, out_h;
    float grid_size_min;
    std::vector<unsigned char> data;
    std::vector<float> heightmap_normalized;
    std::vector<float> grass_density;
};
