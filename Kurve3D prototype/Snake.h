#ifndef SNAKE_H
#define SNAKE_H

#include <glm\glm.hpp>

typedef long clock_t;

class Snake {
public:
    Snake(int pl);
    void draw();
    void move();
    glm::vec3 getPosition();
    glm::vec3 getUp();
    glm::vec3 getDirection();
    glm::mat4 getTransformMatrix(); 
    void setBegin(clock_t);
private:
    glm::vec4 lastPosition;
    glm::vec4 direction;
    glm::vec4 up;
    glm::mat4 rotationMatrix; // The rotation part of TRS matrix
    clock_t begin; // Timestamp of step beginning
	int player; //player number
	int upButton;
	int downButton;
	int leftButton;
	int rightButton;
};

#endif