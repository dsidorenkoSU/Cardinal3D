
#include "debug.h"
#include <imgui/imgui.h>
#include "../geometry/util.h"

#include "../lib/log.h"
#include "../lib/spectrum.h"

#include "../scene/renderer.h"
#include <nfd/nfd.h>
#include <sf_libs/stb_image_write.h>
#include "gui/widgets.h"
#include "LandscapeGen.h"

// Actual storage for the debug data
Debug_Data debug_data;
LandscapeGen gen;

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

    static int LSSize = 2048;
    InputInt("LandscapeSize", &LSSize);
    static float GridSize = 64.0f;
    InputFloat("GridSize", &GridSize);
    gen.setSize(LSSize);
    gen.octaveGridSize(GridSize);

    if(ImGui::Button("Generate Landscape")) {
        char* path = nullptr;
        NFD_SaveDialog("png", nullptr, &path);
        if(path) {

            std::string spath(path);
            if(!Gui::postfix(spath, ".png")) {
                spath += ".png";
            }
            gen.generate();        
            gen.writeToFile(spath);
            
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
