#include "LandscapeGen.h"
#include "../lib/vec2.h"
#include <iostream>
#include "../util/rand.h"
#include "../rays/samplers.h"
#include <sf_libs/stb_image_write.h>

LandscapeGen::LandscapeGen():
	out_w(2028), 
	out_h(2048) 
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
    b ^= a << s | a >> w - s;
    b *= 1911520717;
    a ^= b << s | b >> w - s;
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
    float pdf;
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

void LandscapeGen::generate() 
{
    data.resize(out_w * out_h);
    Samplers::Rect::Uniform s;
    for(int i = 0; i < out_w; ++i) {
        for(int j = 0; j < out_h; ++j) {
            float x = (float)i / 64.0f;
            float y = (float)j / 64.0f;
            float peIJ = (0.5f + perlin(x, y, s) * 0.5f);
            data[i * out_w + j] = 255 * peIJ;
        }
    }
}

void LandscapeGen::writeToFile(const std::string& path) {
    
    if(!stbi_write_png(path.c_str(), (int)out_w, (int)out_h, 1, data.data(), out_w)) {
        std::cout << "Failed to write png!" << std::endl;
    }
    
}