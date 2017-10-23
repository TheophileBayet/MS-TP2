#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp> //i32vec3
#include <vector>
#include <string>

class ImplicitFunction;

class Mesh
{
public:

    // Constructors
    Mesh(){}
    Mesh(const char* filename);
    ~Mesh();

    // Accessors
    glm::i32vec3 get_face(unsigned int i);
    glm::vec3 get_vertex(unsigned int i);
    glm::vec3 get_normal(unsigned int i);

    // utils
    void ComputeNormals();
    void RemoveDouble(float epsilon = 1e-5);
    std::vector< glm::vec3 > computeBB() const ;
    void normalize();

    // i/o
    void write_obj(const char* filename) const;

    // primitives
    static void CreateCube(Mesh& mesh);
    static void CreateCube2(Mesh& mesh);
    static void CreateSphere(Mesh& mesh, unsigned int Nu = 100, unsigned int Nv = 50);


public:

    // data
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< glm::uint > faces;
};

#endif // MESH_H
