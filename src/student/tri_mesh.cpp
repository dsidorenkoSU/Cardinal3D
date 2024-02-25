// #include "../rays/pathtracer.h"
#include "../rays/tri_mesh.h"
#include "debug.h"
#include <iostream>

namespace PT {

BBox Triangle::bbox() const {

    // TODO (PathTracer): Task 2
    // compute the bounding box of the triangle
    Tri_Mesh_Vert v_0 = vertex_list[v0];
    Tri_Mesh_Vert v_1 = vertex_list[v1];
    Tri_Mesh_Vert v_2 = vertex_list[v2];

    Vec3 min_bb = hmin(v_0.position, hmin(v_1.position, v_2.position));
    Vec3 max_bb = hmax(v_0.position, hmax(v_1.position, v_2.position));

    BBox box(min_bb, max_bb);

    // Beware of flat/zero-volume boxes! You may need to
    // account for that here, or later on in BBox::intersect


    // not taking care of flat/zero volume...not sure what need to be done. 

    return box;
}

Trace Triangle::hit(const Ray& ray) const {

    // Vertices of triangle - has postion and surface normal
    // See rays/tri_mesh.h for a description of this struct
    
    Tri_Mesh_Vert v_0 = vertex_list[v0];
    Tri_Mesh_Vert v_1 = vertex_list[v1];
    Tri_Mesh_Vert v_2 = vertex_list[v2];


    // TODO (PathTracer): Task 2
    // Intersect this ray with a triangle defined by the above three points.
    // Intersection should yield a ray t-value, and a hit point (u,v) on the surface of the triangle
    Vec3 e1 = v_1.position.operator-(v_0.position);
    Vec3 e2 = v_1.position.operator-(v_0.position);
    Vec3 s  = ray.point.operator-(v_0.position);
    Vec3 e1_x_d = cross(e1, ray.dir); 

    // You'll need to fill in a "Trace" struct describing information about the hit (or lack of hit)
    Trace ret;
    if (dot(e1_x_d, e2) == 0.0f) { // cant divide by 0 = no solution, ray parallel to triangle 
        ret.origin = ray.point;
        ret.hit = false;       // was there an intersection?
        ret.distance = 0.0f;   // at what distance did the intersection occur?
        ret.position = Vec3{}; // where was the intersection?
        ret.normal = Vec3{};   // what was the surface normal at the intersection?
    }
    else {
        // calculate u, v value 
        Vec3  s_x_e2 = cross(s, e2); 
        float u_cal  = -1/dot(e1_x_d, e2)*dot(s_x_e2, ray.dir);
        float v_cal  =  1/dot(e1_x_d, e2)*dot(e1_x_d, s); 
        float t_cal = -1/dot(e1_x_d, e2)*dot(s_x_e2, e1);
        // if u, v < 0 and u + v > 1 then not in the triangle or t > bound, t < 0 
        if (u_cal < 0.0f || u_cal < 0.0f || u_cal+v_cal > 1.0f || t_cal > ray.dist_bounds.y || t_cal <= 0.0f) {
            ret.origin = ray.point;
            ret.hit = false;       // was there an intersection?
            ret.distance = 0.0f;   // at what distance did the intersection occur?
            ret.position = Vec3{}; // where was the intersection?
            ret.normal = Vec3{};   // what was the surface normal at the intersection?      

        }
        else {
            ret.origin = ray.point;
            ret.hit = true;          // was there an intersection?
            ret.distance = t_cal;    // at what distance did the intersection occur?
            // std::cout << t_cal << std::endl; 
            ret.position = ray.point.operator+(ray.dir.operator*(t_cal)); // where was the intersection? 
            ret.normal = v_0.normal.operator*(1-u_cal-v_cal) + v_1.normal.operator*(u_cal) + v_2.normal.operator*(v_cal);   // what was the surface normal at the intersection? 
                                                                                                                            // (this should be interpolated between the three vertex normals)  
            ray.dist_bounds.y = t_cal; // update dist_bounds since hit. 
            //PT::Pathtracer::log_ray(ray, 10.0f);

        }
    }

    return ret;
}

Triangle::Triangle(Tri_Mesh_Vert* verts, unsigned int v0, unsigned int v1, unsigned int v2)
    : vertex_list(verts), v0(v0), v1(v1), v2(v2) {
}

void Tri_Mesh::build(const GL::Mesh& mesh) {

    verts.clear();
    triangles.clear();

    for(const auto& v : mesh.verts()) {
        verts.push_back({v.pos, v.norm});
    }

    const auto& idxs = mesh.indices();

    std::vector<Triangle> tris;
    for(size_t i = 0; i < idxs.size(); i += 3) {
        tris.push_back(Triangle(verts.data(), idxs[i], idxs[i + 1], idxs[i + 2]));
    }

    triangles.build(std::move(tris), 4);
}

Tri_Mesh::Tri_Mesh(const GL::Mesh& mesh) {
    build(mesh);
}

Tri_Mesh Tri_Mesh::copy() const {
    Tri_Mesh ret;
    ret.verts = verts;
    ret.triangles = triangles.copy();
    return ret;
}

BBox Tri_Mesh::bbox() const {
    return triangles.bbox();
}

Trace Tri_Mesh::hit(const Ray& ray) const {
    Trace t = triangles.hit(ray);
    return t;
}

size_t Tri_Mesh::visualize(GL::Lines& lines, GL::Lines& active, size_t level,
                           const Mat4& trans) const {
    return triangles.visualize(lines, active, level, trans);
}

} // namespace PT
