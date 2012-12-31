#include "stdafx.h"

#include "Snake.h"
#include <GL\glfw.h>
#include <GL\glfw.h>
#include <iostream>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float diff = 0.01f; // Rate of shange TODO : time relative

glm::vec3 v4tov3(glm::vec4 v1);

Snake::Snake() {
    std::cout << "Constructing snake";
    lastPosition[0] = 0.0f;
    lastPosition[1] = 0.0f;
    lastPosition[2] = 0.0f;
    lastPosition[3] = 1.0f; // Point - w=1
    direction[0] = 1.0f; // Pointed in X direction
    direction[1] = 0.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f; // Vector - w=0
    up[0] = 0.0f; // Pointed in Z direction
    up[1] = 0.0f;
    up[2] = 1.0f;
    up[3] = 0.0f; // Vector - w=0
}


void Snake::move() {
    float diff_angle = 2.0f;

    // Check input
    if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS) {
        glm::mat4 rotateM = glm::rotate(glm::mat4(), diff_angle, glm::cross(v4tov3(up), v4tov3(direction))); 
        direction = direction * rotateM;
        up = up * rotateM;
    }
    if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS) {
        glm::mat4 rotateM = glm::rotate(glm::mat4(), -diff_angle, glm::cross(v4tov3(up), v4tov3(direction)));  
        direction = direction * rotateM;
        up = up * rotateM;
    }
    if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS) {
        glm::mat4 rotateM = glm::rotate(glm::mat4(), -diff_angle, v4tov3(up)); 
        direction = direction * rotateM;
        up = up * rotateM;
    }
    if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS) {
        glm::mat4 rotateM = glm::rotate(glm::mat4(), diff_angle, v4tov3(up)); 
        direction = direction * rotateM;
        up = up * rotateM;
    }

    // Change direction

    // Change position
    for (int i = 0; i!=4; ++i) {
        lastPosition[i] += diff * direction[i];
    }

}
void Snake::draw() {
    std::cout << "drawing the snake at position " << lastPosition[0] << " " << lastPosition[1] << " "
        << lastPosition[2]  << " " << lastPosition[3] << "\n";
    std::cout << "direction " << direction[0] << " " << direction[1] << " "
        << direction[2]  << " " << direction[3] << "\n";
}

glm::vec3 Snake::getPosition() {
    return v4tov3(lastPosition);
}

glm::vec3 Snake::getUp() {
    return v4tov3(up);
}

glm::vec3 Snake::getDirection() {
    return v4tov3(direction);
}

glm::vec3 v4tov3(glm::vec4 v1)  {
    return glm::vec3(v1.x, v1.y, v1.z);
}
