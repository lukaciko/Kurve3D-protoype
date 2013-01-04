// Shader World prototype.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Snake.h"
#include "SnakeLink.h"

#include <iostream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <ctime>

#define _USE_MATH_DEFINES // So we can use M_PI
#include <math.h>

// Shader sources
const char* vertexSource =
    "#version 150\n"
    "in vec2 position;"
    "in vec2 texCoord;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 proj;"
    "out vec2 TexCoord;"
    "void main() {"
    "   mat4 modelView = view * model;"
    "   modelView[0][0] = 1.0f;"
    "   modelView[0][1] = 0.0f;"
    "   modelView[0][2] = 0.0f;"
    "   modelView[1][0] = 0.0f;"
    "   modelView[1][1] = 1.0f;"
    "   modelView[1][2] = 0.0f;"
    "   modelView[2][0] = 0.0f;"
    "   modelView[2][1] = 0.0f;"
    "   modelView[2][2] = 1.0f;"
    "	gl_Position = proj * modelView * vec4( position, 0.0, 1.0 );"
    "   TexCoord = texCoord;"
    "}";
const char* fragmentSource =
    "#version 150\n"
    "in vec2 TexCoord;"
    "uniform float color;"
    "out vec4 outColor;"
    "uniform sampler2D tex;"
    "void main() {"
    "   if (texture(tex, TexCoord).a < 0.1f) "
    "      discard;"    
    "   outColor = texture(tex, TexCoord) * vec4( 1.0, color, color, 1.0 );"
    "}";

// Shader sources for cylinders
const char* cylinderVertexSource =
    "#version 150\n"
    "in vec2 position;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 proj;"
    "void main() {"
    "	gl_Position = proj * view * model * vec4( position, 0.0, 1.0 );"
    "}";
const char* cylinderFragmentSource =
    "#version 150\n"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4( 1.0f, 0.0f, 1.0f, 1.0f);"
    "}";

GLuint vertexShader;
GLuint fragmentShader;
GLuint cylinderVertexShader;
GLuint cylinderFragmentShader;

bool gamePaused;
bool unpausedImpulse;

void GLFWCALL keyCallbackFunction(int key, int newState) {
    std::cout << key << " " << ( newState == GLFW_PRESS ? "pressed" : "released" ) << "\n";
    if ( key == GLFW_KEY_SPACE && newState == GLFW_PRESS ) { 
        gamePaused = !gamePaused; 
        if ( !gamePaused ) 
            unpausedImpulse = true;
    }
}

float vertices[] = {
    -0.1f, -0.1f, 0.0f, 0.0f, // Vertex 1 (X, Y)
    +0.1f, -0.1f, 1.0f, 0.0f, // Vertex 2 (X, Y)
    -0.1f, +0.1f, 0.0f, 1.0f, // Vertex 3 (X, Y)

    +0.1f, +0.1f, 1.0f, 1.0f, // Vertex 4 (X, Y)
    +0.1f, -0.1f, 1.0f, 0.0f, // Vertex 2 (X, Y)
    -0.1f, +0.1f, 0.0f, 1.0f // Vertex 3 (X, Y)
};

GLuint compileShaders(GLuint aVertexShader, GLuint aFragmentShader, const char * aVertexSource, const char * aFragmentSource){
    // Create and compile the vertex shader
    aVertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( aVertexShader, 1, &aVertexSource, NULL );
    glCompileShader( aVertexShader );

    // Create and compile the fragment shader
    aFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( aFragmentShader, 1, &aFragmentSource, NULL );
    glCompileShader( aFragmentShader );

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, aVertexShader );
    glAttachShader( shaderProgram, aFragmentShader );
    glBindFragDataLocation( shaderProgram, 0, "outColor" );
    glLinkProgram( shaderProgram );

    GLint abort = 0;
    GLint testVal;
    //catch any errors
    glGetShaderiv(aVertexShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
    {
        abort = 1;
        char infolog[1024];
        glGetShaderInfoLog(aVertexShader,1024,NULL,infolog);
        std::cerr << "The vertex shader failed to compile with the error:" << infolog;
    }

    glGetShaderiv(aFragmentShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE) {
        abort = 1;
        char infolog[1024];
        glGetShaderInfoLog(aFragmentShader,1024,NULL,infolog);
        std::cerr << "The fragment shader failed to compile with the error:" << infolog;
    }

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &testVal);
    if(testVal == GL_FALSE) {
        abort = 1;
        char infolog[1024];
        glGetProgramInfoLog(shaderProgram,1024,NULL,infolog);
        std::cerr << "The program failed to compile with the error:" << infolog;
    }
    if(abort) {
        std::cerr << "errors occured, cannot continue, aborting.";
    }

    return shaderProgram;
}

int _tmain(int argc, _TCHAR* argv[])
{
    GLuint width = 1200;
    GLuint height = 900;

    // Initialize GLFW
    if(glfwInit() == GL_TRUE) {
    }
    else {
        exit( EXIT_FAILURE );
    }

    glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );
    glfwOpenWindowHint( GLFW_OPENGL_PROFILE, 0 );

    if (glfwOpenWindow( width, height, 0, 0, 0, 0, 24, 8, GLFW_WINDOW ) != GL_TRUE) {
        glfwTerminate();
        exit( EXIT_FAILURE );
    }
    glfwSetWindowTitle( "Shader World prototype" );

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << "\n";
        exit( EXIT_FAILURE );
    }

    std::cout << "Running OpenGL version " << glGetString(GL_VERSION) << "\n";

    glfwSetKeyCallback( &keyCallbackFunction );

    GLuint VAOs [2];
    glGenVertexArrays( 2, VAOs );
    glBindVertexArray( VAOs[0] );

    GLuint circleTex;
    glGenTextures( 1, &circleTex );
    glBindTexture( GL_TEXTURE_2D, circleTex );

    int iWidth, iHeight;
    unsigned char* image;

    const char* path = "circle1.png";
    image = SOIL_load_image( path, &iWidth, &iHeight, 0, SOIL_LOAD_RGBA );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image );
    SOIL_free_image_data( image );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glGenerateMipmap( GL_TEXTURE_2D );

    GLuint vbo;
    glGenBuffers( 1, &vbo ); // Generate 1 buffer
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    GLuint shaderProgram = compileShaders(vertexShader, fragmentShader, vertexSource, fragmentSource);
    glUseProgram( shaderProgram );

    GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
    glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 );
    glEnableVertexAttribArray( posAttrib );

    GLint texAttrib = glGetAttribLocation( shaderProgram, "texCoord" );
    glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void * ) ( 2*sizeof(float) ) );
    glEnableVertexAttribArray( texAttrib );

    glEnable( GL_DEPTH_TEST );

    Snake* p_snake = new Snake;
    std::vector<SnakeLink *> snakeLinks;

    // Set up cylinder drawing
    glBindVertexArray( VAOs[1] );

    // Generate the circle vertices
    int k = 50;
    float r = 0.25;
    float* circleVertices =  new float[2*k];
    
    for(int i = 0; i != k; ++i) {
        float phi = 2 * M_PI * i / k;
        float x = sin(phi) * r;
        float y = cos(phi) * r;
        circleVertices[2*i] = x;
        circleVertices[2*i+1] = y;
    }

    GLuint circleVBO;
    glGenBuffers( 1, &circleVBO ); // Generate 1 buffer
    glBindBuffer( GL_ARRAY_BUFFER, circleVBO );
    glBufferData( GL_ARRAY_BUFFER, 2 * k * sizeof(float), circleVertices, GL_DYNAMIC_DRAW );

    GLuint cylinderShaderProgram = compileShaders(cylinderVertexShader, cylinderFragmentShader, cylinderVertexSource, cylinderFragmentSource);
    glUseProgram( cylinderShaderProgram );

    GLint cyposAttrib = glGetAttribLocation( cylinderShaderProgram, "position" );
    glVertexAttribPointer( cyposAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( cyposAttrib );

    // Main game loop
    while( glfwGetWindowParam( GLFW_OPENED ) ) {

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glBindVertexArray(VAOs[0]);
        glUseProgram( shaderProgram );

        bool crash=false;

        glm::mat4 view = glm::lookAt(
            p_snake->getPosition() - p_snake->getDirection() + p_snake->getUp(), // Position of the camera
            p_snake->getPosition(),                                              // Point on screen
            p_snake->getUp()                                                     // Up axis
            );

        GLint uniView = glGetUniformLocation( shaderProgram, "view" );
        glUniformMatrix4fv( uniView, 1, GL_FALSE, glm::value_ptr( view ) );

        glm::mat4 proj = glm::perspective( 45.0f, (float)width / (float)height, 1.0f, 10.0f );
        GLint uniProj = glGetUniformLocation( shaderProgram, "proj" );
        glUniformMatrix4fv( uniProj, 1, GL_FALSE, glm::value_ptr( proj ) );

        if (unpausedImpulse) {
            p_snake->setBegin(clock());
            unpausedImpulse = false;
        }
        if (!gamePaused) { 
            snakeLinks.push_back(new SnakeLink(p_snake->getPosition()));
            p_snake->move();
        }

        glm::vec3 sPos = p_snake->getPosition();
        double dist= 0;
        double increase=0.001;
        bool change = false;
        int i=0;
        bool collision = false;
        for(std::vector<SnakeLink *>::iterator it = snakeLinks.begin(); it != snakeLinks.end(); ++it) {
            i++;
            if (dist>0.899 & !change) {
                increase *= -1;
                change = true;
            }
            if (dist<0.05 & change) {
                increase *= -1;
                change = false;
            }
            dist=dist+increase;
            glm::vec3 iPos = (*it)->getPosition();
            if((pow(sPos.x-iPos.x,2) < 0.01) && (pow(sPos.y-iPos.y,2) < 0.01) && (pow(sPos.z-iPos.z,2) < 0.01) && (i<(int)snakeLinks.size()-150) && ((int)snakeLinks.size()>150) && !collision){
                std::cout<<"Collision!";
                collision=true;
            }
            GLint uniModel = glGetUniformLocation( shaderProgram, "model" );
            glUniformMatrix4fv( uniModel, 1, GL_FALSE, glm::value_ptr( glm::translate(glm::mat4(), iPos ) ) );

            glm::vec3 distV = sPos - iPos;

            GLint uniColor = glGetUniformLocation( shaderProgram, "color" );
            glUniform1f(uniColor, 0.0f+dist);

            glDrawArrays( GL_TRIANGLES, 0, 6 );
        }

        p_snake->draw();
        GLint uniModel = glGetUniformLocation( shaderProgram, "model" );
        glUniformMatrix4fv( uniModel, 1, GL_FALSE, glm::value_ptr( glm::translate(glm::mat4(), sPos ) ) );

        glDrawArrays( GL_TRIANGLES, 0, 6 );

        glBindVertexArray(VAOs[1]);
        glUseProgram( cylinderShaderProgram );

        glm::mat4 viewc = glm::lookAt(
            p_snake->getPosition() - p_snake->getDirection() + p_snake->getUp(), // Position of the camera
            p_snake->getPosition(),                                              // Point on screen
            p_snake->getUp()                                              // Up axis
            );

        GLint uniModel1 = glGetUniformLocation( cylinderShaderProgram, "model" );
        glUniformMatrix4fv( uniModel1, 1, GL_FALSE, glm::value_ptr( p_snake->getTransformMatrix() ));  
        GLint uniView1 = glGetUniformLocation( cylinderShaderProgram, "view" );
        glUniformMatrix4fv( uniView1, 1, GL_FALSE, glm::value_ptr( viewc ) );
        glm::mat4 proj1 = glm::perspective( 45.0f, (float)width / (float)height, 1.0f, 10.0f );
        GLint uniProj1 = glGetUniformLocation( cylinderShaderProgram, "proj" );
        glUniformMatrix4fv( uniProj1, 1, GL_FALSE, glm::value_ptr( proj1 ) );

        glBindBuffer( GL_ARRAY_BUFFER, circleVBO );
        glBufferData( GL_ARRAY_BUFFER, 2 * k * sizeof(float), circleVertices, GL_DYNAMIC_DRAW );

        glDrawArrays( GL_LINE_LOOP, 0, k );

		if(collision) {
				delete p_snake;
				p_snake = new Snake;
				snakeLinks.clear();
		}

        glfwSwapBuffers(); // Also calls glfwPoolEvents
    }

    // TODO delete snakelinks

    delete p_snake;
    p_snake = 0;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwTerminate();

    return 0;
}
