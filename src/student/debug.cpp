
#include "debug.h"
#include <imgui/imgui.h>
#include "../geometry/util.h"

#include "../lib/log.h"
#include "../lib/spectrum.h"

#include "../scene/renderer.h"
#include <nfd/nfd.h>
#include <sf_libs/stb_image_write.h>
#include "gui/widgets.h"
#include <iostream>
#include "../util/rand.h"
#include "../rays/samplers.h"

// Actual storage for the debug data
Debug_Data debug_data;

/* Debugging Tips:

    Based on your Debug_Data fields in debug.h, you can add ImGui calls
    to this function to make them editable in the debug UI panel.
    The UI panel may be shown using the Edit > Edit Debug Data menu option
    or by pressing Ctrl+D.

    ImGui is an immediate-mode GUI library, which means UI control flow
    is expressed just like normal code. For example, to create a button,
    all you have to do is:

        if(Button("My Button")) {
            // This runs when the button is clicked
        }

    Similarly, you can directly connect UI elements to data values by
    passing in the address of your storage variable:

        Checkbox("My Checkbox", &bool_variable);

    Then, bool_variable will always reflect the state of the checkbox.

    These constructs are composable to make pretty advanced UI elements!
    The whole Cardinal3D UI is implemented in this way.

    Some useful functions are documented below, and you can refer to
    deps/imgui/imgui.h for many more.
*/

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

void student_debug_ui() {
    using namespace ImGui;

    // Debug option example
    Checkbox("Pathtracer: use normal colors", &debug_data.normal_colors);

    

    // We need to store values somewhere, or else they will get reset every time
    // we run this function (which is every frame). For convenience, we make them
    // static, which gives them the same storage class as global variables.

    static int debug_draw = 0;
    InputInt("Debug draw", &debug_draw);
    debug_data.debug_draw = debug_draw;
    
    
    static Spectrum color = Spectrum(1.0f);
    ColorEdit3("Color Input", color.data);
    
    static float ROx = -2.0f;
    InputFloat("ROx", &ROx);
    static float ROy = -2.0f;
    InputFloat("ROy", &ROy);
    static float ROz = -3.0f;
    InputFloat("ROz", &ROz);

    static float Dx = 0.0f;
    InputFloat("Dx", &Dx);
    static float Dy = 0.0f;
    InputFloat("Dy", &Dy);
    static float Dz = -1.0f;
    InputFloat("Dz", &Dz);

    // ImGui examples
    if(Button("BoxRayTests")) {
        debug_data.debug_draw = 1;
        debug_data.box = BBox(Vec3(-1.0f, -1.0f, -1.0f), Vec3(1.0f, 1.0f, 1.0f));
        Ray r = Ray(Vec3(ROx, ROy, ROz), Vec3(Dx, Dy, Dz).unit());
        Vec2 t;
        debug_data.box.hit(r, t);
        debug_data.ray = r;
        debug_data.rayBoxHit = t;
    }

    if(ImGui::Button("Generate Landscape")) {
        char* path = nullptr;
        NFD_SaveDialog("png", nullptr, &path);
        if(path) {

            std::string spath(path);
            if(!Gui::postfix(spath, ".png")) {
                spath += ".png";
            }
            int out_w = 2048;
            int out_h = 2048;
            std::vector<unsigned char> data;
            data.resize(out_w*out_h);
            Samplers::Rect::Uniform s;
            for (int i=0;i<out_w;++i){
                for(int j = 0; j < out_h; ++j) {
                    float x = (float)i / 64.0f;
                    float y = (float)j / 64.0f;
                    float peIJ = (0.5f + perlin(x, y, s) * 0.5f);
                    data[i * out_w + j] = 255 * peIJ;
                }
            }

            if(!stbi_write_png(spath.c_str(), (int)out_w, (int)out_h, 1, data.data(), out_w)) {
                std::cout<<"Failed to write png!"<<std::endl;
            }
            free(path);
        }
    }
}

void Debug_Data::render_debug(const Mat4& view)
{
    if (debug_draw == 0) 
        return;
    GL::Lines lines;
    
    lines.add(box.min, Vec3(box.max.x, box.min.y, box.min.z), Vec3(1.0f, 1.0f, 1.0f));
    lines.add(box.min, Vec3(box.min.x, box.max.y, box.min.z), Vec3(1.0f, 1.0f, 1.0f));
    lines.add(box.min, Vec3(box.min.x, box.min.y, box.max.z), Vec3(1.0f, 1.0f, 1.0f));
    lines.add(box.max, Vec3(box.min.x, box.max.y, box.max.z), Vec3(1.0f, 1.0f, 1.0f));
    lines.add(box.max, Vec3(box.max.x, box.min.y, box.max.z), Vec3(1.0f, 1.0f, 1.0f));
    lines.add(box.max, Vec3(box.max.x, box.max.y, box.min.z), Vec3(1.0f, 1.0f, 1.0f));

    if(rayBoxHit.x > 0.0f && rayBoxHit.x < FLT_MAX) {
        Vec3 p0 = ray.at(rayBoxHit.x);
        Vec3 p1 = ray.at(rayBoxHit.y);
        lines.add(ray.point, p0, Vec3(0.0f, 1.0f, 0.0f));
        lines.add(p0, p1, Vec3(1.0f, 0.0f, 0.0f));
    }else {
        Vec3 p_1 = ray.at(-0.1f);
        lines.add(p_1, ray.point, Vec3(1.0f, 0.0f, 0.0f));
        lines.add(ray.point,ray.at(1.0f) , Vec3(0.0f, 0.0f, 1.0f));
    }

    Renderer::get().lines(lines, view);
}
