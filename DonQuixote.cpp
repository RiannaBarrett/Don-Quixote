 //Don Quixote



#include <stdio.h>
#include <vector>
#include "../common/vgl.h"
#include "../common/utils.h"
#include "../common/vmath.h"

using namespace vmath;
using namespace std;

#define NUM_SUN 361

// Vertex array and buffer names
enum VAO_IDs {Square, Triangle, Sun, NumVAOs};
enum Obj_Buffer_IDs {PosBuffer, NormBuffer, TexBuffer, NumObjBuffers};
enum Color_Buffer_IDs {SkyBlue, GrassGreen, HouseBrown, RoofRed, FanBlue, SunYellow, NumColorBuffers};

// Vertex array and buffer objects
GLuint VAOs[NumVAOs];
GLuint ObjBuffers[NumVAOs][NumObjBuffers];
GLuint ColorBuffers[NumColorBuffers];

// Number of vertices in each object
GLint numVertices[NumVAOs];

// Number of component coordinates
GLint posCoords = 2;
GLint colCoords = 4;

// Shader variables
// Shader program reference
GLuint trans_program;
// Shader component references
GLuint trans_vPos;
GLuint trans_vCol;
GLuint trans_model_mat_loc;
// Shader source files
const char *trans_vertex_shader = "../trans.vert";
const char *trans_frag_shader = "../trans.frag";

// Global state
mat4 model_matrix;
GLboolean anim = false;
GLfloat fan_angle = 0.0;
GLdouble elTime = 0.0;
GLdouble rpm = 10.0;
GLint dir = 1;

void display( );
void render_scene( );
void build_geometry( );
void build_square(GLuint obj);
void build_triangle(GLuint obj);
void build_sun(GLuint obj);
void draw_color_object(GLuint obj, GLuint color);
void draw_color_fan_object(GLuint obj, GLuint color);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, int button, int action, int mods);

int main(int argc, char**argv)
{
    // Create OpenGL window
    GLFWwindow* window = CreateWindow("Don Quixote 2022");
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    } else {
        printf("OpenGL window successfully created\n");
    }

    // TODO: Register callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    // Get initial time
    elTime = glfwGetTime();

    // Create geometry buffers
    build_geometry();

    // Load shaders and associate shader variables
    ShaderInfo trans_shaders[] = { {GL_VERTEX_SHADER, trans_vertex_shader},{GL_FRAGMENT_SHADER, trans_frag_shader},{GL_NONE, NULL} };
    trans_program = LoadShaders(trans_shaders);
    trans_vPos = glGetAttribLocation(trans_program, "vPosition");
    trans_vCol = glGetAttribLocation(trans_program, "vColor");
    trans_model_mat_loc = glGetUniformLocation(trans_program, "model_matrix");

    // Start loop
    while ( !glfwWindowShouldClose( window ) ) {
        // Draw graphics
        display();
        // Update other events like input handling
        glfwPollEvents();
        //  TODO: Update angle based on time for fixed rpm when animating
        GLdouble curTime = glfwGetTime();
        fan_angle += (curTime-elTime)*(rpm/60.0)*360.0;
        elTime = curTime;


        // Swap buffer onto screen
        glfwSwapBuffers( window );
    }

    // Close window
    glfwTerminate();
    return 0;
}

void display( )
{
    // Clear window
    glClear(GL_COLOR_BUFFER_BIT);

    // Render objects
    render_scene();

    // Flush pipeline
    glFlush();
}

void render_scene( ) {
    model_matrix = mat4().identity();

    // TODO: Declare transformation matrices

    mat4 scale_matrix = mat4().identity();
    mat4 rot_matrix = mat4().identity();
    mat4 trans_matrix = mat4().identity();

    // TODO: Draw sky
    trans_matrix = translate(vec3(0.0f, 0.25f, 0.0f));
    scale_matrix = scale(vec3(1.0f, 0.75f, 1.0f));
    model_matrix = trans_matrix*scale_matrix;
    draw_color_object(Square, SkyBlue);

    // TODO: Draw grass
    trans_matrix = translate(vec3(0.0f, -0.75f, 0.0f));
    scale_matrix = scale(vec3(3.5f, 0.40f, 1.6f));
    model_matrix = trans_matrix*scale_matrix;
    draw_color_object(Square, GrassGreen);

    // TODO: Draw house

    trans_matrix = translate(vec3(-0.02f, -0.20f, 0.10f));
    scale_matrix = scale(vec3(0.32f, -0.30f, -0.10f));
    model_matrix = trans_matrix*scale_matrix;
    draw_color_object(Square, HouseBrown);

    // TODO: Draw roof

    rot_matrix= rotate(-45.0f, 0.0f, 0.0f, 1.0f);
    scale_matrix = scale(vec3(0.31f, 0.31f, 0.07f));
    trans_matrix = translate(-0.0f, -0.09f, -0.0f);
    model_matrix = rot_matrix* scale_matrix* trans_matrix;
    draw_color_fan_object(Triangle, RoofRed);

    // TODO: Draw fan
    rot_matrix= rotate(3.0f, 0.0f, 10.0f, 1.0f);
    scale_matrix = scale(vec3(0.0f, 0.30f, -0.10f));
    trans_matrix = translate(0.10f, 0.10f, 0.10f);
    model_matrix = trans_matrix*rot_matrix*scale_matrix;
    draw_color_object(Triangle, FanBlue);

    // TODO: Draw sun using draw_color_fan_object
//    rot_matrix= rotate(-45.0f, 0.0f, 0.0f, 1.0f);
//    scale_matrix = scale(vec3(0.25f, 0.25f, 1.0f));
//    trans_matrix = translate(-0.26f, 0.26f, 1.0f);
//    model_matrix = trans_matrix*rot_matrix*scale_matrix;
//    draw_color_fan_object(Sun, SunYellow);
}

void build_geometry( )
{
    // Generate vertex arrays
    glGenVertexArrays(NumVAOs, VAOs);

    // Generate color buffers
    glGenBuffers(NumColorBuffers, ColorBuffers);

    // Build squares
    build_square(Square);

    // Build triangles
    build_triangle(Triangle);

    // Build sun
    build_sun(Sun);
}

void build_square(GLuint obj) {
    vector<vec2> vertices;
    vector<ivec3> indices;
    vector<vec4> blue_grad, green_grad, brown;

    // Bind square
    glBindVertexArray(VAOs[obj]);

    // Define square vertices
    vertices = {
            {1.0f,  1.0f},
            {-1.0f, 1.0f},
            {-1.0f, -1.0f},
            {1.0f,  -1.0f},
    };


    // TODO: Define square face indices (ensure proper orientation)
    indices = {
            {0, 1, 2},
            {2, 3, 0},
    };
    int numFaces = indices.size();

    // TODO: Define blue sky color
    blue_grad = {
            {0.0f, 0.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f, 1.0f}

    };

    // TODO: Define green grass color
    green_grad = {
            {0.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
            {0.0f,  1.0f, 0.0f, 1.0f},
            {0.0f,  1.0f, 0.0f,  1.0f}

    };

    // TODO: Define brown house color
    brown = {
            {0.5f,  0.35f, 0.0f, 1.0f},
            {0.5f,  0.35f, 0.0f,  1.0f},
            {0.5f, 0.35f, 1.0f, 1.0f},
            {0.5f, 0.35f, 0.0f, 1.0f}
    };

    // TODO: Create object vertices and colors from faces
    vector<vec2> obj_vertices;
    vector<vec4> obj_col_sky, obj_col_grass, obj_col_house;


    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 3; j++) {
            obj_vertices.push_back(vertices[indices[i][j]]);
            obj_col_sky.push_back(blue_grad[indices[i][j]]);


            obj_col_grass.push_back(green_grad[indices[i][j]]);


            obj_col_house.push_back(brown[indices[i][i]]);

        }
    }
    // Set numVertices as total number of INDICES
    numVertices[obj] = 3 * numFaces;

    // Generate object buffers for obj
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);

    // TODO: Bind and load position object buffer for obj
    // bind and load position for sky.. sky blue
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], obj_vertices.data(), GL_STATIC_DRAW);


    // TODO: Bind and load color buffers
    // bind and color sky
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[SkyBlue]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colCoords*NumColorBuffers, obj_col_sky.data(), GL_STATIC_DRAW);
    //bind and color grass
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[GrassGreen]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colCoords*NumColorBuffers, obj_col_grass.data(), GL_STATIC_DRAW);
    // bind and color house
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[HouseBrown]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colCoords*NumColorBuffers, obj_col_house.data(), GL_STATIC_DRAW);
}

void build_triangle(GLuint obj) {
    vector<vec2> vertices;
    vector<ivec3> indices;
    vector<vec4> red, blue_grad;

    // Bind vertex array for obj
    glBindVertexArray(VAOs[obj]);

    // Define triangle vertices
    vertices = {
            { 1.0f, 1.0f},
            {-1.0f, 1.0f},
            {-1.0f,-1.0f},
    };

    // TODO: Define triangle indices (ensure proper orientation)
    indices = {
            {0, 1, 2},
    };
    int numFaces = indices.size();

    // TODO: Define red roof color
    red = {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f, 0.0f},
            {1.0f,  0.0f, 0.0f, 0.0f}
    };

    // TODO: Define blue fan color
    blue_grad = {
            {0.0f, 0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 0.0f}
    };

    // TODO: Create object vertices and colors from faces

    vector<vec2> obj_vertices_1;
    vector<vec4> obj_col_roof, obj_col_fan;

    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 3; j++) {
            obj_vertices_1.push_back(vertices[indices[i][j]]);
            obj_col_roof.push_back(red[indices[i][j]]);
            obj_col_fan.push_back(blue_grad[indices[i][j]]);
        }
    }
//     Set numVertices as total number of INDICES (3*number of faces)
    numVertices[obj] = 3*numFaces;

    // Generate object buffers for obj
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);

    // TODO: Bind and load position object buffer for obj

    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*posCoords*numVertices[obj], obj_vertices_1.data(), GL_STATIC_DRAW);

    // TODO: Bind and load color buffer
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[RoofRed]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*colCoords*NumColorBuffers, obj_col_roof.data(), GL_STATIC_DRAW);
}

void build_sun(GLuint obj) {
    vector<vec2> vertices;
    vector<ivec3> indices;
    vector<vec4> yellow_grad;

    // Bind vertex array for obj
    glBindVertexArray(VAOs[obj]);

    // TODO: Define sun vertices and colors for triangle fan
//    vertices = {
//            {1.0f,  1.0f},
//            {-1.0f, 1.0f},
//            {-1.0f, -1.0f},
//            {1.0f,  -1.0f},
//    };
//
//    color= {
//            {0.0f, 0.0f, 0.0f, 1.0f},
//            {1.0f, 0.0f, 0.0f, 1.0f},
//            {1.0f, 0.0f, 1.0f, 1.0f},
//            {0.0f, 0.0f, 1.0f, 1.0f},
//            {0.0f, 1.0f, 0.0f, 1.0f},
//            {1.0f, 1.0f, 0.0f, 1.0f},
//            {1.0f, 1.0f, 1.0f, 1.0f},
//            {0.0f, 1.0f, 1.0f, 1.0f}
//    };
//    // TODO: Set numVertices for sun
////      vertices<vec2>

    // Generate object buffers for obj
    glGenBuffers(NumObjBuffers, ObjBuffers[obj]);

    // TODO: Bind and load position object buffer for obj

    // TODO: Bind and load color buffer

}

void draw_color_object(GLuint obj, GLuint color) {
    // Select shader program
    glUseProgram(trans_program);

    // Pass model matrix to shader
    glUniformMatrix4fv(trans_model_mat_loc, 1, GL_FALSE, model_matrix);

    // Bind vertex array
    glBindVertexArray(VAOs[obj]);

    // Bind position object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(trans_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(trans_vPos);

    // Bind color buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[color]);
    glVertexAttribPointer(trans_vCol, colCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(trans_vCol);

    // Draw geometry
    glDrawArrays(GL_TRIANGLES, 0, numVertices[obj]);
}

void draw_color_fan_object(GLuint obj, GLuint color) {
    // Select shader program
    glUseProgram(trans_program);

    // Pass model matrix to shader
    glUniformMatrix4fv(trans_model_mat_loc, 1, GL_FALSE, model_matrix);

    // Bind vertex array
    glBindVertexArray(VAOs[obj]);

    // Bind position object buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ObjBuffers[obj][PosBuffer]);
    glVertexAttribPointer(trans_vPos, posCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(trans_vPos);

    // Bind color buffer and set attributes
    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffers[color]);
    glVertexAttribPointer(trans_vCol, colCoords, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(trans_vCol);

    // Draw geometry
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices[obj]);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Esc closes window
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    // TODO: Start/Stop animation with spacebar
//    if (key == GLFW_KEY_SPACE) {
//        glfwTerminate;
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods){
    // TODO: Flip spin direction with mouse click
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        dir *= -1;
    }
}




