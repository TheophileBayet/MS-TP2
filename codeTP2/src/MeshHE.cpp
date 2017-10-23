#include <MeshHE.h>
#include <Mesh.h>

#include <iostream>
#include <map>
//#include <algorithm>

using namespace glm;
using namespace std;


//---------------------------------------------------------
// MeshHE section
//---------------------------------------------------------


//***************
// Constructors

/**
 * @brief MeshHE::MeshHE
 * Creates all the structure of an half edge mesh.
 * WARNING : the input mesh m should be manifold, ie.:
 *  - each edge should be incident to one or two faces
 *  - the one ring of each vertex should be in one connected component
 * @param m
 */
MeshHE::MeshHE(const Mesh &m)
{
    m_vertices.reserve(m.vertices.size());
    m_faces.reserve(m.faces.size());
    m_half_edges.reserve(m.faces.size()*3);

    m_positions.reserve(m.vertices.size());
    m_normals.reserve(m.vertices.size());

    for(glm::uint i=0; i<m.vertices.size(); i++)
    {
        m_positions.push_back(m.vertices[i]);
        m_normals.push_back(m.normals[i]);
        Vertex* v = new Vertex (&m_positions[i], &m_normals[i]);
        m_vertices.push_back(v);
    }

    for(glm::uint i=0; i<m.faces.size() / 3; i++)
    {
        Face* f = new Face ();
        m_faces.push_back(f);

        std::vector<HalfEdge*> temp;
        for(glm::uint j=0; j<3; j++)
        {
            HalfEdge* he = new HalfEdge();
            m_vertices[m.faces[3*i+j]]->m_half_edge = he;
            he->m_vertex = m_vertices[m.faces[3*i+j]];
            he->m_face = f;
            temp.push_back(he);
        }

        f->m_half_edge = temp.front();

        for(glm::uint j=0; j<3; j++)
        {
            temp[j]->m_next = temp[(j+1)%3];
        }

        m_half_edges.insert(m_half_edges.end(), temp.begin(), temp.end());
    }

    map<Vertex*, vector<HalfEdge*> > mapping;

    for(glm::uint i=0; i<m_vertices.size(); i++)
    {
        mapping[m_vertices[i]] = vector<HalfEdge*>();
    }

    for(glm::uint i=0; i<m_half_edges.size(); i++)
    {
        mapping[m_half_edges[i]->m_vertex].push_back(m_half_edges[i]);
    }

    for(glm::uint i0 = 0; i0 < m_half_edges.size(); i0++)
    {
        HalfEdge* he = m_half_edges[i0];
        if(he->m_twin != NULL)
            continue;

        vector<HalfEdge*> potential_twin = mapping.at(he->m_next->m_vertex);

        for(glm::uint i1 = 0; i1 < potential_twin.size(); i1++)
        {
            HalfEdge* he2 = potential_twin[i1];

            if(he2->m_twin != NULL)
                continue;

            if(he->m_vertex == he2->m_next->m_vertex
            && he->m_next->m_vertex == he2->m_vertex)
            {
                he->m_twin = he2;
                he2->m_twin = he;
                break;
            }
        }
    }
}


MeshHE::~MeshHE()
{
    ClearRessources();
}


void MeshHE::ClearRessources()
{
    for(glm::uint i_he = 0; i_he < m_half_edges.size(); i_he++)
    {
        delete m_half_edges[i_he];
    }

    for(glm::uint i_f = 0; i_f < m_faces.size(); i_f++)
    {
        delete m_faces[i_f];
    }

    for(glm::uint i_v = 0; i_v < m_vertices.size(); i_v++)
    {
        delete m_vertices[i_v];
    }

    m_positions.clear();
    m_normals.clear();
}

MeshHE& MeshHE::operator=(const MeshHE& m)
{
    ClearRessources();

    m_vertices.reserve(m.m_vertices.size());
    m_faces.reserve(m.m_faces.size());
    m_half_edges.reserve(m.m_half_edges.size());

    m_positions.reserve(m.m_vertices.size());
    m_normals.reserve(m.m_vertices.size());

    for(glm::uint i=0; i<m.m_vertices.size(); i++)
    {
        m_positions.push_back(*m.m_vertices[i]->m_position);
        m_normals.push_back(*m.m_vertices[i]->m_normal);
        Vertex* v = new Vertex (&m_positions[i], &m_normals[i]);
        m_vertices.push_back(v);
    }

    for(glm::uint i=0; i<m.m_faces.size(); i++)
    {
        Face* f = new Face();
        m_faces.push_back(f);

        std::vector<HalfEdge*> temp;

        HalfEdge* he0 = new HalfEdge();
        m_vertices[m.m_faces[i]->m_half_edge->m_vertex->m_id]->m_half_edge = he0;
        he0->m_vertex = m_vertices[m.m_faces[i]->m_half_edge->m_vertex->m_id];
        he0->m_face = f;
        temp.push_back(he0);

        HalfEdge* he1 = new HalfEdge();
        m_vertices[m.m_faces[i]->m_half_edge->m_next->m_vertex->m_id]->m_half_edge = he1;
        he1->m_vertex = m_vertices[m.m_faces[i]->m_half_edge->m_next->m_vertex->m_id];
        he1->m_face = f;
        temp.push_back(he1);

        HalfEdge* he2 = new HalfEdge();
        m_vertices[m.m_faces[i]->m_half_edge->m_next->m_next->m_vertex->m_id]->m_half_edge = he2;
        he2->m_vertex = m_vertices[m.m_faces[i]->m_half_edge->m_next->m_next->m_vertex->m_id];
        he2->m_face = f;
        temp.push_back(he2);

        f->m_half_edge = temp.front();

        for(glm::uint j=0; j<3; j++)
        {
            temp[j]->m_next = temp[(j+1)%3];
        }

        m_half_edges.insert(m_half_edges.end(), temp.begin(), temp.end());
    }

    for(glm::uint i0 = 0; i0 < m_half_edges.size(); i0++)
    {
        HalfEdge* he = m_half_edges[i0];
        if(he->m_twin != NULL)
            continue;

        for(glm::uint i1 = 0; i1 < m_half_edges.size(); i1++)
        {
            HalfEdge* he2 = m_half_edges[i1];

            if(he2->m_twin != NULL)
                continue;

            if(he->m_vertex == he2->m_next->m_vertex
            && he->m_next->m_vertex == he2->m_vertex)
            {
                he->m_twin = he2;
                he2->m_twin = he;
                break;
            }
        }
    }
}

/**
 * @brief MeshHE::MeshHE
 * Deep copy is performed.
 * @param m
 */
MeshHE::MeshHE(const MeshHE& m)
{
    m_vertices.reserve(m.m_vertices.size());
    m_faces.reserve(m.m_faces.size());
    m_half_edges.reserve(m.m_half_edges.size());

    m_positions.reserve(m.m_vertices.size());
    m_normals.reserve(m.m_vertices.size());

    for(glm::uint i=0; i<m.m_vertices.size(); i++)
    {
        m_positions.push_back(*m.m_vertices[i]->m_position);
        m_normals.push_back(*m.m_vertices[i]->m_normal);
        Vertex* v = new Vertex (&m_positions[i], &m_normals[i]);
        m_vertices.push_back(v);
    }

    for(glm::uint i=0; i<m.m_faces.size(); i++)
    {
        Face* f = new Face();
        m_faces.push_back(f);

        std::vector<HalfEdge*> temp;

        HalfEdge* he0 = new HalfEdge();
        m_vertices[m.m_faces[i]->m_half_edge->m_vertex->m_id]->m_half_edge = he0;
        he0->m_vertex = m_vertices[m.m_faces[i]->m_half_edge->m_vertex->m_id];
        he0->m_face = f;
        temp.push_back(he0);

        HalfEdge* he1 = new HalfEdge();
        m_vertices[m.m_faces[i]->m_half_edge->m_next->m_vertex->m_id]->m_half_edge = he1;
        he1->m_vertex = m_vertices[m.m_faces[i]->m_half_edge->m_next->m_vertex->m_id];
        he1->m_face = f;
        temp.push_back(he1);

        HalfEdge* he2 = new HalfEdge();
        m_vertices[m.m_faces[i]->m_half_edge->m_next->m_next->m_vertex->m_id]->m_half_edge = he2;
        he2->m_vertex = m_vertices[m.m_faces[i]->m_half_edge->m_next->m_next->m_vertex->m_id];
        he2->m_face = f;
        temp.push_back(he2);

        f->m_half_edge = temp.front();

        for(glm::uint j=0; j<3; j++)
        {
            temp[j]->m_next = temp[(j+1)%3];
        }

        m_half_edges.insert(m_half_edges.end(), temp.begin(), temp.end());
    }

    for(glm::uint i0 = 0; i0 < m_half_edges.size(); i0++)
    {
        HalfEdge* he = m_half_edges[i0];
        if(he->m_twin != NULL)
            continue;

        for(glm::uint i1 = 0; i1 < m_half_edges.size(); i1++)
        {
            HalfEdge* he2 = m_half_edges[i1];

            if(he2->m_twin != NULL)
                continue;

            if(he->m_vertex == he2->m_next->m_vertex
            && he->m_next->m_vertex == he2->m_vertex)
            {
                he->m_twin = he2;
                he2->m_twin = he;
                break;
            }
        }
    }
}

//***************
// Smoothing [TODO]


vector<Vertex*> MeshHE::GetVertexNeighbors(const Vertex* v) const
{
    cout << "MeshHE::GetVertexNeighbors(const Vertex* v) is not coded yet!" << endl;

    return vector<Vertex*>();
}


glm::vec3 MeshHE::Laplacian(const Vertex* v) const
{
    cout << "MeshHE::Laplacian(const Vertex* v) is not coded yet!" << endl;

    return vec3(0.0);
}

void MeshHE::LaplacianSmooth(const float lambda, const glm::uint nb_iter)
{
    cout << "MeshHE::LaplacianSmooth(const float lambda, const glm::uint nb_iter) is not coded yet!" << endl;
}

void MeshHE::TaubinSmooth(const float lambda, const float mu, const glm::uint nb_iter)
{
    cout << "MeshHE::TaubinSmooth(const float lambda, const float mu, const glm::uint nb_iter) is not coded yet!" << endl;
}


//***************
// Border detection [TODO]

bool MeshHE::IsAtBorder(const Vertex* v) const
{
    cout << "MeshHE::IsAtBorder(const Vertex* v) is not coded yet!" << endl;
    return false;
}


bool MeshHE::IsAtBorder(const HalfEdge* he) const
{
    cout << "MeshHE::IsAtBorder(const HalfEdge* he) is not coded yet!" << endl;
    return false;
}


bool MeshHE::IsAtBorder(const Face* f) const
{
    cout << "MeshHE::IsAtBorder(const Face* f) is not coded yet!" << endl;
    return false;
}



//***************
// I/O

void MeshHE::display() const
{
    for(glm::uint i=0; i<m_vertices.size(); i++)
        m_vertices[i]->display();

    for(glm::uint i=0; i<m_faces.size(); i++)
        m_faces[i]->display();

    for(glm::uint i=0; i<m_half_edges.size(); i++)
        m_half_edges[i]->display();
}


void MeshHE::write_obj(const char* filename) const
{
    FILE *file;

    if((file=fopen(filename,"w"))==NULL)
    {
        std::cout << "Unable to open : " << filename << std::endl;
    }

    for(glm::uint i = 0; i < m_vertices.size(); i++)
    {
        vec3 p = *m_vertices[i]->m_position;
        fprintf(file,"v %f %f %f\n", p.x, p.y, p.z);
    }

    for(glm::uint i = 0; i < m_faces.size(); i++)
    {
        fprintf(file,"f %i %i %i\n", m_faces[i]->m_half_edge->m_vertex->m_id+1, m_faces[i]->m_half_edge->m_next->m_vertex->m_id+1, m_faces[i]->m_half_edge->m_next->m_next->m_vertex->m_id+1);
    }
}



//***************
// Geometric utilities


vector<vec3> MeshHE::computeBB() const
{
    vector<vec3> output;

    output.push_back(*m_vertices[0]->m_position);
    output.push_back(*m_vertices[0]->m_position);

    for(int i=1; i<m_vertices.size(); ++i)
    {
        vec3 v = *m_vertices[i]->m_position;

        output[0] = glm::min(output[0], v);
        output[1] = glm::max(output[1], v);

    }

    return output;
}


void MeshHE::Normalize()
{
    vector<vec3> bb = computeBB();

    vec3 centre = (bb[0] + bb[1])*0.5f;
    float radius = glm::max(glm::max(bb[1].x - bb[0].x, bb[1].y - bb[0].y), bb[1].z - bb[0].z);

    for(int i=0; i<m_vertices.size(); ++i)
    {
        *m_vertices[i]->m_position = (*m_vertices[i]->m_position - centre) / radius;
    }
}


void MeshHE::ComputeNormals()
{
    for(unsigned int i=0; i<m_vertices.size(); i++)
    {
        *m_vertices[i]->m_normal = vec3(0.0);

        vector<Vertex*> neib = GetVertexNeighbors(m_vertices[i]);

        glm::uint j_max = neib.size();
        if(IsAtBorder(m_vertices[i]))
            j_max --;

        for(glm::uint j = 0; j < j_max; j++)
        {
            vec3 d01 = glm::normalize(*neib[ j               ]->m_position - *m_vertices[i]->m_position);
            vec3 d02 = glm::normalize(*neib[(j+1)%neib.size()]->m_position - *m_vertices[i]->m_position);

            vec3 faceNormal = glm::normalize(glm::cross(d01, d02));

            float alpha = asin(length(glm::cross(d01, d02)));//1.0;//

            if(glm::isnan(alpha))
                alpha = 1.0f;

            *m_vertices[i]->m_normal += faceNormal * alpha;
        }

        *m_vertices[i]->m_normal = glm::normalize(*m_vertices[i]->m_normal);
    }
}


//***************
// OpenGL utilities

const vector<vec3>& MeshHE::gen_positions_array() const
{
    return m_positions;
}


const vector<vec3>& MeshHE::gen_normals_array() const
{
    return m_normals;
}


vector<glm::uint> MeshHE::gen_faces_array() const
{
    vector<glm::uint> output;

    for(glm::uint i=0; i<m_faces.size(); i++)
    {
        output.push_back(m_faces[i]->m_half_edge->m_vertex->m_position - &m_positions[0]);
        output.push_back(m_faces[i]->m_half_edge->m_next->m_vertex->m_position - &m_positions[0]);
        output.push_back(m_faces[i]->m_half_edge->m_next->m_next->m_vertex->m_position - &m_positions[0]);
    }

    return output;
}





//---------------------------------------------------------
// Vertex section
//---------------------------------------------------------

glm::uint Vertex::s_id = 0;

void Vertex::display() const
{
    cout << "v #" << m_id << endl;

    cout << "| position: " << m_position->x << " " << m_position->y << " " << m_position->z << endl;

    cout << "| normal: " << m_normal->x << " " << m_normal->y << " " << m_normal->z << endl;

    if(m_half_edge != NULL)
        cout << "| half edge: " << m_half_edge->m_id << endl;
    else
        cout << "| half edge: NULL" << endl;
}



//---------------------------------------------------------
// Face section
//---------------------------------------------------------

glm::uint Face::s_id = 0;

void Face::display() const
{
    cout << "f #" << m_id << endl;

    if(m_half_edge != NULL)
        cout << "| half edge: " << m_half_edge->m_id << endl;
    else
        cout << "| half edge: NULL" << endl;
}



//---------------------------------------------------------
// HalfEdge section
//---------------------------------------------------------

glm::uint HalfEdge::s_id = 0;

void HalfEdge::display() const
{
    cout << "he #" << m_id << endl;

    if(m_vertex != NULL)
        cout << "| vertex: " << m_vertex->m_id << endl;
    else
        cout << "| vertex: NULL" << endl;

    if(m_face != NULL)
        cout << "| face: " << m_face->m_id << endl;
    else
        cout << "| face: NULL" << endl;

    if(m_next != NULL)
        cout << "| next: " << m_next->m_id << endl;
    else
        cout << "| next: NULL" << endl;

    if(m_twin != NULL)
        cout << "| twin: " << m_twin->m_id << endl;
    else
        cout << "| twin: NULL" << endl;
}

