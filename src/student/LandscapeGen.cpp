#include "LandscapeGen.h"
#include "../lib/vec2.h"
#include <iostream>
#include "../util/rand.h"
#include "../rays/samplers.h"
#include <sf_libs/stb_image_write.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

LandscapeGen::LandscapeGen():
	out_w(2028), 
	out_h(2048), 
    grid_size_min(64.0f) 
{
}

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
    return (a1 - a0) * w + a0;
    /* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
     * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
     *
     * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on
     * boundaries: return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
     */
}

Vec2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443;
    b ^= a << s | a >> (w - s);
    b *= 1911520717;
    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    Vec2 v;
    v.x = cos(random);
    v.y = sin(random);
    return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y, Samplers::Rect::Uniform& s) {
    // Get gradient from integer coordinates
    // float pdf;
    Vec2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y, Samplers::Rect::Uniform& s) {
    // Determine grid cell coordinates
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y, s);
    n1 = dotGridGradient(x1, y0, x, y, s);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y, s);
    n1 = dotGridGradient(x1, y1, x, y, s);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and
                  // add 0.5
}

std::vector<float>& LandscapeGen::generate(int nOct) {
    std::vector<std::vector<float>> octaves;

    int gs = out_w / 2;
    for(int i = 0; i < nOct;++i) {
        auto d = generateOctave(gs);
        octaves.push_back(d);    
        gs /= 2;
    }

    float falloff = 0.5f;
    float oct_weight = 1.0f;
    heightmap_normalized.resize(out_w * out_h);
    std::fill(heightmap_normalized.begin(), heightmap_normalized.end(), 0.0f);
    for(size_t o = 0; o < octaves.size(); ++o) {
        float oMin = 1.0f;
        float oMax = -1.0f;
        for(int i = 0; i < out_w; ++i) {
            for(int j = 0; j < out_h; ++j) {
                heightmap_normalized[i * out_w + j] += octaves[o][i * out_w + j] * oct_weight;
                if(octaves[o][i * out_w + j] < oMin) {
                    oMin = octaves[o][i * out_w + j];
                }
                if(octaves[o][i * out_w + j] > oMax) {
                    oMax = octaves[o][i * out_w + j];
                }
            }

        }
        oct_weight *= falloff;
    }
    
    float fMin = 1.0f;
    float fMax = -1.0f;
    for(int i = 0; i < out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {
            heightmap_normalized[i * out_w + j] = 0.5f + heightmap_normalized[i * out_w + j] * 0.5f;
            fMin = std::min(fMin, heightmap_normalized[i * out_w + j]);
            fMax = std::max(fMax, heightmap_normalized[i * out_w + j]);
        }
    }

    // renormalize to [0.0, 1.0]
    float dF = fMax - fMin;
    for(int i = 0; i < out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {
            heightmap_normalized[i * out_w + j] = (heightmap_normalized[i * out_w + j] - fMin) / dF; 
        }
    }

    calcGrassDensity(0.7f);
    
    data.resize(out_w * out_h);
    for(int i = 0; i < out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {
            data[i * out_w + j] = (unsigned char)(heightmap_normalized[i * out_w + j] * 255);
        }
    }
    return heightmap_normalized;
}

void LandscapeGen::calcGrassDensity(float threshold) {
    grass_density.resize(heightmap_normalized.size());
    std::fill(grass_density.begin(), grass_density.end(), 0.0f);
    for(int i = 0; i < out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {

            if(i == 0 || i == out_w - 1 || j == 0 || j == out_h - 1) {
                grass_density[i * out_w + j] = 0.0f;
                continue;
            }

            if(heightmap_normalized[i * out_w + j] < threshold) {
                float sigma = sigmaAt(i, j);
                grass_density[i * out_w + j] = 1.0f - sigma/0.5f;
            }
        }
    }
}           

std::vector<float>& LandscapeGen::grassDensity() {
    return grass_density;
}

void LandscapeGen::generateOctaves(int nOct) {

}

void LandscapeGen::writeToFile(const std::string& path) 
{
    std::string height = path + "_height.png";
    if(!stbi_write_png(height.c_str(), (int)out_w, (int)out_h, 1, data.data(), out_w)) {
        std::cout << "Failed to write png!" << std::endl;
    }
    std::vector<unsigned char> grass_d(grass_density.size());
    for(int i = 0; i<out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {
            grass_d[i * out_w + j] = grass_density[i * out_w + j] * 255;        
        }
    }
    std::string grass_path = path + "_grass.png";
    if(!stbi_write_png(grass_path.c_str(), (int)out_w, (int)out_h, 1, grass_d.data(), out_w)) {
        std::cout << "Failed to write png!" << std::endl;
    }
}

void LandscapeGen::setSize(int size)
{
    out_w = size;
    out_h = size;
}

void LandscapeGen::setGridSizeMin(float size) 
{
    grid_size_min = size;
}

std::vector<float> LandscapeGen::generateOctave(float _grid_size) {
    std::vector<float> _data;
    _data.resize(out_w * out_h);
    Samplers::Rect::Uniform s;
    for(int i = 0; i < out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {
            float x = (float)i / _grid_size;
            float y = (float)j / _grid_size;
            float peIJ = perlin(x, y, s);
            _data[i * out_w + j] = peIJ;
        }
    }
    return _data;
}

float LandscapeGen::avgHeightAt(int x, int y) {
    float sum = 0.0f;
    for(int dX = -1; dX<1;++dX){
        for(int dY = -1; dY < 1; ++dY) {
            sum += heightmap_normalized[(x + dX) * out_w + y + dY];    
        }
    } 
    return sum / 9.0f;
}

float LandscapeGen::sigmaAt(int x, int y) {
    float avg = avgHeightAt(x, y);
    float sigSq = 0.0f;
    for(int dX = -1; dX < 1; ++dX) {
        for(int dY = -1; dY < 1; ++dY) {
            float delta = heightmap_normalized[(x + dX) * out_w + y + dY] - avg; 
            sigSq += delta*delta;
        }
    }
    return sqrtf(sigSq);
}