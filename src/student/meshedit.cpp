
#include <queue>
#include <set>
#include <unordered_map>
#include <iostream>
#include "../geometry/halfedge.h"
#include "debug.h"

#include <iostream>

/* Note on local operation return types:

    The local operations all return a std::optional<T> type. This is used so that your
    implementation can signify that it does not want to perform the operation for
    whatever reason (e.g. you don't want to allow the user to erase the last vertex).

    An optional can have two values: std::nullopt, or a value of the type it is
    parameterized on. In this way, it's similar to a pointer, but has two advantages:
    the value it holds need not be allocated elsewhere, and it provides an API that
    forces the user to check if it is null before using the value.

    In your implementaiton, if you have successfully performed the operation, you can
    simply return the required reference:

            ... collapse the edge ...
            return collapsed_vertex_ref;

    And if you wish to deny the operation, you can return the null optional:

            return std::nullopt;

    Note that the stubs below all reject their duties by returning the null optional.
*/

/*
    This method should replace the given vertex and all its neighboring
    edges and faces with a single face, returning the new face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::erase_vertex(Halfedge_Mesh::VertexRef v) {

    (void)v;
    return std::nullopt;
}

/*
    This method should erase the given edge and return an iterator to the
    merged face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::erase_edge(Halfedge_Mesh::EdgeRef e) {


    (void)e;
    return std::nullopt;
}

void fill_internal_loop(Halfedge_Mesh::HalfedgeRef he, std::vector<Halfedge_Mesh::HalfedgeRef>& l){
    
    Halfedge_Mesh::HalfedgeRef t = he;
    do {
        l.push_back(t);
        t = t->next();
    } while(t != he);
}

void fill_over_vertex_loop(Halfedge_Mesh::HalfedgeRef he,
                           std::vector<Halfedge_Mesh::HalfedgeRef>& l){
    Halfedge_Mesh::HalfedgeRef t = he;
    do {
        l.push_back(t);
        l.push_back(t->twin());
        t = t->twin()->next();
    } while(t != he);
}

bool is_in_list(Halfedge_Mesh::HalfedgeRef he,const std::vector<Halfedge_Mesh::HalfedgeRef>& l){
    return std::find(l.begin(), l.end(), he) != l.end();
}

bool is_in_list(Halfedge_Mesh::EdgeRef e, const std::vector<Halfedge_Mesh::EdgeRef>& l) {
    return std::find(l.begin(), l.end(), e) != l.end();
}

Halfedge_Mesh::HalfedgeRef next_in_list(Halfedge_Mesh::HalfedgeRef he,
                                        const std::vector<Halfedge_Mesh::HalfedgeRef>& l) {
    auto it = std::find(l.begin(), l.end(), he);
    if(it == l.end()) return *l.begin();
    return *(it + 1);
}

Halfedge_Mesh::HalfedgeRef next_twin(Halfedge_Mesh::HalfedgeRef ct,
                                     const std::vector<Halfedge_Mesh::HalfedgeRef>& l0,
                                     const std::vector<Halfedge_Mesh::HalfedgeRef>& l1) {
    Halfedge_Mesh::HalfedgeRef tt = ct;
    while(true) {
        if(is_in_list(tt->twin(), l0) || is_in_list(tt->twin(), l1))
            tt = next_in_list(tt, l0);
        else
            break;
    }
    return tt->twin();
}

/*
    This method should collapse the given edge and return an iterator to
    the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_edge(Halfedge_Mesh::EdgeRef e) {

    // Phase1: Collect data
    // Halfedges
    std::vector<Halfedge_Mesh::HalfedgeRef> l0, l1, s0, s1, pdh;
    fill_internal_loop(e->halfedge(), l0);
    fill_internal_loop(e->halfedge()->twin(), l1);
    fill_over_vertex_loop(e->halfedge()->next(), s0);
    fill_over_vertex_loop(e->halfedge()->twin()->next(), s1);
    
    for(auto it = l0.begin(); it != l0.end(); ++it) {
        pdh.push_back(*it);
    }
    for(auto it = l1.begin(); it != l1.end(); ++it) {
        pdh.push_back(*it);
    }

    // halfedges and edges to erase
    std::vector<Halfedge_Mesh::EdgeRef> pde, ea;
    pde.push_back(e);
    for (auto it = l0.begin(); it < l0.end(); ++it){
        Halfedge_Mesh::EdgeRef erf = (*it)->edge();
        if(erf != edges_end() && !is_in_list(erf, pde)) {
            pde.push_back(erf);
        }   
    }
    for(auto it = l1.begin(); it < l1.end(); ++it) {
        Halfedge_Mesh::EdgeRef erf = (*it)->edge();
        if(erf != edges_end() && !is_in_list(erf, pde)) {
            pde.push_back(erf);
        }
    }
    for(auto hei = s0.begin(); hei < s0.end(); ++hei) {
        if(!is_in_list((*hei)->edge(), pde) && !is_in_list((*hei)->edge(), ea)) {
            ea.push_back((*hei)->edge());
        }
    }
    for(auto hei = s1.begin(); hei < s1.end(); ++hei) {
        if(!is_in_list((*hei)->edge(), pde) && !is_in_list((*hei)->edge(), ea)) {
            ea.push_back((*hei)->edge());
        }
    }
    // vertexes
    std::vector<Halfedge_Mesh::VertexRef> v;
    v.push_back(e->halfedge()->vertex());
    v.push_back(e->halfedge()->twin()->vertex());
    //
    
    // faces
    std::vector<Halfedge_Mesh::FaceRef> f;
    f.push_back(e->halfedge()->face());
    f.push_back(e->halfedge()->twin()->face());

    // Phase2: Create
    VertexRef v4 = new_vertex(); 
    v4->pos = (v[0]->pos + v[1]->pos) * 0.5f;
    EdgeRef ne0 = new_edge();
    EdgeRef ne1 = new_edge();
    // Phase3: Reassign 
    
    // halfedge
    for (auto hp = s0.begin(); hp<s0.end();++hp) {
        auto h = *hp;
        // don't alter halfedges sheduled for deletion
        if(is_in_list(h, l0) || is_in_list(h, l1)) 
            continue;

        h->next() = h->next();
        if(is_in_list(h->twin(), l0)) {
            auto nt = next_twin(h, l0, l1);
            h->twin() = nt;
            nt->twin() = h;
        } else if(is_in_list(h->twin(), l1)) {
            auto nt = next_twin(h, l1, l0);
            h->twin() = nt;
            nt->twin() = h;
        } 
        else {
            h->twin() = h->twin();
        }
        if(h->vertex() == v[1] || h->vertex() == v[0]) {
            h->vertex() = v4;
            v4->halfedge() = h;
        } else {
            h->vertex() = h->vertex();
        }
        if(is_in_list(h->edge(), pde)) {
            h->edge() = ne0;
            ne0->halfedge() = h;
            h->twin()->edge() = ne0;
        } else {
            h->edge() = h->edge();
        } 
        h->face() = h->face();
    }

    for(auto hp = s1.begin(); hp < s1.end(); ++hp) {
        auto h = *hp;
        // don't change halfedges sheduled for deletion
        if(is_in_list(h, l0) || is_in_list(h, l1)) 
            continue;

        h->next() = h->next();
        if(is_in_list(h->twin(), l0)) {
            auto nt = next_twin(h, l0, l1);
            h->twin() = nt;
            nt->twin() = h;
        } else if(is_in_list(h->twin(), l1)) {
            auto nt = next_twin(h, l1, l0);
            h->twin() = nt;
            nt->twin() = h;
        } else {
            h->twin() = h->twin();
        }
        if(h->vertex() == v[1] || h->vertex() == v[0]) {
            h->vertex() = v4;
            v4->halfedge() = h;
        } else {
            h->vertex() = h->vertex();
        }
        if(is_in_list(h->edge(), pde)) {
            h->edge() = ne1;
            ne1->halfedge() = h;
            h->twin()->edge() = ne1;
        } else {
            h->edge() = h->edge();
        }
        
        h->face() = h->face();     
    }
   
    for(auto hp = pdh.begin(); hp != pdh.end(); ++hp) {
        erase(*hp);
    }
    for (auto et = pde.begin(); et != pde.end(); ++et) 
        erase(*et);

    erase(f[0]);
    erase(f[1]);
    
    erase(v[0]);
    erase(v[1]);
    
    return std::optional(v4);
    //return v4;
}

/*
    This method should collapse the given face and return an iterator to
    the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_face(Halfedge_Mesh::FaceRef f) {

    (void)f;
    return std::nullopt;
}

/*
    This method should flip the given edge and return an iterator to the
    flipped edge.
*/
std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::flip_edge(Halfedge_Mesh::EdgeRef e) {

/* Steps (by Dmitrii):
     *        
     *     v2  *--------* v1, vne
     *         /         /
     *        /         /
     *       /         /
     * v3,vs *-------- * v0
     *       \   e   / 
     *        \     /
     *         \   /
     *          \ /
     *           * 
     *           v4, vns
     * 0. Collecting affected halfedges of edge e and affected points v0, v1
     * 1. Create edge (and two half edges) between points v1, v4
     * 2. Connect new halfedges and edge to existing graph
     * 2. Delete edge e and all references to it from halfedges
     */

    // Phase 0: Collect
    // HALFEDGES
    HalfedgeRef h0 = e->halfedge();
    HalfedgeRef h1 = h0->twin();
    HalfedgeRef ht = h0;
    do {
        ht = ht->next();
    } while(ht->next() != e->halfedge());
    HalfedgeRef h0e = ht;

    ht = h1;
    do {
        ht = ht->next();
    } while(ht->next() != h1);
    HalfedgeRef h1e = ht;

    // VERTICES
    VertexRef vns = e->halfedge()->twin()->next()->next()->vertex();
    VertexRef vne = e->halfedge()->next()->next()->vertex();

    // FACES
    FaceRef f0 = h0->face();
    FaceRef f1 = h1->face();

    // Phase 1: Create 
    HalfedgeRef nh0 = new_halfedge();
    HalfedgeRef nh1 = new_halfedge();
    EdgeRef ne = new_edge();

    // Phase 2: Connect new halfedges and edge
    h0e->next() = h1->next();

    nh0->next() = e->halfedge()->next()->next();
    nh0->twin() = nh1;
    nh0->vertex() = vns;
    nh0->edge() = ne;
    nh0->face() = f0;

    h0->next()->next() = nh1;
    h0->next()->face() = f1;
    h1e->next() = h0->next();
    
    nh1->next() = e->halfedge()->twin()->next()->next();
    nh1->twin() = nh0;
    nh1->vertex() = vne;
    nh1->edge() = ne;
    nh1->face() = f1;
    h1->next()->next() = nh0;
    h1->next()->face() = f0;

    // Vertexes 
    h0->vertex()->halfedge() = h1->next();
    h1->vertex()->halfedge() = h0->next();

    // edges
    ne->halfedge() = nh0;

    // faces
    f0->halfedge() = nh0; 
    f1->halfedge() = nh1;

    // Phase 3: erase old edge and halfedges
    erase(h0);
    erase(h1);
    erase(e);

    return std::optional(ne);
}

/*
    This method should split the given edge and return an iterator to the
    newly inserted vertex. The halfedge of this vertex should point along
    the edge that was split, rather than the new edges.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::split_edge(Halfedge_Mesh::EdgeRef e) {

    /* Steps (by Jerry): 

              c
            / | \
           /  |  \
          /   |   \
         a -- m -- d
          \   |   /
           \  |  /
            \ | /
              b

        0. first check face is trainalge! If not, no ops 
        1. Find the two vertices pos of a given edge (bc)
        2. acalculate middle of the two vertices 
        3. add elements to mesh 
           - new vertex m
           - m to c: 2 new half edges, 1 new edge 
           - m to a: 2 new half edges, 1 new edge
           - m to d: 2 new half edges, 1 new edge  
           - 2 new faces - mca, mdc 
        4. reassign given edge and its halfedge   
        5. return an iterator to the newly inserted vertex (m)
    */
    
    // const vs non const ref? 
    // 0. 
    if (e->halfedge() != e->halfedge()->next()->next()->next()){
        std::cout << "not a traingle mesh or it's boundary -> no ops" << std::endl; 
        return std::nullopt;
    }
    if (e->halfedge()->twin() != e->halfedge()->twin()->next()->next()->next()){
        std::cout << "not a traingle mesh or it's boundary -> no ops" << std::endl; 
        return std::nullopt;
    }
    // 1. 
    // an edge will point arbitrarily to either its "left" or "right" halfedge, use twin to get both 
    VertexRef vertex_b = e->halfedge()->vertex(); // 1st vertex 
    VertexRef vertex_c = e->halfedge()->twin()->vertex(); // 2nd vertex 

    // 2. calculate middle of two pos    
    Vec3 pos_middle = vertex_b->pos.operator+(vertex_c->pos);    
    pos_middle = pos_middle.operator*(0.5);

    // 3. 
    VertexRef vertex_m = new_vertex(); // initialize new vertex, m 
    HalfedgeRef half_m_c = new_halfedge(); 
    HalfedgeRef half_c_m = new_halfedge(); 
    HalfedgeRef half_m_d = new_halfedge(); 
    HalfedgeRef half_d_m = new_halfedge(); 
    HalfedgeRef half_m_a = new_halfedge(); 
    HalfedgeRef half_a_m = new_halfedge(); 
    FaceRef face_mca = new_face();
    FaceRef face_mdc = new_face();
    EdgeRef edge_mc = new_edge(); 
    EdgeRef edge_ma = new_edge(); 
    EdgeRef edge_md = new_edge(); 

    vertex_m->pos = pos_middle; // assign pos to m 
    // vertex_m->halfedge() = e->halfedge()->twin(); // The halfedge of this vertex should point along the edge that was split, rather than the new edges.
    vertex_m->halfedge() = half_m_c; // The halfedge of this vertex should point along the edge that was split, rather than the new edges.

    half_m_c->twin() = half_c_m;
    half_m_c->next() = e->halfedge()->next(); 
    half_m_c->vertex() = vertex_m; 
    half_m_c->edge() = edge_mc;
    half_m_c->face() = face_mca; 

    half_c_m->twin() = half_m_c; 
    half_c_m->next() = half_m_d; 
    half_c_m->vertex() = vertex_c; 
    half_c_m->edge() = edge_mc;
    half_c_m->face() = face_mdc;

    half_m_d->twin() = half_d_m;
    half_m_d->next() = e->halfedge()->twin()->next()->next(); // assume triangle....
    half_m_d->vertex() = vertex_m; 
    half_m_d->edge() = edge_md;
    half_m_d->face() = face_mdc; 

    half_d_m->twin() = half_m_d; 
    half_d_m->next() = e->halfedge()->twin(); 
    half_d_m->vertex() = e->halfedge()->twin()->next()->next()->vertex(); 
    half_d_m->edge() = edge_md;
    half_d_m->face() = e->halfedge()->twin()->face(); // original face 

    half_m_a->twin() = half_a_m;
    half_m_a->next() = e->halfedge()->next()->next(); // assume triangle....
    half_m_a->vertex() = vertex_m; 
    half_m_a->edge() = edge_ma;
    half_m_a->face() = e->halfedge()->face(); // original face  

    half_a_m->twin() = half_m_a; 
    half_a_m->next() = half_m_c; 
    half_a_m->vertex() = e->halfedge()->next()->next()->vertex(); 
    half_a_m->edge() = edge_ma;
    half_a_m->face() = face_mca;   

    edge_mc->halfedge() = half_m_c; // arbitrary choose m_c halfedge 
    edge_ma->halfedge() = half_m_a; // arbitrary choose m_a halfedge 
    edge_md->halfedge() = half_m_d; // arbitrary choose m_d halfedge 
    
    face_mca->halfedge() = half_m_c; // arbitrary choose m_c halfedge 
    face_mdc->halfedge() = half_m_d; // arbitrary choose m_d halfedge 

    // 4. reassign 
    // make sure vertex_c halfedge is point to correct one even it is already 
    vertex_c->halfedge() = e->halfedge()->next(); 

    // update twin's vertex (m_b)
    e->halfedge()->twin()->vertex() = vertex_m;

    // update face for exisiting half edges (c_a, d_c)
    e->halfedge()->next()->face() = face_mca; 
    e->halfedge()->twin()->next()->next()->face() = face_mdc;

    // update next for existing halfedges, order of these 2 line is important  
    e->halfedge()->twin()->next()->next()->next() = half_c_m; // d_c first 
    e->halfedge()->twin()->next()->next() = half_d_m; // then b_c
    
    // update next for existing halfedges, order of this two line is important  
    e->halfedge()->next()->next() = half_a_m; // c_a first 
    e->halfedge()->next() = half_m_a; // then halfedge, b_m

    // make sure previous face point to correct half edge even it is already 
    e->halfedge()->face()->halfedge() = half_m_a; 
    e->halfedge()->twin()->face()->halfedge() = half_d_m;  

    // debug 
    // std::cout << "1st vertex: " << pos1 << std::endl; 
    // std::cout << "2nd vertex: " << pos2 << std::endl; 
    // std::cout << "middle: " << pos_middle << std::endl << std::endl; 

    // (void)e;
    return std::optional(vertex_m);
}

/* Note on the beveling process:

    Each of the bevel_vertex, bevel_edge, and bevel_face functions do not represent
    a full bevel operation. Instead, they should update the _connectivity_ of
    the mesh, _not_ the positions of newly created vertices. In fact, you should set
    the positions of new vertices to be exactly the same as wherever they "started from."

    When you click on a mesh element while in bevel mode, one of those three functions
    is called. But, because you may then adjust the distance/offset of the newly
    beveled face, we need another method of updating the positions of the new vertices.

    This is where bevel_vertex_positions, bevel_edge_positions, and
    bevel_face_positions come in: these functions are called repeatedly as you
    move your mouse, the position of which determins the normal and tangent offset
    parameters. These functions are also passed an array of the original vertex
    positions: for  bevel_vertex, it has one element, the original vertex position,
    for bevel_edge,  two for the two vertices, and for bevel_face, it has the original
    position of each vertex in halfedge order. You should use these positions, as well
    as the normal and tangent offset fields to assign positions to the new vertices.

    Finally, note that the normal and tangent offsets are not relative values - you
    should compute a particular new position from them, not a delta to apply.
*/

/*
    This method should replace the vertex v with a face, corresponding to
    a bevel operation. It should return the new face.  NOTE: This method is
    responsible for updating the *connectivity* of the mesh only---it does not
    need to update the vertex positions.  These positions will be updated in
    Halfedge_Mesh::bevel_vertex_positions (which you also have to
    implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_vertex(Halfedge_Mesh::VertexRef v) {

    // Reminder: You should set the positions of new vertices (v->pos) to be exactly
    // the same as wherever they "started from."

    (void)v;
    return std::nullopt;
}

/*
    This method should replace the edge e with a face, corresponding to a
    bevel operation. It should return the new face. NOTE: This method is
    responsible for updating the *connectivity* of the mesh only---it does not
    need to update the vertex positions.  These positions will be updated in
    Halfedge_Mesh::bevel_edge_positions (which you also have to
    implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_edge(Halfedge_Mesh::EdgeRef e) {

    // Reminder: You should set the positions of new vertices (v->pos) to be exactly
    // the same as wherever they "started from."

    (void)e;
    return std::nullopt;
}

/*
    This method should replace the face f with an additional, inset face
    (and ring of faces around it), corresponding to a bevel operation. It
    should return the new face.  NOTE: This method is responsible for updating
    the *connectivity* of the mesh only---it does not need to update the vertex
    positions. These positions will be updated in
    Halfedge_Mesh::bevel_face_positions (which you also have to
    implement!)
*/
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_face(Halfedge_Mesh::FaceRef f) {

   /* Steps (by Jerry): 

    Top view (if square face)
         
                  half_n0  edge_new        
              new - - - - - - 
              |   half_n1    |
      half_c0 | half_c1      |   
              |    face_c    |
       edge_c |              |
              |  half_orig   |
              a - - - - - -   

    */
    // Reminder: You should set the positions of new vertices (v->pos) to be exactly
    // the same as wherever they "started from."
    

    // 0. accumlate list of original halfedges 
    std::vector<HalfedgeRef> new_halfedges;
    auto h = f->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != f->halfedge());

    // 1. loop around face to count and intilize and assign if possible 
    // int count = 0; 
    // HalfedgeRef half_origin = f->halfedge();

    for (size_t i = 0; i < new_halfedges.size(); i++) {
        
        HalfedgeRef half_origin = new_halfedges[i];
        VertexRef vertex_a = half_origin->vertex(); 
        
        // create 1 new face, 2 edges, 4 half edges, 1 vertex    
        FaceRef face_connect = new_face(); 
        EdgeRef edge_connect = new_edge(); 
        EdgeRef edge_new = new_edge(); 
        HalfedgeRef half_connect0 = new_halfedge(); 
        HalfedgeRef half_connect1 = new_halfedge(); 
        HalfedgeRef half_new0 = new_halfedge(); 
        HalfedgeRef half_new1 = new_halfedge(); 
        VertexRef vertex_new = new_vertex(); 
        
        half_connect1->twin() = half_connect0; 
        half_connect1->next() = half_origin;   
        half_connect1->vertex() = vertex_new; 
        half_connect1->edge() = edge_connect;
        half_connect1->face() = face_connect; 

        half_connect0->twin() = half_connect1; 
        // half_connect0->next() assign later   --  update in step 2
        half_connect0->vertex() = vertex_a; 
        half_connect0->edge() = edge_connect;
        // half_connect0->face() assign later,  -- update in step 2

        half_new1->twin() = half_new0; 
        half_new1->next() = half_connect1;   
        // half_new1->vertex() assign later  -- update in step 2
        half_new1->edge() = edge_new;
        half_new1->face() = face_connect; 

        half_new0->twin() = half_new1; 
        // half_new0->next() assign later     -- updatein step 2 
        half_new0->vertex() = vertex_new; 
        half_new0->edge() = edge_new;
        half_new0->face() = f; // original face (f to be updated later)

        edge_new->halfedge() = half_new1;         // arbitrary choose new1
        edge_connect->halfedge() = half_connect1; // arbitrary choose connect1        
        
        face_connect->halfedge() = half_connect1; 

        vertex_new->pos = vertex_a->pos; // set the positions of new vertices to be the same as starting 
        vertex_new->halfedge() = half_new0; // choose new0 

        // half_origin->twin() no need to update
        // half_origin->next() assign later, update in step 2 
        half_origin->face() = face_connect; 
        // half_connect1->vertex() no update 
        // half_connect1->edge() no update 

        vertex_a->halfedge() = half_connect0; // reassign to half_origin. 

        // next one 
        // half_origin = half_origin->next();
        // count++;
    } 

    // 2. loop around face again to finish assignment 
    
    for (size_t i = 0; i < new_halfedges.size(); i++) {
        
        HalfedgeRef half_origin = new_halfedges[i];
        FaceRef face_connect = half_origin->face();
        HalfedgeRef half_connect1 = face_connect->halfedge(); 
        size_t temp = i+1; 
        if (temp == new_halfedges.size()) {
            temp = 0; 
        }
        HalfedgeRef half_origin_next = new_halfedges[temp];
        HalfedgeRef half_connect0_across = half_origin_next->face()->halfedge()->twin(); 
        HalfedgeRef half_new0 = half_connect1->vertex()->halfedge();
        HalfedgeRef half_new1 = half_new0->twin();
        
        half_connect0_across->face() = face_connect;
        half_connect0_across->next() = half_new1;
        half_new1->vertex() = half_connect0_across->twin()->vertex();         
        half_new0->next() = half_connect0_across->twin()->vertex()->halfedge(); 
        half_origin->next() = half_connect0_across; 
    }

    // // 3. update f->halfedge()->next()
    // VertexRef vertex_a = half_origin->vertex(); 
    // for (int i = 0; i < count; i++) {
    //     HalfedgeRef half_origin1 = vertex_a->halfedge();
    //     HalfedgeRef half_connect0_across = vertex_a->halfedge()->next()->face()->halfedge()->twin();
    //     vertex_a = vertex_a->halfedge()->next()->vertex(); // go to next vertex_a

    //     half_origin1->next() = half_connect0_across; 
    // } 

    // 4. update f halfedge 
    f->halfedge() = f->halfedge()->next()->next()->twin();

    return std::optional(f);
}
/*
    Compute new vertex positions for the vertices of the beveled vertex.

    These vertices can be accessed via new_halfedges[i]->vertex()->pos for
    i = 1, ..., new_halfedges.size()-1.

    The basic strategy here is to loop over the list of outgoing halfedges,
    and use the original vertex position and its associated outgoing edge
    to compute a new vertex position along the outgoing edge.
*/
void Halfedge_Mesh::bevel_vertex_positions(const std::vector<Vec3>& start_positions,
                                           Halfedge_Mesh::FaceRef face, float tangent_offset) {

    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
}

/*
    Compute new vertex positions for the vertices of the beveled edge.

    These vertices can be accessed via new_halfedges[i]->vertex()->pos for
    i = 1, ..., new_halfedges.size()-1.

    The basic strategy here is to loop over the list of outgoing halfedges,
    and use the preceding and next vertex position from the original mesh
    (in the orig array) to compute an offset vertex position.

    Note that there is a 1-to-1 correspondence between halfedges in
    newHalfedges and vertex positions
    in orig.  So, you can write loops of the form

    for(size_t i = 0; i < new_halfedges.size(); i++)
    {
            Vector3D pi = start_positions[i]; // get the original vertex
            position corresponding to vertex i
    }
*/
void Halfedge_Mesh::bevel_edge_positions(const std::vector<Vec3>& start_positions,
                                         Halfedge_Mesh::FaceRef face, float tangent_offset) {

    std::vector<HalfedgeRef> new_halfedges;
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    (void)new_halfedges;
    (void)start_positions;
    (void)face;
    (void)tangent_offset;
}

/*
    Compute new vertex positions for the vertices of the beveled face.

    These vertices can be accessed via new_halfedges[i]->vertex()->pos for
    i = 1, ..., new_halfedges.size()-1.

    The basic strategy here is to loop over the list of outgoing halfedges,
    and use the preceding and next vertex position from the original mesh
    (in the start_positions array) to compute an offset vertex
    position.

    Note that there is a 1-to-1 correspondence between halfedges in
    new_halfedges and vertex positions
    in orig. So, you can write loops of the form

    for(size_t i = 0; i < new_halfedges.size(); i++)
    {
            Vec3 pi = start_positions[i]; // get the original vertex
            position corresponding to vertex i
    }

    Finally, note that the normal and tangent offsets are not relative values - you
    should compute a particular new position from them, not a delta to apply.

*/
void Halfedge_Mesh::bevel_face_positions(const std::vector<Vec3>& start_positions,
                                         Halfedge_Mesh::FaceRef face, float tangent_offset,
                                         float normal_offset) {

    // normal_offset is relative to face 
    
    if (flip_orientation) normal_offset = -normal_offset;
    
    std::vector<HalfedgeRef> new_halfedges;

    // create a list of halfedges to loop 
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    // find the normal vector of the face
    //Vec3 v10 = new_halfedges[1]->vertex()->pos - new_halfedges[0]->vertex()->pos; 
    //Vec3 v20 = new_halfedges[2]->vertex()->pos - new_halfedges[0]->vertex()->pos; // start_positions[2]-start_positions[0]; 
    Vec3 v10 = start_positions[1]-start_positions[0]; 
    Vec3 v20 = start_positions[2]-start_positions[0]; 
    Vec3 normal = cross(v10, v20).unit();  

    // calculate tangent vector 
    // set new pos 
    for (size_t i = 0; i < new_halfedges.size(); i++) {
        Vec3 pi = start_positions[i]; // get the original vertex pos 
        int temp_pre = i-1; 
        if (temp_pre == -1) {
            temp_pre = new_halfedges.size()-1; 
        }
        Vec3 pi_pre = start_positions[temp_pre]; // get the original previous vertex pos 
        
        size_t temp_next = i+1; 
        if (temp_next == new_halfedges.size()) {
            temp_next = 0; 
        }
        Vec3 pi_next = start_positions[temp_next]; // get the original next vertex pos 

        Vec3 temp_vec = pi.operator+(normal.operator*(-normal_offset));
        Vec3 tangent = ((pi_next.operator-(pi)).operator+(pi_pre.operator-(pi))).unit(); 
        new_halfedges[i]->vertex()->pos = temp_vec.operator+(tangent.operator*(-tangent_offset));
    }
    // std::cout << normal_offset << std::endl;
    // std::cout << tangent_offset << std::endl << std::endl;

}
/*
    Splits all non-triangular faces into triangles.
*/
void Halfedge_Mesh::triangulate() {

    for (FaceRef f = faces_begin(); f != faces_end(); ++f) {
        
        // Collect half edges
        std::vector<HalfedgeRef> h;
        HalfedgeRef t = f->halfedge();
        do {
            h.push_back(t);
            t = t->next();
        } 
        while (t != f->halfedge());
        
        // Skip triangles  
        if (h.size() <=3 ) {
            continue;
        }

        // Collect vertices 
        VertexRef v0 = f->halfedge()->vertex();

        // Triangulate convex polygon
        // for each halfedge (except first and last) 
        // do the following steps
        // 1. Create edge and two halfedges  
        // 2. Create face
        // 3. Link them together 
        HalfedgeRef ph = h[0];
        std::vector<FaceRef> nf;
        for(int i = 1; i < h.size() - 1; ++i) {
            EdgeRef ne = new_edge();
            
            HalfedgeRef nh0 = new_halfedge();
            HalfedgeRef nh1 = new_halfedge();

            nf.push_back(new_face());

            // Assign halfedges
            h[i]->next() = nh0;
            h[i]->twin() = h[i]->twin();
            h[i]->vertex() = h[i]->vertex();
            h[i]->edge() = h[i]->edge();
            h[i]->face() = nf.back();

            nh0->next() = ph;
            nh0->twin() = nh1;
            nh0->vertex() = h[i]->next()->vertex();
            nh0->edge() = ne;
            nh0->face() = nf.back();

            nh1->next() = h[i+1];
            nh1->twin() = nh0;
            nh1->vertex() = v0;
            nh1->edge() = ne;
            //nh1->face() = <--- will be created on the next iteration
            
            ph->face() = nf.back();

            // assign edge
            ne->halfedge() = nh0;

            // assign face
            nf.back()->halfedge() = ph;
            
            EdgeRef ei = h[i]->edge();
            ei->halfedge() = ei->halfedge();

            // Track previous halfedge
            ph = nh1;
        }

        ph->face() = nf.back();
        h[0]->face() = nf[0];
        h.back()->face() = nf.back();

        // Delete old face
        erase(f);
    }
}

/* Note on the quad subdivision process:

        Unlike the local mesh operations (like bevel or edge flip), we will perform
        subdivision by splitting *all* faces into quads "simultaneously."  Rather
        than operating directly on the halfedge data structure (which as you've
        seen is quite difficult to maintain!) we are going to do something a bit nicer:
           1. Create a raw list of vertex positions and faces (rather than a full-
              blown halfedge mesh).
           2. Build a new halfedge mesh from these lists, replacing the old one.
        Sometimes rebuilding a data structure from scratch is simpler (and even
        more efficient) than incrementally modifying the existing one.  These steps are
        detailed below.

  Step I: Compute the vertex positions for the subdivided mesh.
        Here we're going to do something a little bit strange: since we will
        have one vertex in the subdivided mesh for each vertex, edge, and face in
        the original mesh, we can nicely store the new vertex *positions* as
        attributes on vertices, edges, and faces of the original mesh. These positions
        can then be conveniently copied into the new, subdivided mesh.
        This is what you will implement in linear_subdivide_positions() and
        catmullclark_subdivide_positions().

  Steps II-IV are provided (see Halfedge_Mesh::subdivide()), but are still detailed
  here:

  Step II: Assign a unique index (starting at 0) to each vertex, edge, and
        face in the original mesh. These indices will be the indices of the
        vertices in the new (subdivided mesh).  They do not have to be assigned
        in any particular order, so long as no index is shared by more than one
        mesh element, and the total number of indices is equal to V+E+F, i.e.,
        the total number of vertices plus edges plus faces in the original mesh.
        Basically we just need a one-to-one mapping between original mesh elements
        and subdivided mesh vertices.

  Step III: Build a list of quads in the new (subdivided) mesh, as tuples of
        the element indices defined above. In other words, each new quad should be
        of the form (i,j,k,l), where i,j,k and l are four of the indices stored on
        our original mesh elements.  Note that it is essential to get the orientation
        right here: (i,j,k,l) is not the same as (l,k,j,i).  Indices of new faces
        should circulate in the same direction as old faces (think about the right-hand
        rule).

  Step IV: Pass the list of vertices and quads to a routine that clears
        the internal data for this halfedge mesh, and builds new halfedge data from
        scratch, using the two lists.
*/

/*
    Compute new vertex positions for a mesh that splits each polygon
    into quads (by inserting a vertex at the face midpoint and each
    of the edge midpoints).  The new vertex positions will be stored
    in the members Vertex::new_pos, Edge::new_pos, and
    Face::new_pos.  The values of the positions are based on
    simple linear interpolation, e.g., the edge midpoints and face
    centroids.
*/
void Halfedge_Mesh::linear_subdivide_positions() {

    // For each vertex, assign Vertex::new_pos to
    // its original position, Vertex::pos.

    for (VertexRef v = vertices_begin(); v != vertices_end(); v++) {
        v->new_pos = v->pos;

    }

    // For each edge, assign the midpoint of the two original
    // positions to Edge::new_pos.
    for (EdgeRef e = edges_begin(); e != edges_end(); e++) {
        // Vec3 v1 = e->halfedge()->vertex()->pos;
        // Vec3 v2 = e->halfedge()->twin()->vertex()->pos;

        e->new_pos = e->center();
    }

    // For each face, assign the centroid (i.e., arithmetic mean)
    // of the original vertex positions to Face::new_pos. Note
    // that in general, NOT all faces will be triangles!
    for(FaceRef f = faces_begin(); f != faces_end(); f++) {

        
        f->new_pos = f->center();

    }
}

/*
    Compute new vertex positions for a mesh that splits each polygon
    into quads (by inserting a vertex at the face midpoint and each
    of the edge midpoints).  The new vertex positions will be stored
    in the members Vertex::new_pos, Edge::new_pos, and
    Face::new_pos.  The values of the positions are based on
    the Catmull-Clark rules for subdivision.

    Note: this will only be called on meshes without boundary
*/
void Halfedge_Mesh::catmullclark_subdivide_positions() {

    // For each face, assign the centroid (i.e., arithmetic mean)
    for(FaceRef f = faces_begin(); f != faces_end(); f++) {

        f->new_pos = f->center();
    }

    // Edges
    for(EdgeRef e = edges_begin(); e != edges_end(); e++) {

        e->new_pos = e->center();
    }

    // For each vertex, assign Vertex::new_pos to
    // its original position, Vertex::pos.
    for(VertexRef v = vertices_begin(); v != vertices_end(); v++) {
        
       std::vector<FaceRef> vfaces;
       HalfedgeRef href = v->halfedge(); 
       std::vector<EdgeRef> vedges;
       std::vector<VertexRef> vvert;

       do {
          vfaces.push_back(href->face());
          vedges.push_back(href->edge());
          vvert.push_back(href->twin()->vertex());
          href = href->twin()->next();
       } while(href != v->halfedge());

       Vec3 Q;
       for (std::vector<FaceRef>::const_iterator cf = vfaces.begin(); cf != vfaces.end(); ++cf){
          Q += (*cf)->new_pos;
       }
       Q /= (float)vfaces.size();
       Vec3 R;
       for (std::vector<EdgeRef>::const_iterator er = vedges.begin(); er != vedges.end(); ++er){
          R += (*er)->new_pos;
       }
       R /= (float)vedges.size();



       v->new_pos = (Q + 2*R + (vfaces.size()-3) * v->pos) / (float)vfaces.size();
    }
}

/*
        This routine should increase the number of triangles in the mesh
        using Loop subdivision. Note: this is will only be called on triangle meshes.
*/
void Halfedge_Mesh::loop_subdivide() {

    // Compute new positions for all the vertices in the input mesh, using
    // the Loop subdivision rule, and store them in Vertex::new_pos.
    // -> At this point, we also want to mark each vertex as being a vertex of the
    //    original mesh. Use Vertex::is_new for this.
    // -> Next, compute the updated vertex positions associated with edges, and
    //    store it in Edge::new_pos.
    // -> Next, we're going to split every edge in the mesh, in any order.  For
    //    future reference, we're also going to store some information about which
    //    subdivided edges come from splitting an edge in the original mesh, and
    //    which edges are new, by setting the flat Edge::is_new. Note that in this
    //    loop, we only want to iterate over edges of the original mesh.
    //    Otherwise, we'll end up splitting edges that we just split (and the
    //    loop will never end!)
    // -> Now flip any new edge that connects an old and new vertex.
    // -> Finally, copy the new vertex positions into final Vertex::pos.

    // Each vertex and edge of the original surface can be associated with a
    // vertex in the new (subdivided) surface.
    // Therefore, our strategy for computing the subdivided vertex locations is to
    // *first* compute the new positions
    // using the connectivity of the original (coarse) mesh; navigating this mesh
    // will be much easier than navigating
    // the new subdivided (fine) mesh, which has more elements to traverse.  We
    // will then assign vertex positions in
    // the new mesh based on the values we computed for the original mesh.

    // Compute updated positions for all the vertices in the original mesh, using
    // the Loop subdivision rule.

    // Next, compute the updated vertex positions associated with edges.

    // Next, we're going to split every edge in the mesh, in any order. For
    // future reference, we're also going to store some information about which
    // subdivided edges come from splitting an edge in the original mesh, and
    // which edges are new.
    // In this loop, we only want to iterate over edges of the original
    // mesh---otherwise, we'll end up splitting edges that we just split (and
    // the loop will never end!)

    // Finally, flip any new edge that connects an old and new vertex.

    // Copy the updated vertex positions to the subdivided mesh.
}

/*
    Isotropic remeshing. Note that this function returns success in a similar
    manner to the local operations, except with only a boolean value.
    (e.g. you may want to return false if this is not a triangle mesh)
*/
bool Halfedge_Mesh::isotropic_remesh() {

    // Compute the mean edge length.
    // Repeat the four main steps for 5 or 6 iterations
    // -> Split edges much longer than the target length (being careful about
    //    how the loop is written!)
    // -> Collapse edges much shorter than the target length.  Here we need to
    //    be EXTRA careful about advancing the loop, because many edges may have
    //    been destroyed by a collapse (which ones?)
    // -> Now flip each edge if it improves vertex degree
    // -> Finally, apply some tangential smoothing to the vertex positions

    // Note: if you erase elements in a local operation, they will not be actually deleted
    // until do_erase or validate are called. This is to facilitate checking
    // for dangling references to elements that will be erased.
    // The rest of the codebase will automatically call validate() after each op,
    // but here simply calling collapse_edge() will not erase the elements.
    // You should use collapse_edge_erase() instead for the desired behavior.

    return false;
}

/* Helper type for quadric simplification */
struct Edge_Record {
    Edge_Record() {
    }
    Edge_Record(std::unordered_map<Halfedge_Mesh::VertexRef, Mat4>& vertex_quadrics,
                Halfedge_Mesh::EdgeRef e)
        : edge(e) { // initialize Edge_record edge to e 

        // Compute the combined quadric from the edge endpoints.

        Halfedge_Mesh::VertexRef v1 = e->halfedge()->vertex(); 
        Halfedge_Mesh::VertexRef v2 = e->halfedge()->twin()->vertex();
        Mat4 Q1 = vertex_quadrics.at(v1);
        Mat4 Q2 = vertex_quadrics.at(v2);

        //  
        // -> Build the 3x3 linear system whose solution minimizes the quadric error
        //    associated with these two endpoints.
        Mat4 Qopt = (Q1.operator+(Q2)).operator*(0.5f); // use error at mid point for now

        // -> Use this system to solve for the optimal position, and store it in
        //    Edge_Record::optimal.
        Edge_Record::optimal = 0.5f*(v1->pos + v2->pos); // use mid point for now 
        // -> Also store the cost associated with collapsing this edge in
        //    Edge_Record::cost.

        Vec4 u(optimal.x, optimal.y, optimal.z, 1.0);
        Vec4 u_Qopt(dot(u, Qopt.operator[](0)), dot(u, Qopt.operator[](1)), dot(u, Qopt.operator[](2)), dot(u, Qopt.operator[](3))); 
        Edge_Record::cost = dot(u_Qopt, u);
    }
    Halfedge_Mesh::EdgeRef edge;
    Vec3 optimal;
    float cost;
};

/* Comparison operator for Edge_Records so std::set will properly order them */
bool operator<(const Edge_Record& r1, const Edge_Record& r2) {
    if(r1.cost != r2.cost) {
        return r1.cost < r2.cost;
    }
    // if equal....? what does &*e1 mean? 
    Halfedge_Mesh::EdgeRef e1 = r1.edge; 
    Halfedge_Mesh::EdgeRef e2 = r2.edge;
    return &*e1 < &*e2;
}

/** Helper type for quadric simplification
 *
 * A PQueue is a minimum-priority queue that
 * allows elements to be both inserted and removed from the
 * queue.  Together, one can easily change the priority of
 * an item by removing it, and re-inserting the same item
 * but with a different priority.  A priority queue, for
 * those who don't remember or haven't seen it before, is a
 * data structure that always keeps track of the item with
 * the smallest priority or "score," even as new elements
 * are inserted and removed.  Priority queues are often an
 * essential component of greedy algorithms, where one wants
 * to iteratively operate on the current "best" element.
 *
 * PQueue is templated on the type T of the object
 * being queued.  For this reason, T must define a comparison
 * operator of the form
 *
 *    bool operator<( const T& t1, const T& t2 )
 *
 * which returns true if and only if t1 is considered to have a
 * lower priority than t2.
 *
 * Basic use of a PQueue might look
 * something like this:
 *
 *    // initialize an empty queue
 *    PQueue<myItemType> queue;
 *
 *    // add some items (which we assume have been created
 *    // elsewhere, each of which has its priority stored as
 *    // some kind of internal member variable)
 *    queue.insert( item1 );
 *    queue.insert( item2 );
 *    queue.insert( item3 );
 *
 *    // get the highest priority item currently in the queue
 *    myItemType highestPriorityItem = queue.top();
 *
 *    // remove the highest priority item, automatically
 *    // promoting the next-highest priority item to the top
 *    queue.pop();
 *
 *    myItemType nextHighestPriorityItem = queue.top();
 *
 *    // Etc.
 *
 *    // We can also remove an item, making sure it is no
 *    // longer in the queue (note that this item may already
 *    // have been removed, if it was the 1st or 2nd-highest
 *    // priority item!)
 *    queue.remove( item2 );
 *
 */
template<class T> struct PQueue {
    void insert(const T& item) {
        queue.insert(item);
    }
    void remove(const T& item) {
        if(queue.find(item) != queue.end()) {
            queue.erase(item);
        }
    }
    const T& top(void) const {
        return *(queue.begin());
    }
    void pop(void) {
        queue.erase(queue.begin());
    }
    size_t size() {
        return queue.size();
    }

    std::set<T> queue;
};

/*
    Mesh simplification. Note that this function returns success in a similar
    manner to the local operations, except with only a boolean value.
    (e.g. you may want to return false if you can't simplify the mesh any
    further without destroying it.)
*/
bool Halfedge_Mesh::simplify() {

    std::unordered_map<VertexRef, Mat4> vertex_quadrics;
    std::unordered_map<FaceRef, Mat4> face_quadrics;
    std::unordered_map<EdgeRef, Edge_Record> edge_records;
    PQueue<Edge_Record> edge_queue; 

    // Compute initial quadrics for each face by simply writing the plane equation
    // for the face in homogeneous coordinates. These quadrics should be stored
    // in face_quadrics
    for(FaceRef f = faces_begin(); f != faces_end(); f++) {
        // to find plane equation   
        Vec3 p0 = f->halfedge()->vertex()->pos; 
        Vec3 p1 = f->halfedge()->next()->vertex()->pos;
        Vec3 p2 = f->halfedge()->next()->next()->vertex()->pos;
        Vec3 p3 = f->halfedge()->next()->next()->next()->vertex()->pos;

        // check if traingle only mesh 
        if (p0 != p3) {
            std::cout << "not a traingle only mesh!" << std::endl; 
            return false;
        }
        
        Vec3 normal = cross((p1.operator-(p0)), (p2.operator-(p0))); 
        float d = -dot(normal, p0); 

        Vec4 col1(normal.x*normal.x, normal.y*normal.x, normal.x*normal.z, normal.x*d);
        Vec4 col2(normal.x*normal.y, normal.y*normal.y, normal.y*normal.z, normal.y*d);
        Vec4 col3(normal.x*normal.z, normal.y*normal.z, normal.z*normal.z, normal.z*d);
        Vec4 col4(normal.x*d, normal.y*d, normal.z*d, d*d);

        Mat4 Q(col1, col2, col3, col4);  
        face_quadrics[f] = Q; 
    }

    // -> Compute an initial quadric for each vertex as the sum of the quadrics
    //    associated with the incident faces, storing it in vertex_quadrics
    for (VertexRef v = vertices_begin(); v != vertices_end(); v++) {

        // loop through face surrounding v 
        HalfedgeRef h = v->halfedge();
        Mat4 Q_sum = Mat4::Zero;  
        do {
            FaceRef f_tmp = h->face(); 
             // Searching for key
            Q_sum = Q_sum.operator+=(face_quadrics.at(f_tmp));
            h = h->twin()->next();
        } while(h != v->halfedge());
        //std::cout << v->degree() << std::endl;

        vertex_quadrics[v] = Q_sum;
    }

    // -> Build a priority queue of edges according to their quadric error cost,
    //    i.e., by building an Edge_Record for each edge and sticking it in the
    //    queue. You may want to use the above PQueue<Edge_Record> for this.
    int count = 0; 
    for (EdgeRef e = edges_begin(); e != edges_end(); e++) {
        Edge_Record edge_r(vertex_quadrics, e); 
        edge_records[e] = edge_r;
        edge_queue.insert(edge_r); 
        count++; // count the total number of edge 
    }

    if (count < 30) {
        std::cout << "Cant simplify anymore!" << std::endl; 
        return false; 
    }
    
    // -> Until we reach the target edge budget, collapse the best edge. Remember
    //    to remove from the queue any edge that touches the collapsing edge
    //    BEFORE it gets collapsed, and add back into the queue any edge touching
    //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
    //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
    //    top of the queue.
    int target_num = count/2; 
    for (int i = 0; i < target_num; i++) {
        Edge_Record e_rm_record = edge_queue.top();
        EdgeRef e_rm = e_rm_record.edge; 
        VertexRef v1 = e_rm->halfedge()->vertex(); 
        VertexRef v2 = e_rm->halfedge()->twin()->vertex(); 

        // loop through edge surrounding v1 and remove e_tmp from queue but not e_rm 
        HalfedgeRef h1 = v1->halfedge();
        // int count_temp = 0; 
        do {
            EdgeRef e_tmp = h1->edge(); 
            if (e_tmp != e_rm) {
                edge_queue.remove(edge_records[e_tmp]); 
                edge_records.erase(e_tmp); 
            }
            h1 = h1->twin()->next();
            // std::cout << count_temp << std::endl; 
        } while(h1 != v1->halfedge());

        // loop through edge surrounding v2 and remove e_tmp from queue but not e_rm
        HalfedgeRef h2 = v2->halfedge();
        int count_temp = 0; 
        do {
            EdgeRef e_tmp = h2->edge(); 
            if (e_tmp != e_rm) {
                edge_queue.remove(edge_records[e_tmp]); 
                edge_records.erase(e_tmp); 
            }
            h2 = h2->twin()->next();
            //std::cout << count_temp << std::endl; 
        } while(h2 != v2->halfedge());
        
        // std::cout << i << std::endl << std::endl; 
        vertex_quadrics.erase(v1); 
        vertex_quadrics.erase(v2); 

        // add back into the queue any edge touching the collapsed vertex 
        VertexRef v_new = collapse_edge(e_rm).value(); // have to use collapse_edge here instead of collapse_edge_erase() to avoid crash, because Dimitri wrote erase in collas_edge already? 
        HalfedgeRef h = v_new->halfedge();
        Mat4 Q_sum = Mat4::Zero;  
        
        do { // calculate the Q of new vertex 
            FaceRef f = h->face(); 
            Vec3 p0 = f->halfedge()->vertex()->pos; 
            Vec3 p1 = f->halfedge()->next()->vertex()->pos;
            Vec3 p2 = f->halfedge()->next()->next()->vertex()->pos;
            Vec3 normal = cross((p1.operator-(p0)), (p2.operator-(p0))); 
            float d = -dot(normal, p0); 

            Vec4 col1(normal.x*normal.x, normal.y*normal.x, normal.x*normal.z, normal.x*d);
            Vec4 col2(normal.x*normal.y, normal.y*normal.y, normal.y*normal.z, normal.y*d);
            Vec4 col3(normal.x*normal.z, normal.y*normal.z, normal.z*normal.z, normal.z*d);
            Vec4 col4(normal.x*d, normal.y*d, normal.z*d, d*d);
            Mat4 Q(col1, col2, col3, col4);  

            Q_sum = Q_sum.operator+=(Q);
            h = h->twin()->next();
            count_temp++;
        } while(h != v_new->halfedge());
        vertex_quadrics[v_new] = Q_sum;
        // std::cout << count_temp << std::endl; 

        h = v_new->halfedge();
        do { // loop through edge of new vertex and add to queue 
            Edge_Record edge_r(vertex_quadrics, h->edge()); 
            edge_records[h->edge()] = edge_r;
            edge_queue.insert(edge_r); 
            h = h->twin()->next();
        } while(h != v_new->halfedge());

        edge_queue.remove(e_rm_record); // remove collapsed edge 
    }

    // Note: if you erase elements in a local operation, they will not be actually deleted
    // until do_erase or validate are called. This is to facilitate checking
    // for dangling references to elements that will be erased.
    // The rest of the codebase will automatically call validate() after each op,
    // but here simply calling collapse_edge() will not erase the elements.
    // You should use collapse_edge_erase() instead for the desired behavior.
    
    return true;
}
