
#include "../rays/bvh.h"
#include "debug.h"
#include <stack>

namespace PT {

template<typename Primitive>
void BVH<Primitive>::subdivide(size_t root_node_addr, Node& node, size_t max_leaf_size) {
    // Terminate if max leaf size is reached reached
    if(node.size <= max_leaf_size) {
        return;
    }

    float SAEMin = FLT_MAX;
    int dimMin = -1;
    int pdimMin = -1;

    const int BUCKETS_COUNT = 8;

    // For each coordinate x, y, z
    for(int cind = 0; cind < 3; cind++) {

        std::vector<int> buckets;

        buckets.resize(BUCKETS_COUNT, 0);
        std::vector<BBox> bboxes;
        bboxes.resize(BUCKETS_COUNT, BBox());
        float cmin = node.bbox.min[cind];
        float cmax = node.bbox.max[cind];
        float dC = (cmax - cmin) / (BUCKETS_COUNT-1);

        // Assign primitives to buckets
        for(auto itPrim = node.start; itPrim < (size_t)(node.start + node.size); ++itPrim) {
            const BBox& bbox = primitives[itPrim].bbox();
            Vec3 c = 0.5f * (bbox.min + bbox.max);
            float bNo = (int)((c[cind] - cmin) / dC);
            bboxes[bNo].enclose(bbox);
            buckets[bNo]++;
        }

        float SN = 1.0f;
        for(int sind = 0; sind < 3; sind++) {
            if(sind != cind) SN *= node.bbox.max[sind] - node.bbox.min[sind];
        }

        // int pMin = -1;
        for(int p = 1; p < BUCKETS_COUNT; ++p) {
            BBox bl, br;
            float Na = 0.0f;
            float Nb = 0.0f;
            for(int i = 0; i < BUCKETS_COUNT; ++i) {

                // skip empty buckets
                if(buckets[i] == 0)
                    continue;

                if(i < p) {
                    Na += buckets[i];
                    bl.enclose(bboxes[i]);
                } else {
                    Nb += buckets[i];
                    br.enclose(bboxes[i]);
                }
            }

            // Only emply buckets on the left of right side
            if ((int)Na == 0 || (int)Nb == 0)
                continue;

            float SA = 1.0f;
            float SB = 1.0f;
            for(int sind = 0; sind < 3; ++sind) {
                if(sind != cind) {
                    SA *= (bl.max[sind] - bl.min[sind]);
                    SB *= (br.max[sind] - br.min[sind]);
                }
            }

            float SAE = ((SA * Na) / SN) + (SB * Nb) / SN;
            if(SAE < SAEMin) {
                SAEMin = SAE;
                // pMin = p;
                dimMin = cind;
                pdimMin = p;
            }
        }
    }

    std::vector<int> buckets;

    buckets.resize(BUCKETS_COUNT, 0);
    std::vector<BBox> bboxes;
    bboxes.resize(BUCKETS_COUNT, BBox());
    float cmin = node.bbox.min[dimMin];
    float cmax = node.bbox.max[dimMin];
    float dC = (cmax - cmin) / (BUCKETS_COUNT-1);

    // Assign primitives to buckets
    std::vector<int> lind, rind;

    // Sort all primitives in place
    // So that primitives in the left bucket come first in the primitives list
    auto ns = primitives.begin() + node.start;
    auto ne = ns + node.size;
    auto middle =
        std::partition(ns,ne, [dimMin, dC, cmin, pdimMin](const auto& em) {
                    const BBox& bboxj = em.bbox();
                    Vec3 cj = 0.5f * (bboxj.min + bboxj.max);
                    int bjNo = (int)((cj[dimMin] - cmin) / dC);
                    return bjNo < pdimMin;
        });

    // Create bounding boxes for children
    BBox split_leftBox;
    BBox split_rightBox;
    PrimitivesCIterator it = primitives.begin() + node.start;
    for(; it != middle; ++it) {
       split_leftBox.enclose(it->bbox());
    }
    for(; it != primitives.begin() + node.start + node.size; it++) {
       split_rightBox.enclose(it->bbox());
    }

    // Note that by construction in this simple example, the primitives are
    // contiguous as required. But in the students real code, students are
    // responsible for reorganizing the primitives in the primitives array so that
    // after a SAH split is computed, the chidren refer to contiguous ranges of primitives.
    int sizel = std::distance(primitives.begin() + node.start, middle);
    size_t startl = node.start;
    size_t rangel = sizel ;              // number of prims in left child
    size_t startr = node.start + sizel; // starting prim index of right child
    size_t ranger = node.size - sizel;  // number of prims in right child

    // create child nodes
    size_t node_addr_l = new_node();
    size_t node_addr_r = new_node();
    nodes[root_node_addr].l = node_addr_l;
    nodes[root_node_addr].r = node_addr_r;

    nodes[node_addr_l].bbox = split_leftBox;
    nodes[node_addr_l].start = startl;
    nodes[node_addr_l].size = rangel;

    nodes[node_addr_r].bbox = split_rightBox;
    nodes[node_addr_r].start = startr;
    nodes[node_addr_r].size = ranger;

    subdivide(node_addr_l, nodes[node_addr_l], max_leaf_size);
    subdivide(node_addr_r, nodes[node_addr_r], max_leaf_size);
}
    // construct BVH hierarchy given a vector of prims
template<typename Primitive>
void BVH<Primitive>::build(std::vector<Primitive>&& prims, size_t max_leaf_size) {

    // NOTE (PathTracer):
    // This BVH is parameterized on the type of the primitive it contains. This allows
    // us to build a BVH over any type that defines a certain interface. Specifically,
    // we use this to both build a BVH over triangles within each Tri_Mesh, and over
    // a variety of Objects (which might be Tri_Meshes, Spheres, etc.) in Pathtracer.
    //
    // The Primitive interface must implement these two functions:
    //      BBox bbox() const;
    //      Trace hit(const Ray& ray) const;
    // Hence, you may call bbox() and hit() on any value of type Primitive.

    // Keep these two lines of code in your solution. They clear the list of nodes and
    // initialize member variable 'primitives' as a vector of the scene prims
    nodes.clear();
    primitives = std::move(prims);

    // TODO (PathTracer): Task 3
    // Modify the code ahead to construct a BVH from the given vector of primitives and maximum leaf
    // size configuration.
    //
    // Please use the SAH as described in class.  We recomment the binned build from lecture.
    // In general, here is a rough sketch:
    //
    //  For each axis X,Y,Z:
    //     Try possible splits along axis, evaluate SAH for each
    //  Take minimum cost across all axes.
    //  Partition primitives into a left and right child group
    //  Compute left and right child bboxes
    //  Make the left and right child nodes.
    //
    //
    // While a BVH is conceptually a tree structure, the BVH class uses a single vector (nodes)
    // to store all the nodes. Therefore, BVH nodes don't contain pointers to child nodes,
    // but rather the indices of the
    // child nodes in this array. Hence, to get the child of a node, you have to
    // look up the child index in this vector (e.g. nodes[node.l]). Similarly,
    // to create a new node, don't allocate one yourself - use BVH::new_node, which
    // returns the index of a newly added node.
    //
    // As an example of how to make nodes, the starter code below builds a BVH with a
    // root node that encloses all the primitives and its two descendants at Level 2.
    // For now, the split is hardcoded such that the first primitive is put in the left
    // child of the root, and all the other primitives are in the right child.
    // There are no further descendants.

    // edge case
    if(primitives.empty()) {
        return;
    }

    // compute bounding box for all primitives
    BBox bb;
    for(size_t i = 0; i < primitives.size(); ++i) {
        bb.enclose(primitives[i].bbox());
    }

    //primitives[i].center

    // set up root node (root BVH). Notice that it contains all primitives.
    size_t root_node_addr = new_node();
    Node& node = nodes[root_node_addr];
    node.bbox = bb;
    node.start = 0;
    node.size = primitives.size();

    subdivide(root_node_addr, node, max_leaf_size);

}

template<typename Primitive>
void BVH<Primitive>::find_closest_hit(const Ray& ray,const Node& node, Trace& closest) const {
    if(node.is_leaf()) {
        for(PrimitivesCIterator itPrim = primitives.begin() + node.start;
            itPrim != primitives.begin() + node.start + node.size; itPrim++) {
            Trace hit = itPrim->hit(ray);
            closest = Trace::min(closest, hit);
        }
    } else {
        const Node& cl = nodes[node.l];
        const Node& cr = nodes[node.r];
        Vec2 hit[2];
        bool hl = cl.bbox.hit(ray, hit[0]);
        bool hr = cr.bbox.hit(ray, hit[1]);
        // Not hit any boxes
        if(!hl && !hr)
            return;

        // hit only one box
        if((hl && !hr) || (!hl && hr)) {
            find_closest_hit(ray, hl ? cl : cr ,closest);
            return;
        }

        // Hit two boxes, so we should check the closest first
        size_t first = hit[0].x <= hit[1].x ? node.l : node.r;
        size_t second = first == node.l ? node.r : node.l;
        // size_t second_hit = second == node.l ? 0 : 1;

        find_closest_hit(ray, nodes[first], closest);
        // Trace c1 = closest;
        //if(hit[second_hit].x < closest.distance)
            find_closest_hit(ray, nodes[second], closest);
    }
}

template<typename Primitive>
Trace BVH<Primitive>::hit(const Ray& ray) const {


    // TODO (PathTracer): Task 3
    // Implement ray - BVH intersection test. A ray intersects
    // with a BVH aggregate if and only if it intersects a primitive in
    // the BVH that is not an aggregate.

    // The starter code simply iterates through all the primitives.
    // Again, remember you can use hit() on any Primitive value.

    Trace ret;
    find_closest_hit(ray, nodes[0], ret);
    return ret;
}

template<typename Primitive>
BVH<Primitive>::BVH(std::vector<Primitive>&& prims, size_t max_leaf_size) {
    build(std::move(prims), max_leaf_size);
}

template<typename Primitive>
BVH<Primitive> BVH<Primitive>::copy() const {
    BVH<Primitive> ret;
    ret.nodes = nodes;
    ret.primitives = primitives;
    ret.root_idx = root_idx;
    return ret;
}

template<typename Primitive>
bool BVH<Primitive>::Node::is_leaf() const {
    return l == r;
}

template<typename Primitive>
size_t BVH<Primitive>::new_node(BBox box, size_t start, size_t size, size_t l, size_t r) {
    Node n;
    n.bbox = box;
    n.start = start;
    n.size = size;
    n.l = l;
    n.r = r;
    nodes.push_back(n);
    return nodes.size() - 1;
}

template<typename Primitive>
BBox BVH<Primitive>::bbox() const {
    return nodes[root_idx].bbox;
}

template<typename Primitive>
std::vector<Primitive> BVH<Primitive>::destructure() {
    nodes.clear();
    return std::move(primitives);
}

template<typename Primitive>
void BVH<Primitive>::clear() {
    nodes.clear();
    primitives.clear();
}

template<typename Primitive>
size_t BVH<Primitive>::visualize(GL::Lines& lines, GL::Lines& active, size_t level,
                                 const Mat4& trans) const {

    std::stack<std::pair<size_t, size_t>> tstack;
    tstack.push({root_idx, 0});
    size_t max_level = 0;

    if(nodes.empty()) return max_level;

    while(!tstack.empty()) {

        auto [idx, lvl] = tstack.top();
        max_level = std::max(max_level, lvl);
        const Node& node = nodes[idx];
        tstack.pop();

        Vec3 color = lvl == level ? Vec3(1.0f, 0.0f, 0.0f) : Vec3(1.0f);
        GL::Lines& add = lvl == level ? active : lines;

        BBox box = node.bbox;
        box.transform(trans);
        Vec3 min = box.min, max = box.max;

        auto edge = [&](Vec3 a, Vec3 b) { add.add(a, b, color); };

        edge(min, Vec3{max.x, min.y, min.z});
        edge(min, Vec3{min.x, max.y, min.z});
        edge(min, Vec3{min.x, min.y, max.z});
        edge(max, Vec3{min.x, max.y, max.z});
        edge(max, Vec3{max.x, min.y, max.z});
        edge(max, Vec3{max.x, max.y, min.z});
        edge(Vec3{min.x, max.y, min.z}, Vec3{max.x, max.y, min.z});
        edge(Vec3{min.x, max.y, min.z}, Vec3{min.x, max.y, max.z});
        edge(Vec3{min.x, min.y, max.z}, Vec3{max.x, min.y, max.z});
        edge(Vec3{min.x, min.y, max.z}, Vec3{min.x, max.y, max.z});
        edge(Vec3{max.x, min.y, min.z}, Vec3{max.x, max.y, min.z});
        edge(Vec3{max.x, min.y, min.z}, Vec3{max.x, min.y, max.z});

        if(node.l && node.r) {
            tstack.push({node.l, lvl + 1});
            tstack.push({node.r, lvl + 1});
        } else {
            for(size_t i = node.start; i < node.start + node.size; i++) {
                size_t c = primitives[i].visualize(lines, active, level - lvl, trans);
                max_level = std::max(c, max_level);
            }
        }
    }
    return max_level;
}

} // namespace PT
