#ifndef SNAKE_H
#define SNAKE_H

#include <glm\glm.hpp>
#include <ctime>

class Snake {
public:
    Snake();
    void draw();
    void move();
    glm::vec3 getPosition();
    glm::vec3 getUp();
    glm::vec3 getDirection();
    glm::mat4 getTransformMatrix(); 
    void setBegin(clock_t newBegin) { begin = newBegin; } ;
private:
    glm::vec4 lastPosition;
    glm::vec4 direction;
    glm::vec4 up;
    glm::mat4 rotationMatrix; // The rotation part of TRS matrix
    clock_t begin; // Timestamp of step beginning
};

#endif