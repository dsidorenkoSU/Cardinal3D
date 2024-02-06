
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

/*
    This method should collapse the given edge and return an iterator to
    the new vertex created by the collapse.
*/
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_edge(Halfedge_Mesh::EdgeRef e) {

    
    // // Phase1
    // // HALFEDGES
    // HalfedgeRef h0 = e->halfedge();
    // HalfedgeRef h1 = h0->next();
    // HalfedgeRef h2 = h1->next();
    // HalfedgeRef h3 = h0->twin();
    // HalfedgeRef h4 = h3->next();
    // HalfedgeRef h5 = h4->next();
    // HalfedgeRef h6 = h1->twin();
    // HalfedgeRef h7 = h2->twin();
    // HalfedgeRef h8 = h4->twin();
    // HalfedgeRef h9 = h5->twin();

    // // VERTICES
    // VertexRef v0 = h0->vertex();
    // VertexRef v1 = h3->vertex();
    // VertexRef v2 = h5->vertex();
    // VertexRef v3 = h2->vertex();

    // // EDGES
    // EdgeRef e0 = h0->edge();
    // EdgeRef e1 = h5->edge();
    // EdgeRef e2 = h4->edge();
    // EdgeRef e3 = h2->edge();
    // EdgeRef e4 = h1->edge();

    // // FACES
    // FaceRef f0 = h0->face();
    // FaceRef f1 = h3->face();

    // // Phase2
    // VertexRef v4 = new_vertex(); 
    
    (void)e;
    return std::nullopt;
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

    // HALFEDGES
    HalfedgeRef h0 = e->halfedge();
    HalfedgeRef h1 = h0->next();
    HalfedgeRef h2 = h1->next();
    HalfedgeRef h3 = h0->twin();
    HalfedgeRef h4 = h3->next();
    HalfedgeRef h5 = h4->next();
    HalfedgeRef h6 = h1->twin();
    HalfedgeRef h7 = h2->twin();
    HalfedgeRef h8 = h4->twin();
    HalfedgeRef h9 = h5->twin();

    // VERTICES
    VertexRef v0 = h0->vertex();
    VertexRef v1 = h3->vertex();
    VertexRef v2 = h5->vertex();
    VertexRef v3 = h2->vertex();

    // EDGES
    EdgeRef e0 = h0->edge();
    EdgeRef e1 = h9->edge();
    EdgeRef e2 = h8->edge();
    EdgeRef e3 = h7->edge();
    EdgeRef e4 = h6->edge();

    // FACES
    FaceRef f0 = h0->face();
    FaceRef f1 = h3->face();

    // HALFEDGES
    h0->next() = h1;
    h0->twin() = h3;
    h0->vertex() = v2;
    h0->edge() = e0;
    h0->face() = f0;

    h1->next() = h2;
    h1->twin() = h7;
    h1->vertex() = v3;
    h1->edge() = e3;
    h1->face() = f0;
    
    h2->next() = h0;
    h2->twin() = h8;
    h2->vertex() = v0;
    h2->edge() = e2;
    h2->face() = f0;

    h3->next() = h4;
    h3->twin() = h0;
    h3->vertex() = v3;
    h3->edge() = e0;
    h3->face() = f1;

    h4->next() = h5;
    h4->twin() = h9;
    h4->vertex() = v2;
    h4->edge() = e1;
    h4->face() = f1;

    h5->next() = h3;
    h5->twin() = h6;
    h5->vertex() = v1;
    h5->edge() = e4;
    h5->face() = f1;

    h6->next() = h6->next();
    h6->twin() = h5;
    h6->vertex() = v3;
    h6->edge() = e4;
    h6->face() = h6->face();

    h7->next() = h7->next();
    h7->twin() = h1;
    h7->vertex() = v0;
    h7->edge() = e3;
    h7->face() = h7->face();

    h8->next() = h8->next();
    h8->twin() = h2;
    h8->vertex() = v2;
    h8->edge() = e2;
    h8->face() = h8->face();

    h9->next() = h9->next(); // didn't change, but set it anyway!
    h9->twin() = h4;
    h9->vertex() = v1;
    h9->edge() = e1;
    h9->face() = h9->face(); // didn't change, but set it anyway!

    // VERTICES
    v0->halfedge() = h2;
    v1->halfedge() = h5;
    v2->halfedge() = h0;
    v3->halfedge() = h3;

    // EDGES
    e0->halfedge() = h0; 
    e1->halfedge() = h4; 
    e2->halfedge() = h2; 
    e3->halfedge() = h7; 
    e4->halfedge() = h6; 

    // FACES
    f0->halfedge() = h0; 
    f1->halfedge() = h3;

    (void)e;
    return std::optional(e0);
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
        std::cout << "not a traingle mesh" << std::endl; 
        return std::nullopt;
    }
    if (e->halfedge()->twin() != e->halfedge()->twin()->next()->next()->next()){
        std::cout << "not a traingle mesh" << std::endl; 
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
    return vertex_m;
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
    

    // 0. accumlate list of halfedges 
    std::vector<HalfedgeRef> new_halfedges;
    auto h = f->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != f->halfedge());

    // 1. loop around face to count and intilize and assign if possible 
    // int count = 0; 
    // HalfedgeRef half_origin = f->halfedge();
    std::cout << new_halfedges.size()-1 << std::endl; 

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

    return f;
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

    if(flip_orientation) normal_offset = -normal_offset;
    
    std::vector<HalfedgeRef> new_halfedges;

    // create a list of halfedges to loop 
    auto h = face->halfedge();
    do {
        new_halfedges.push_back(h);
        h = h->next();
    } while(h != face->halfedge());

    for (size_t i = 0; i < new_halfedges.size()-1; i++) {
        Vec3 pi = start_positions[i]; // get the original vertex pos 
        size_t temp = i+1; 
        if (temp == new_halfedges.size()-1) {
            temp = 0; 
        }
        //Vec3 pi_next = start_positions[temp]; // get the original next vertex pos 
        new_halfedges[i]->vertex()->pos = pi.operator+(normal_offset);

    }
    std::cout << normal_offset << std::endl;
    std::cout << tangent_offset << std::endl << std::endl;

}
/*
    Splits all non-triangular faces into triangles.
*/
void Halfedge_Mesh::triangulate() {

    // For each face...
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

    // For each edge, assign the midpoint of the two original
    // positions to Edge::new_pos.

    // For each face, assign the centroid (i.e., arithmetic mean)
    // of the original vertex positions to Face::new_pos. Note
    // that in general, NOT all faces will be triangles!
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

    // The implementation for this routine should be
    // a lot like Halfedge_Mesh:linear_subdivide_positions:(),
    // except that the calculation of the positions themsevles is
    // slightly more involved, using the Catmull-Clark subdivision
    // rules. (These rules are outlined in the Developer Manual.)

    // Faces

    // Edges

    // Vertices
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
        : edge(e) {

        // Compute the combined quadric from the edge endpoints.
        // -> Build the 3x3 linear system whose solution minimizes the quadric error
        //    associated with these two endpoints.
        // -> Use this system to solve for the optimal position, and store it in
        //    Edge_Record::optimal.
        // -> Also store the cost associated with collapsing this edge in
        //    Edge_Record::cost.
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
    // -> Compute an initial quadric for each vertex as the sum of the quadrics
    //    associated with the incident faces, storing it in vertex_quadrics
    // -> Build a priority queue of edges according to their quadric error cost,
    //    i.e., by building an Edge_Record for each edge and sticking it in the
    //    queue. You may want to use the above PQueue<Edge_Record> for this.
    // -> Until we reach the target edge budget, collapse the best edge. Remember
    //    to remove from the queue any edge that touches the collapsing edge
    //    BEFORE it gets collapsed, and add back into the queue any edge touching
    //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
    //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
    //    top of the queue.

    // Note: if you erase elements in a local operation, they will not be actually deleted
    // until do_erase or validate are called. This is to facilitate checking
    // for dangling references to elements that will be erased.
    // The rest of the codebase will automatically call validate() after each op,
    // but here simply calling collapse_edge() will not erase the elements.
    // You should use collapse_edge_erase() instead for the desired behavior.

    return false;
}
