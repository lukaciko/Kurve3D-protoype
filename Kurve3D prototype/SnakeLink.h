#ifndef SNAKELINK_H
#define SNAKELINK_H

#include <glm\glm.hpp>

class SnakeLink {
public:
    SnakeLink(glm::vec3 pos);
    void draw();
    glm::vec3 getPosition();
private:
    glm::vec4 position;
    // SnakeLink prev
};

#endif