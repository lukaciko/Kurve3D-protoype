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
    "   if (color < 0.5f)"
    "	   outColor = texture(tex, TexCoord) * vec4( 1.0, color, color, 1.0 );"
    "   else"
    "      outColor = texture(tex, TexCoord) * vec4( 1.0, color, color, 1.0 );"
    "}";

GLuint vertexShader;
GLuint fragmentShader;

void GLFWCALL keyCallbackFunction(int key, int newState) {
    std::cout << key << " " << ( newState == GLFW_PRESS ? "pressed" : "released" ) << "\n";
}

float vertices[] = {
    -0.1f, -0.1f, 0.0f, 0.0f, // Vertex 1 (X, Y)
    +0.1f, -0.1f, 1.0f, 0.0f, // Vertex 2 (X, Y)
    -0.1f, +0.1f, 0.0f, 1.0f, // Vertex 3 (X, Y)

    +0.1f, +0.1f, 1.0f, 1.0f, // Vertex 4 (X, Y)
    +0.1f, -0.1f, 1.0f, 0.0f, // Vertex 2 (X, Y)
    -0.1f, +0.1f, 0.0f, 1.0f // Vertex 3 (X, Y)
};

GLuint compileShaders(){
    // Create and compile the vertex shader
    vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShader, 1, &vertexSource, NULL );
    glCompileShader( vertexShader );

    // Create and compile the fragment shader
    fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragmentShader, 1, &fragmentSource, NULL );
    glCompileShader( fragmentShader );

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glBindFragDataLocation( shaderProgram, 0, "outColor" );
    glLinkProgram( shaderProgram );

    GLint abort = 0;
    GLint testVal;
    //catch any errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE)
    {
        abort = 1;
        char infolog[1024];
        glGetShaderInfoLog(vertexShader,1024,NULL,infolog);
        std::cerr << "The vertex shader failed to compile with the error:" << infolog;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &testVal);
    if(testVal == GL_FALSE) {
        abort = 1;
        char infolog[1024];
        glGetShaderInfoLog(fragmentShader,1024,NULL,infolog);
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

    GLuint circleTex;
    glGenTextures( 1, &circleTex );   
    glBindTexture( GL_TEXTURE_2D, circleTex );

    int iWidth, iHeight;
    unsigned char* image;
    
    const char* path = "circle.png";
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

    GLuint shaderProgram = compileShaders();
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

    // Main game loop
    while( glfwGetWindowParam( GLFW_OPENED ) ) {

        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        //if (glfwGetKey( 'a' ) == GLFW_PRESS || glfwGetKey( 'A' ) == GLFW_PRESS)
        //    view = glm::rotate( view, 0.3f, glm::vec3( 1.0f, 0.0f, 0.0f) );
        //if (glfwGetKey( 'b' ) == GLFW_PRESS || glfwGetKey( 'B' ) == GLFW_PRESS)
        //    view = glm::rotate( view, 0.3f, glm::vec3( 0.0f, 1.0f, 0.0f) );
        //if (glfwGetKey( 'c' ) == GLFW_PRESS || glfwGetKey( 'C' ) == GLFW_PRESS)
        //    view = glm::rotate( view, 0.3f, glm::vec3( 0.0f, 0.0f, 1.0f) );

        glm::mat4 view = glm::lookAt(
            p_snake->getPosition() - p_snake->getDirection() + p_snake->getUp(),          // Postion of the camera
            p_snake->getPosition(),         // Point on screen
            p_snake->getUp()          // Up axis
            );

        GLint uniView = glGetUniformLocation( shaderProgram, "view" );
        glUniformMatrix4fv( uniView, 1, GL_FALSE, glm::value_ptr( view ) );

        glm::mat4 proj = glm::perspective( 45.0f, (float)width / (float)height, 1.0f, 10.0f );
        GLint uniProj = glGetUniformLocation( shaderProgram, "proj" );
        glUniformMatrix4fv( uniProj, 1, GL_FALSE, glm::value_ptr( proj ) );

        snakeLinks.push_back(new SnakeLink(p_snake->getPosition()));
        p_snake->move();

        glm::vec3 sPos = p_snake->getPosition();
        double dist= 0;
		double increase=0.001;
		bool change = false;
        for(std::vector<SnakeLink *>::iterator it = snakeLinks.begin(); it != snakeLinks.end(); ++it) {
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
            GLint uniModel = glGetUniformLocation( shaderProgram, "model" );
            glUniformMatrix4fv( uniModel, 1, GL_FALSE, glm::value_ptr( glm::translate(glm::mat4(), iPos ) ) );

            glm::vec3 distV = sPos - iPos;

            GLint uniColor = glGetUniformLocation( shaderProgram, "color" );
            if (distV.x*distV.x + distV.y*distV.y + distV.z*distV.z > 0.01f) {                
                glUniform1f(uniColor, 0.0f+dist);                
            }
            else
                glUniform1f(uniColor, 0.0f+dist);

            glDrawArrays( GL_TRIANGLES, 0, 6 );
        }

        p_snake->draw();
        GLint uniModel = glGetUniformLocation( shaderProgram, "model" );
        glUniformMatrix4fv( uniModel, 1, GL_FALSE, glm::value_ptr( glm::translate(glm::mat4(), sPos ) ) );

        glDrawArrays( GL_TRIANGLES, 0, 6 );


        glfwSwapBuffers(); // Also calls glfwPoolEvents
    }

    //delete snakelinks

    delete p_snake;
    p_snake = 0;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glfwTerminate();

    return 0;
}
