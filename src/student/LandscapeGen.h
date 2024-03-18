#pragma once

#include <vector>
#include <string>

// Landscape generator class
class LandscapeGen {
public:    
    
    LandscapeGen();

    void generate();

    void generateOctaves(int nOct);

    void writeToFile(const std::string& path);

    void setSize(int size);

    void setGridSizeMin(float ogs);

private: 

    std::vector<float> generateOctave(float _grid_size);

	int out_w, out_h;
    float grid_size_min;
    std::vector<unsigned char> data;
};
