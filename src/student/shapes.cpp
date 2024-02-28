
#include "../rays/shapes.h"
#include "debug.h"

namespace PT {

const char* Shape_Type_Names[(int)Shape_Type::count] = {"None", "Sphere"};

BBox Sphere::bbox() const {

    BBox box;
    box.enclose(Vec3(-radius));
    box.enclose(Vec3(radius));
    return box;
}

Trace Sphere::hit(const Ray& ray) const {

    // TODO (PathTracer): Task 2
    // Intersect this ray with a sphere of radius Sphere::radius centered at the origin.

    // If the ray intersects the sphere twice, ret should
    // represent the first intersection, but remember to respect
    // ray.dist_bounds! For example, if there are two intersections,
    // but only the _later_ one is within ray.dist_bounds, you should
    // return that one!
    float Ox = ray.point.x;
    float Oy = ray.point.y;
    float Oz = ray.point.z;
    float Dx = ray.dir.x;
    float Dy = ray.dir.y;
    float Dz = ray.dir.z;
    float DSq = Dx * Dx + Dy * Dy + Dz * Dz;
    float p = (Dx * Ox + Dy * Oy + Dz * Oz) / DSq;
    float q = (Ox * Ox + Oy * Oy + Oz * Oz - radius * radius) / DSq;
    float Det = p * p - q;
    
    Trace ret;
    ret.origin = ray.point;
    ret.hit = false;       // was there an intersection?
    ret.distance = 0.0f;   // at what distance did the intersection occur?
    ret.position = Vec3{}; // where was the intersection?
    ret.normal = Vec3{};   // what was the surface normal at the intersection?
    if(Det < 0.0f)
        return ret;

    
    float ts[2];
    float SqDet = sqrtf(Det);
    ts[0] = -p - SqDet;     
    ts[1] = -p + SqDet;
    float t = FLT_MAX;
    for(int i = 0; i < 2; ++i) {
        if(ts[i] >= ray.dist_bounds.x && ts[i] <= ray.dist_bounds.y) {
            t = ts[i];
            break;
        }
    }
    if(t == FLT_MAX) {
        return ret;
    }
    
    ret.hit = true;
    ret.position = ray.at(t);
    ret.normal = ray.at(t).unit();
    ret.distance = (ray.point - ret.position).norm(); 

    return ret;
}

} // namespace PT
