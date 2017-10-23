#include <cstdlib>
#include <iostream>
#include <math.h>
#include <omp.h>

#include <Mesh.h>

using namespace glm;
using namespace std;

i32vec3 Mesh::get_face(unsigned int i) {
	glm::i32vec3 face( faces[3*i], faces[3*i+1], faces[3*i+2] );
	return face;
}

vec3 Mesh::get_vertex(unsigned int i) {
	return vertices[i];
}

vec3 Mesh::get_normal(unsigned int i) {
	return normals[i];
}


Mesh::Mesh(const char* filename) 
{
	int j = 0;
    unsigned int tmp;
	float *n;
	FILE *file;
	int   error;
	float r;

	if((file=fopen(filename,"r"))==NULL) 
	{
		std::cout << "Unable to read : " << filename << std::endl;
	}

	// create mesh
	vertices = vector<vec3>();
    normals  = vector<vec3>();
	faces    = vector<unsigned int>();

    glm::uint nb_vertices, nb_faces;

	error = fscanf(file,"OFF\n%d %d %d\n",&(nb_vertices),&(nb_faces),&tmp);
	if(error==EOF) 
	{
		std::cout << "Unable to read : " << filename << std::endl;
	}

	vertices.resize(nb_vertices);
    normals.resize(nb_vertices);
	faces.resize(nb_faces*3);

	// reading vertices
	for(int i=0;i<nb_vertices;++i) 
	{
		error = fscanf(file,"%f %f %f\n",&(vertices[i][0]),&(vertices[i][1]),&(vertices[i][2]));
		if(error==EOF) 
		{
			std::cout << "Unable to read vertices of : " << filename << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	// reading faces
	j = 0;
	for(int i=0;i<nb_faces;++i) 
	{
        error = fscanf(file,"%d %d %d %d\n",&tmp,&(faces[j]),&(faces[j+1]),&(faces[j+2]));

		if(error==EOF) 
		{
			std::cout << "Unable to read faces of : " << filename << std::endl;
			exit(EXIT_FAILURE);
		}

		if(tmp!=3) 
		{
			printf("Error : face %d is not a triangle (%d polygonal face!)\n",i/3,tmp);
			exit(EXIT_FAILURE);
		}
		j += 3;
	}

    fclose(file);
}

Mesh::~Mesh() {}


vector< vec3 > Mesh::computeBB() const 
{
    vector< vec3 > output;
    
    output.push_back(vertices[0]);
    output.push_back(vertices[0]);
    
	for(int i=1; i<vertices.size(); ++i) 
	{
	    vec3 v = vertices[i];
	    
	    for(int j = 0; j < 3; j++)
	    {
	        output[0][j] = std::min(output[0][j], v[j]);
	        output[1][j] = std::max(output[1][j], v[j]);
	    }
	}
	
	return output;
}

void Mesh::normalize()
{
    vector<vec3> bb = Mesh::computeBB();

    vec3 centre = (bb[0] + bb[1])*0.5f;
    float radius = glm::max(glm::max(bb[1].x - bb[0].x, bb[1].y - bb[0].y), bb[1].z - bb[0].z);
    
	for(int i=0; i<vertices.size(); ++i) 
	{
	    for(int j = 0; j < 3; j++)
	    {
//            vertices[i][j] = (vertices[i][j] - bb[0][j]) / (bb[1][j] - bb[0][j]) - 0.5;
            vertices[i][j] = (vertices[i][j] - centre[j]) / radius;
	    }
	}
}



void Mesh::CreateCube(Mesh& mesh)
{
    mesh.vertices.push_back(vec3(-1, -1, -1));
    mesh.vertices.push_back(vec3( 1, -1, -1));
    mesh.vertices.push_back(vec3( 1,  1, -1));
    mesh.vertices.push_back(vec3(-1,  1, -1));

    mesh.vertices.push_back(vec3(-1, -1, -1));
    mesh.vertices.push_back(vec3(-1,  1, -1));
    mesh.vertices.push_back(vec3(-1,  1,  1));
    mesh.vertices.push_back(vec3(-1, -1,  1));

    mesh.vertices.push_back(vec3(-1, -1, -1));
    mesh.vertices.push_back(vec3(-1, -1,  1));
    mesh.vertices.push_back(vec3( 1, -1,  1));
    mesh.vertices.push_back(vec3( 1, -1, -1));

    mesh.vertices.push_back(vec3( 1,  1,  1));
    mesh.vertices.push_back(vec3(-1,  1,  1));
    mesh.vertices.push_back(vec3(-1, -1,  1));
    mesh.vertices.push_back(vec3( 1, -1,  1));

    mesh.vertices.push_back(vec3( 1,  1,  1));
    mesh.vertices.push_back(vec3( 1, -1,  1));
    mesh.vertices.push_back(vec3( 1, -1, -1));
    mesh.vertices.push_back(vec3( 1,  1, -1));

    mesh.vertices.push_back(vec3( 1,  1,  1));
    mesh.vertices.push_back(vec3( 1,  1, -1));
    mesh.vertices.push_back(vec3(-1,  1, -1));
    mesh.vertices.push_back(vec3(-1,  1,  1));

    mesh.normals.push_back(vec3(0, 0, -1));
    mesh.normals.push_back(vec3(0, 0, -1));
    mesh.normals.push_back(vec3(0, 0, -1));
    mesh.normals.push_back(vec3(0, 0, -1));

    mesh.normals.push_back(vec3(-1, 0, 0));
    mesh.normals.push_back(vec3(-1, 0, 0));
    mesh.normals.push_back(vec3(-1, 0, 0));
    mesh.normals.push_back(vec3(-1, 0, 0));

    mesh.normals.push_back(vec3(0, -1, 0));
    mesh.normals.push_back(vec3(0, -1, 0));
    mesh.normals.push_back(vec3(0, -1, 0));
    mesh.normals.push_back(vec3(0, -1, 0));

    mesh.normals.push_back(vec3(0, 0,  1));
    mesh.normals.push_back(vec3(0, 0,  1));
    mesh.normals.push_back(vec3(0, 0,  1));
    mesh.normals.push_back(vec3(0, 0,  1));

    mesh.normals.push_back(vec3( 1, 0, 0));
    mesh.normals.push_back(vec3( 1, 0, 0));
    mesh.normals.push_back(vec3( 1, 0, 0));
    mesh.normals.push_back(vec3( 1, 0, 0));

    mesh.normals.push_back(vec3(0,  1, 0));
    mesh.normals.push_back(vec3(0,  1, 0));
    mesh.normals.push_back(vec3(0,  1, 0));
    mesh.normals.push_back(vec3(0,  1, 0));


    mesh.faces.push_back(0);
    mesh.faces.push_back(2);
    mesh.faces.push_back(1);

    mesh.faces.push_back(0);
    mesh.faces.push_back(3);
    mesh.faces.push_back(2);


    mesh.faces.push_back(4);
    mesh.faces.push_back(6);
    mesh.faces.push_back(5);

    mesh.faces.push_back(4);
    mesh.faces.push_back(7);
    mesh.faces.push_back(6);


    mesh.faces.push_back(8);
    mesh.faces.push_back(10);
    mesh.faces.push_back(9);

    mesh.faces.push_back(8);
    mesh.faces.push_back(11);
    mesh.faces.push_back(10);


    mesh.faces.push_back(12);
    mesh.faces.push_back(13);
    mesh.faces.push_back(14);

    mesh.faces.push_back(12);
    mesh.faces.push_back(14);
    mesh.faces.push_back(15);


    mesh.faces.push_back(16);
    mesh.faces.push_back(17);
    mesh.faces.push_back(18);

    mesh.faces.push_back(16);
    mesh.faces.push_back(18);
    mesh.faces.push_back(19);


    mesh.faces.push_back(20);
    mesh.faces.push_back(21);
    mesh.faces.push_back(22);

    mesh.faces.push_back(20);
    mesh.faces.push_back(22);
    mesh.faces.push_back(23);
}

void Mesh::CreateCube2(Mesh& mesh)
{
    for(float x = -1.0; x < 3.0; x += 2.0)
        for(float y = -1.0; y < 3.0; y += 2.0)
            for(float z = -1.0; z < 3.0; z += 2.0)
            {
                mesh.vertices.push_back(vec3(x, y, z));
                mesh.normals.push_back(glm::normalize(vec3(x, y, z)));
            }

    mesh.faces.push_back(0);
    mesh.faces.push_back(1);
    mesh.faces.push_back(3);

    mesh.faces.push_back(0);
    mesh.faces.push_back(3);
    mesh.faces.push_back(2);


    mesh.faces.push_back(0);
    mesh.faces.push_back(2);
    mesh.faces.push_back(6);

    mesh.faces.push_back(0);
    mesh.faces.push_back(6);
    mesh.faces.push_back(4);


    mesh.faces.push_back(0);
    mesh.faces.push_back(4);
    mesh.faces.push_back(5);

    mesh.faces.push_back(0);
    mesh.faces.push_back(5);
    mesh.faces.push_back(1);




    mesh.faces.push_back(1);
    mesh.faces.push_back(7);
    mesh.faces.push_back(3);

    mesh.faces.push_back(3);
    mesh.faces.push_back(7);
    mesh.faces.push_back(2);


    mesh.faces.push_back(2);
    mesh.faces.push_back(7);
    mesh.faces.push_back(6);

    mesh.faces.push_back(6);
    mesh.faces.push_back(7);
    mesh.faces.push_back(4);


    mesh.faces.push_back(4);
    mesh.faces.push_back(7);
    mesh.faces.push_back(5);

    mesh.faces.push_back(5);
    mesh.faces.push_back(7);
    mesh.faces.push_back(1);
}



void Mesh::CreateSphere(Mesh& mesh, unsigned int Nu, unsigned int Nv)
{

    for(int i = 0; i < Nu; i++)
    {
        float u = float(i) / (Nu-1);
        float phi = u * M_PI * 2;

        for(int j = 0; j < Nv; j++)
        {
            float v = float(j) / (Nv-1);
            float psi = v * M_PI;

            vec3 p(cos(phi)*sin(psi), sin(phi)*sin(psi), cos(psi));
            mesh.vertices.push_back(p);

            mesh.normals.push_back(p);

        }
    }

    for(int i = 0; i < Nu; i++)
    {
        for(int j = 0; j < Nv - 1; j++)
        {
            mesh.faces.push_back( i * Nv + j);
            mesh.faces.push_back((i+1)%Nu * Nv + j);
            mesh.faces.push_back( i * Nv + j+1);

            mesh.faces.push_back( i * Nv + j+1);
            mesh.faces.push_back((i+1)%Nu * Nv + j);
            mesh.faces.push_back((i+1)%Nu * Nv + j + 1);
        }
    }
}


void Mesh::write_obj(const char* filename) const
{
    FILE *file;

    if((file=fopen(filename,"w"))==NULL)
    {
        std::cout << "Unable to open : " << filename << std::endl;
    }

    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        vec3 p = vertices[i];
        fprintf(file,"v %f %f %f\n", p[0], p[1], p[2]);
    }

    for(unsigned int i = 0; i < faces.size(); i+=3)
    {
        fprintf(file,"f %i %i %i\n", faces[i]+1, faces[i+1]+1, faces[i+2]+1);
    }
}


void Mesh::ComputeNormals()
{
    normals.resize(vertices.size());

    for(unsigned int i=0; i<normals.size(); i++)
    {
        normals[i] = vec3(0.0);
    }

    for(unsigned int i=0; i<faces.size(); i+=3)
    {
        unsigned int i0 = faces[i+0];
        unsigned int i1 = faces[i+1];
        unsigned int i2 = faces[i+2];

        vec3 p0 = vertices[i0];
        vec3 p1 = vertices[i1];
        vec3 p2 = vertices[i2];

        vec3 d01 = glm::normalize(p1 - p0);
        vec3 d12 = glm::normalize(p2 - p1);
        vec3 d20 = glm::normalize(p0 - p2);

        vec3 faceNormal = glm::normalize(glm::cross(d01, -d20));

        float alpha0 = asin(length(glm::cross(d01, -d20)));
        float alpha1 = asin(length(glm::cross(d12, -d01)));
        float alpha2 = asin(length(glm::cross(d20, -d12)));

        if(glm::isnan(alpha0))
            alpha0 = 1.0f;
        if(glm::isnan(alpha1))
            alpha1 = 1.0f;
        if(glm::isnan(alpha2))
            alpha2 = 1.0f;

        normals[i0] += faceNormal * alpha0;
        normals[i1] += faceNormal * alpha1;
        normals[i2] += faceNormal * alpha2;
    }

    for(unsigned int i=0; i<normals.size(); i++)
    {
        float l = length(normals[i]);

        if(l < 1e-5)
        {
            cerr << "Error : normal of length 0." << endl;
            continue;
        }

        normals[i] /= l;
    }
}

void Mesh::RemoveDouble(float epsilon)
{
    vector<unsigned int> dbl;
    for(unsigned int i=0; i<vertices.size(); i++)
        dbl.push_back(i);

    for(unsigned int i=0; i<vertices.size()-1; i++)
    {
        if(dbl[i] != i)
            continue;

        for(unsigned int j=i+1; j<vertices.size(); j++)
        {
            if(length(vertices[i] - vertices[j]) < epsilon)
            {
                dbl[j] = i;
            }
        }
    }


    for(unsigned int i=0; i<vertices.size(); i++)
    {
        while(dbl[dbl[i]] != dbl[i])
        {
            dbl[i] = dbl[dbl[i]];
        }
    }

    vector<vec3> new_vertices;
    vector<int> corresp;

    for(unsigned int i=0; i<vertices.size(); i++)
    {
        if(dbl[i] == i)
        {
            corresp.push_back(new_vertices.size());
            new_vertices.push_back(vertices[i]);
            continue;
        }

        corresp.push_back(-1);
    }


    for(unsigned int i=0; i<vertices.size(); i++)
    {
        while(corresp[i] == -1)
        {
            corresp[i] = corresp[dbl[i]];
        }
    }



    for(unsigned int i=0; i<faces.size(); i++)
    {
        faces[i] = corresp[faces[i]];
    }

    vertices = new_vertices;

}


