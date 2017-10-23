#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "MeshHE.h"

#define DISPLAY_DEBUG_INFO  true


class Object
{
public:
    Object();
    ~Object();

    void Draw(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const GLuint PmatrixID, const GLuint VmatrixID) const;

    void GenBuffers();
    void UpdateGeometryBuffers();
    void UpdateElementsBuffer();
    void UpdateBuffers();

    void SetMesh(MeshHE *mesh);
    void SetShader(const GLuint programID);
    void UpdateAttributeLocations();

public:

    MeshHE* m_mesh;
    glm::mat4 m_matrix;
    unsigned int m_id;

    GLuint m_programID;
    GLuint m_positionID;
    GLuint m_normalID;

    GLuint m_vertexBufferID;
    GLuint m_normalBufferID;
    GLuint m_elementBufferID;

    static unsigned int s_id;
};

#endif
