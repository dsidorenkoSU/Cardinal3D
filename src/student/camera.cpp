
#include "../util/camera.h"
#include "../rays/samplers.h"
#include "debug.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include "math.h"

Ray Camera::generate_ray(Vec2 screen_coord) const {

    // TODO (PathTracer): Task 1
    //
    // The input screen_coord is a normalized screen coordinate [0,1]^2
    //
    // You need to transform this 2D point into a 3D position on the sensor plane, which is
    // located one unit away from the pinhole in camera space (aka view space).
    // You'll need to compute this position based on the vertial field of view
    // (vert_fov) of the camera, and the aspect ratio of the output image (aspect_ratio).

    // float horz_fov = vert_fov*aspect_ratio; // vert_fov is in deg 
    float vert_len = tan(vert_fov * M_PI / 180.0f / 2.0f) * 2.0f; 
    float horz_len = vert_len*aspect_ratio; 

    Vec3 pixel_coord((screen_coord.x-0.5f)*horz_len, (screen_coord.y-0.5f)*vert_len, -1.0f); // -0.5f because (0, 0) in screen is at (-0.5, -0.5)

    // Tip: compute the ray direction in view space and use
    // the camera space to world space transform (iview) to transform the ray back into world space.
    Vec3 origin(0, 0, 0); // camera position is at origin
    // Ray ray_world(iview.operator*(origin), iview.operator*(pixel_coord)); // ray in world space 
    Ray ray(origin, pixel_coord.unit()); 
    //Ray ray(origin, Vec3(0, 0, -1)); 

    ray.transform(iview);

    return ray;
}
