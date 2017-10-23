#include "Object.h"

#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include "shader.h"

using namespace std;
using namespace glm;


unsigned int Object::s_id = 0;

Object::Object():
    m_id(s_id)
{
    s_id++;

    m_matrix = scale(vec3(1.0f) * 0.5f);
}

Object::~Object()
{
    glDeleteBuffers(1, &m_vertexBufferID);
    glDeleteBuffers(1, &m_normalBufferID);
    glDeleteBuffers(1, &m_elementBufferID);
}

void Object::SetMesh(MeshHE *mesh)
{
    m_mesh = mesh;
    UpdateBuffers();
}

void Object::UpdateGeometryBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_mesh->m_vertices.size(), m_mesh->gen_positions_array().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_mesh->m_vertices.size(), m_mesh->gen_normals_array().data(), GL_STATIC_DRAW);
}

void Object::UpdateElementsBuffer()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * 3 * m_mesh->m_faces.size(), m_mesh->gen_faces_array().data(), GL_STATIC_DRAW);
}

void Object::UpdateBuffers()
{
    UpdateGeometryBuffers();
    UpdateElementsBuffer();
}




void Object::GenBuffers()
{
    glGenBuffers(1, &m_vertexBufferID);
    if(DISPLAY_DEBUG_INFO)
        cout << "  vertexBufferID = " << m_vertexBufferID << endl;

    glGenBuffers(1, &m_normalBufferID);
    if(DISPLAY_DEBUG_INFO)
        cout << "  normalBufferID = " << m_normalBufferID << endl;

    glGenBuffers(1, &m_elementBufferID);
    if(DISPLAY_DEBUG_INFO)
        cout << "  elementBufferID = " << m_elementBufferID << endl;
}

void Object::SetShader(const GLuint programID)
{
    m_programID = programID;
    if(DISPLAY_DEBUG_INFO)
        cout << "  programID = " << m_programID << endl;

    UpdateAttributeLocations();
}

void Object::UpdateAttributeLocations()
{
    m_positionID = glGetAttribLocation(m_programID, "in_position");
    if(DISPLAY_DEBUG_INFO)
        cout << "positionID = " << m_positionID << endl;

    m_normalID = glGetAttribLocation(m_programID, "in_normal");
    if(DISPLAY_DEBUG_INFO)
        cout << "normalID = " << m_normalID << endl;
}


void Object::Draw(const mat4& projection_matrix, const mat4& view_matrix, const GLuint PmatrixID, const GLuint VmatrixID) const
{
    // Shader program setting
    glUseProgram(m_programID);

    // Matrix transmission
    glUniformMatrix4fv(PmatrixID, 1, GL_FALSE, value_ptr(projection_matrix));
    glUniformMatrix4fv(VmatrixID, 1, GL_FALSE, value_ptr(view_matrix));


    // Pointer settings
    glEnableVertexAttribArray(m_positionID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glVertexAttribPointer(
                m_positionID,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void*)0
                );


    glEnableVertexAttribArray(m_normalID);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferID);
    glVertexAttribPointer(
                m_normalID,
                3,
                GL_FLOAT,
                GL_TRUE,
                0,
                (void*)0
                );


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferID);


    // Draw triangles
    glDrawElements(
                GL_TRIANGLES,               // mode
                m_mesh->m_faces.size()*3,    // count
                GL_UNSIGNED_INT,            // type
                (void*)0                    // offset
                );

    glDisableVertexAttribArray(m_positionID);
    glDisableVertexAttribArray(m_normalID);
}
