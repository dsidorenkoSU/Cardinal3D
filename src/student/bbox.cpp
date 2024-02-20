
#include "../lib/mathlib.h"
#include "debug.h"

bool BBox::hit(const Ray& ray, Vec2& times) const {

    // TODO (PathTracer):
    const Vec3& d = ray.dir;
    const Vec3& o = ray.point;
    Vec2 tx(FLT_MAX, FLT_MAX);
    Vec2 ty(FLT_MAX, FLT_MAX);
    Vec2 tz(FLT_MAX, FLT_MAX);
    // Calculate all box planes intersections
    if(d.x != 0.0f) {
        tx.x = (min.x - o.x) / d.x;
        tx.y = (max.x - o.x) / d.x;
    }
    if(d.y != 0.0f) {
        ty.x = (min.y - o.y) / d.y;
        ty.y = (max.y - o.y) / d.y;
    }
    if(d.z != 0.0f) {
        tz.x = (min.z - o.z) / d.z;
        tz.y = (max.z - o.z) / d.z;
    }

    // Remove cases when there are no intersecting intervals
    if (tx.y < ty.x || tx.x > ty.y) 
        return false;
    if (tx.y < tz.x || tx.x > tz.y) 
        return false;
    if (ty.y < tz.x || ty.x > tz.y) 
        return false;

    float tmin = tx.x > ty.x ? tx.x : ty.x;
    tmin = tz.x > tmin ? tz.x : tmin;
    float tmax = tx.y < ty.y ? tx.y : ty.y;
    tmax = tmax < tz.y ? tmax : tz.y;
    times.x = tmin;
    times.y = tmax;
    return true;
}
