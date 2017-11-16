#ifndef MESH_HE_H
#define MESH_HE_H

#include <glm/glm.hpp>

#include <vector>
#include <memory>

class Mesh;
class HalfEdge;


/**
 * @brief The Vertex class.
 * Represents a vertex for the MeshHE class.
 * Vertex attributes: position and normal.
 */
class Vertex
{
public:

    // Constructors
    Vertex(glm::vec3* position, glm::vec3* normal) :
        m_id(s_id++), m_position(position), m_normal(normal), m_half_edge(NULL) {}


    // I/O
    void display() const;            /// Displays some information about the vertex in the console
    glm::uint m_id;                  /// Id of the vertex (used for display)
    static glm::uint s_id;           /// Static instance counter (for id generation)


public:

    // Attributes
    HalfEdge* m_half_edge;      /// Pointer to one of the half edges originating from this vertex (from which the others can be retrieved)

    glm::vec3* m_position;       /// Pointer to the position of the vertex
    glm::vec3* m_normal;         /// Pointer to the normal of the vertex (should be normalized)
};


/**
 * @brief The Face class.
 * Represents a face of for the MeshHE class.
 * Face attribute: none.
 */
class Face
{
public:

    // Constructors
    Face() : m_id(s_id++), m_half_edge(NULL) {}

    void display() const;            /// displays some information about the face in the console
    glm::uint m_id;                  /// id of the face (used for display)
    static glm::uint s_id;           /// static instance counter (for id generation)


public:

    // Attributes
    HalfEdge* m_half_edge;           /// Pointer to one of the half edges contouring this face (from which the others can be retrieved)
};


/**
 * @brief The HalfEdge class.
 * Represents a half edge for the MeshHE class.
 * Half edge attribute: none.
 */
class HalfEdge
{
public:

    // Constructors & utils  glm::vec3* m_position;
    HalfEdge(): m_id(s_id++), m_next(NULL), m_twin(NULL) {}

    bool EstEgal(const HalfEdge& he1,const HalfEdge& he2 );         /// Assignement operator performing deep copy


    // I/O
    void display() const;            /// Displays some information about the half edge in the console
    glm::uint m_id;                  /// Id of the face (used for display)
    static glm::uint s_id;           /// Static instance counter (for id generation)


public:

    // Attributes
    Vertex* m_vertex;           /// Pointer to the vertex at the origin of the half edge
    Face*   m_face;             /// Pointer to the face which contour is described by this half edge

    HalfEdge* m_twin;           /// Pointer to the other half of the edge (NULL if this is a border edge)
    HalfEdge* m_next;           /// Pointer to the next half edge in m_face
};


/**
 * @brief The MeshHE class.
 * Implements the half edge data structure for triangular meshes.
 */
class MeshHE
{
 public:

    // Constructors / Destructor & copy utils
    MeshHE(){}                                  /// Standard constructor
    MeshHE(const MeshHE &m);                    /// Copy constructor
    MeshHE(const Mesh& m);                      /// Constructor from Mesh (usefull for OFF loading)
    ~MeshHE();                                  /// Simple ressources de-allocation

    MeshHE& operator=(const MeshHE& m);         /// Assignement operator performing deep copy
    void ClearRessources();                     /// Simple ressources de-allocation


    // Smoothing [TODO]
    std::vector<Vertex*> GetVertexNeighbors(const Vertex* v) const;                                      /// Computes the 1-ring of vertex v
    glm::vec3 Laplacian(const Vertex *v) const;                                                          /// Computes the laplacian of vertex v of this mesh
    void LaplacianSmooth(const float lambda = 1.0, const glm::uint nb_iter = 1);                         /// Performs nb_iter steps of laplacian smoothing with factor lambda
    void TaubinSmooth(const float lambda = 0.330, const float mu = -0.331, const glm::uint nb_iter = 1); /// Performs nb_iter steps of taubin smoothing with factors lambda and mu


    // Border detection [TODO]
    bool IsAtBorder(const Vertex* v) const;     /// Tells wether vertex v is at border or not
    bool IsAtBorder(const HalfEdge* he) const;  /// Tells wether half edge he is at border or not
    bool IsAtBorder(const Face* f) const;       /// Tells wether face f is at border or not


    // I/O
    void display() const;                       /// Displays some info about this mesh in the console
    void write_obj(const char* filename) const; /// Exports this mesh in an OBJ file


    // Geometric utilities
    std::vector< glm::vec3 > computeBB() const ;/// Computes the bounding box of this mesh (usefull for normalization)
    void Normalize();                           /// Normalises and centers this mesh
    void ComputeNormals();                      /// Computes new normals for each vertex of this mesh


    // OpenGL utilities
    const std::vector<glm::vec3> &gen_positions_array() const;      /// Generates a contiguous representation for vertices positions
    const std::vector<glm::vec3> &gen_normals_array() const;        /// Generates a contiguous representation for vertices normals
    std::vector<glm::uint> gen_faces_array() const;                 /// Generates a contiguous representation for faces indices



public:

    // Attributes
    std::vector<HalfEdge*> m_half_edges;            /// Container for the half edges of the mesh
    std::vector<Face*> m_faces;                     /// Container for the faces of the mesh
    std::vector<Vertex*> m_vertices;                /// Container for the vertices of the mesh

    std::vector<glm::vec3> m_positions;             /// Container for the vertices positions
    std::vector<glm::vec3> m_normals;               /// Container for the vertices normals

private:

    std::vector<Vertex*> GetVertexNeighborsNotBorder(const Vertex* v) const;                                      /// Computes the 1-ring of vertex v
    std::vector<Vertex*> GetVertexNeighborsAtBorder(const Vertex* v) const;                                      /// Computes the 1-ring of vertex v

};

#endif // MESH_HE_H
