#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <shader.h> // Help to load shaders from files

// Include GLEW : Always include it before glfw.h et gl.h :)
#include <GL/glew.h>    // OpenGL Extension Wrangler Library : http://glew.sourceforge.net/ 
#include <GL/glfw.h>    // Window, keyboard, mouse : http://www.glfw.org/

#include <glm/glm.hpp>  // OpenGL Mathematics : http://glm.g-truc.net/0.9.5/index.html
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include "GLFW_define.h"
#include "Mesh.h"
#include "MeshHE.h"
#include "Object.h"


// Window size :
#define WIDTH 1000.0f
#define HEIGHT 800.0f

using namespace glm;
using namespace std;


void view_control(mat4& view_matrix, float dx);

int main()
{

    cout << "Starting program..." << endl;

    //==================================================
    //============= Creation de la fenetre =============
    //==================================================

    // GLFW initialization
    if( !glfwInit() )
    {
        cerr << "Failed to initialize GLFW!" << endl;
        exit(EXIT_FAILURE);
    }

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // Anti Aliasing
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // OpenGL 3.1
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

    // Window and OpenGL conetxt creation
    if( !glfwOpenWindow(WIDTH, HEIGHT, 0,0,0,0, 32,0, GLFW_WINDOW ) )
//    if( !glfwOpenWindow(WIDTH, HEIGHT, 0,0,0,0, 32,0, GLFW_FULLSCREEN ) )
    {
        cerr << "GLFW failed to open OpenGL window!" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // GLFW Settings
    glfwSetWindowTitle( "TP 3A Ensimag - MMMIS - Marching cube" );
    glfwEnable( GLFW_STICKY_KEYS );

    // GLEW Initialization
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to intialize GLEW:!" << endl;
        exit(EXIT_FAILURE);
    }



    // Soft- and Firm-ware checkings
    const GLubyte* renderer = glGetString (GL_RENDERER);
    cout << "GPU : " << renderer << endl;

    const GLubyte* version = glGetString (GL_VERSION);
    cout << "OpenGL Driver : " << version << endl;


    cout << endl;


    //==================================================
    //================= Initialization =================
    //==================================================

    cout << "Initializing..." << endl;



    //-------------------------------------------------
    // OpenGL Initialization

    glClearColor(0.1, 0.1, 0.1, 1.0);       /// Dark Back ground
//    glClearColor(1.0, 1.0, 1.0, 1.0);       /// Light Back ground
    glEnable(GL_DEPTH_TEST);



    //-------------------------------------------------
    // Shader program initialization

    GLuint programID = LoadShaders("../shader/vertex.glsl", "../shader/fragment.glsl");
    cout << "programID = " << programID << endl;


    //-------------------------------------------------
    // Data arrays Initialization

    // Mesh creation

//    Mesh m("../models/armadillo.off");
//    Mesh m("../models/buddha.off");
//    Mesh m("../models/bunny.off");
    Mesh m("../models/ceasar.off");
//    Mesh m("../models/cube_closed.off");
//    Mesh m("../models/cylindre.off");
//    Mesh m("../models/dragon.off");
//    Mesh m("../models/half_cylindre.off");
//    Mesh m("../models/max.off");
//    Mesh m("../models/pipes_round.off");
//    Mesh m("../models/pipes_squared.off");
//    Mesh m("../models/sphere.off");
//    Mesh m("../models/sphere_piece.off");
//    Mesh m("../models/test.off");
//    Mesh m("../models/tetrahedron.off");
//    Mesh m("../models/tetrahedron_2.off");
//    Mesh m("../models/thing_rounded.off");
//    Mesh m("../models/thing_squared.off");
//    Mesh m("../models/triceratops.off");

    m.normalize();
    m.ComputeNormals();

    cout << "half edge conversion... " << endl;

    // Half edge conversion
    MeshHE m_he(m);


    /// TODO : test your neighborhood and laplacian computation here.








    // Object Generation
    Object o;
    o.GenBuffers();
    o.SetMesh(&m_he);
    o.SetShader(programID);



    //-------------------------------------------------
    // MVP matrices initialization

    mat4 projection_matrix = perspective(45.0f, WIDTH / HEIGHT, 0.1f, 100.0f);
    mat4 view_matrix = lookAt(vec3(1.0, 0.5, 1.0), vec3(0.0), vec3(0.0, 1.0, 0.0));

    GLuint PmatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
    cout << "PmatrixID = " << PmatrixID << endl;

    GLuint VmatrixID = glGetUniformLocation(programID, "ViewMatrix");
    cout << "VmatrixID = " << VmatrixID << endl;
    


    cout << "Initializing done." << endl;
    cout << endl;
    


    //==================================================
    //==================== Main Loop ===================
    //==================================================


    cout << "Starting main loop..." << endl;

    double init_time = glfwGetTime();
    double prec_time = init_time;
    double cur_time = init_time;
    double speed = 2.0;

    do{
        // Clearing Viewport
        glClear( GL_COLOR_BUFFER_BIT );
        glClear( GL_DEPTH_BUFFER_BIT );


        //==================================================
        //================== Computations ==================
        //==================================================

        prec_time = cur_time;
        cur_time = glfwGetTime() - init_time;
        float delta_time = cur_time - prec_time;

        view_control(view_matrix, speed * delta_time);

        // Smoothing control: press the space bar to see the effect of your smoothing in real time !
        if (glfwGetKey( GLFW_KEY_SPACE ) == GLFW_PRESS)
        {
            o.m_mesh->LaplacianSmooth(0.5, 1);
            o.UpdateGeometryBuffers();
        }


        //==================================================
        //===================== Drawing ====================
        //==================================================

        o.Draw(view_matrix, projection_matrix, VmatrixID, PmatrixID);

        glfwSwapBuffers();


        //==================================================
        //================== Stats Display =================
        //==================================================

        cout.precision(2);
        cout<< fixed  << "FPS: " << 1.0 / delta_time << "\t---\tElapsed time: " << setw(8) << cur_time << "\ts\r" << flush;

    }
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED )        );

    // Closing the window
    glfwTerminate();

    cout << endl << endl << "Main loop ended." << endl;


    cout << "Program ended." << endl;


    return EXIT_SUCCESS;
}




void view_control(mat4& view_matrix, float dx)
{
    if (glfwGetKey( GLFW_KEY_LSHIFT ) == GLFW_PRESS)
    {
        dx /= 10.0;
    }

    if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS)
    {
        vec4 axis = vec4(1.0, 0.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS)
    {
        vec4 axis = vec4(1.0, 0.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, -dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, -dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_PAGEUP ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, dx * 180.0f, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_PAGEDOWN ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0);
        axis = inverse(view_matrix) * axis;
        view_matrix = rotate(view_matrix, -dx * 180.0f, vec3(axis));
    }

    if (glfwGetKey( GLFW_KEY_Z ) == GLFW_PRESS)
    {
        vec3 pos = vec3(view_matrix * vec4(0,0,0,1));
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0) * dx * length(pos) * 0.5;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_S ) == GLFW_PRESS)
    {
        vec3 pos = vec3(view_matrix * vec4(0,0,0,1));
        vec4 axis = vec4(0.0, 0.0, 1.0, 0.0) * (-dx) * length(pos) * 0.5;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_Q) == GLFW_PRESS)
    {
        vec4 axis = vec4(-1.0, 0.0, 0.0, 0.0) * dx;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_D ) == GLFW_PRESS)
    {
        vec4 axis = vec4(-1.0, 0.0, 0.0, 0.0) * (-dx);
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_A ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0) * dx;
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
    if (glfwGetKey( GLFW_KEY_E ) == GLFW_PRESS)
    {
        vec4 axis = vec4(0.0, 1.0, 0.0, 0.0) * (-dx);
        axis = inverse(view_matrix) * axis;
        view_matrix = translate(view_matrix, vec3(axis));
    }
}


