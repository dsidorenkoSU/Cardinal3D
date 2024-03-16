#pragma once

#include <vector>
#include <string>

// Landscape generator class
class LandscapeGen {
public:    
    
    LandscapeGen();

    void generate();

    void writeToFile(const std::string& path);

    void setSize(int size);

    void octaveGridSize(float ogs);

private: 

    std::vector<float> generateOctave(float _grid_size);

	int out_w, out_h;
    float grid_size;
    std::vector<unsigned char> data;
};
