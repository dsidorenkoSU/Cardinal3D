
#include "../lib/mathlib.h"
#include "debug.h"

bool BBox::hit(const Ray& ray, Vec2& times) const {

    times.x = FLT_MAX;
    times.y = FLT_MAX;
    // TODO (PathTracer):
    const Vec3& d = ray.dir;
    const Vec3& o = ray.point;
    Vec2 tx(FLT_MAX, FLT_MAX);
    Vec2 ty(FLT_MAX, FLT_MAX);
    Vec2 tz(FLT_MAX, FLT_MAX);
    // Calculate all box planes intersections
    bool yz = abs(d.x) <= FLT_MIN;
    bool xz = abs(d.y) <= FLT_MIN;
    bool xy = abs(d.z) <= FLT_MIN;

    if(!yz) {
        tx.x = (min.x - o.x) / d.x;
        tx.y = (max.x - o.x) / d.x;
        if(tx.x > tx.y) 
            std::swap(tx.x, tx.y);
    }
    if(!xz) {
        ty.x = (min.y - o.y) / d.y;
        ty.y = (max.y - o.y) / d.y;
        if (ty.x > ty.y) 
            std::swap(ty.x, ty.y);
    }
    if(!xy) {
        tz.x = (min.z - o.z) / d.z;
        tz.y = (max.z - o.z) / d.z;
        if(tz.x > tz.y) 
            std::swap(tz.x, tz.y);
    }

    // Degraded ray is just a point
    if (xy && yz && xz) {
        return false;
    }

    // Ray is parallel to X
    if (xy && xz) {
        if(ray.point.y >= min.y && ray.point.y <= max.y && ray.point.z >= min.z &&
           ray.point.z <= max.z) {
            if (tx.x >=0){
                times = tx;
                return true;
            }else {
                return false;
            }
        }     
    }

    // Ray is parallel to Z
    if (yz && xz) {
        if (ray.point.y >= min.y && ray.point.y <= max.y && ray.point.x >= min.x &&
           ray.point.x <= max.x) {
            if(tz.x >= 0.0f) {
                times = tz;
                return true;
            }else {
                return false;
            } 
        }else {
            return false;
        }
    }

    // Ray is parallel to Y
    if(xy && yz) {
        if (ray.point.x >= min.x && ray.point.x <= max.x && ray.point.z >= min.z &&
            ray.point.z <= max.z) {
            if (ty.x >=0.0f ){
                times = ty;
                return true;
            }else {
                return false;
            }
        } else {
            return false;
        }
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
