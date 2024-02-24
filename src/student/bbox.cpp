
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

    // Compute intersections and sort intersection points,
    // So that t1 comes before t2 
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

    // There is no intersection
    // if at least two intersectoins are behind the origin of the ray
    if ((tx.x < 0.0f && tx.x != FLT_MAX && tx.y < 0.0f && tx.y != FLT_MAX) || 
        (ty.x < 0.0f && ty.x != FLT_MAX && ty.y < 0.0f && ty.y != FLT_MAX) || 
        (tz.x != FLT_MAX && tz.x < 0.0f && tz.y < 0.0f && tz.x != FLT_MAX))
        return false;

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

    // Ray is in xy plane
    if (xy) {
        float tmin = tx.x > ty.x ? tx.x : ty.x;
        float tmax = tx.y < ty.y ? tx.y : ty.y;
        times = Vec2(tmin, tmax);
        return true;
    }

    // Ray is in xz plane
    if(xz) {
        float tmin = tx.x > tz.x ? tx.x : tz.x;
        float tmax = tx.y < tz.y ? tx.y : tz.y;
        times = Vec2(tmin, tmax);
        return true;
    }

    // Ray is in yz plane
    if(yz) {
        float tmin = ty.x > tz.x ? ty.x : tz.x;
        float tmax = ty.y < tz.y ? ty.y : tz.y;
        times = Vec2(tmin, tmax);
        return true;
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
    times = Vec2(tmin, tmax);
    return true;
}
