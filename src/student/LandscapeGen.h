#pragma once

#include <vector>
#include <string>

// Landscape generator class
class LandscapeGen {
public:    
    
    LandscapeGen();

    void generate();

    void writeToFile(const std::string& path);

private: 
	int out_w, out_h;
    std::vector<unsigned char> data;
};
