#ifndef SNAKE_H
#define SNAKE_H

#include <glm\glm.hpp>

class Snake {
public:
    Snake();
    void draw();
    void move();
    glm::vec3 getPosition();
    glm::vec3 getUp();
    glm::vec3 getDirection();
private:
    glm::vec4 lastPosition;
    glm::vec4 direction;
    glm::vec4 up;
};

#endif