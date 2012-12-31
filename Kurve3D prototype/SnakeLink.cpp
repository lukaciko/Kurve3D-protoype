#include "stdafx.h"

#include "SnakeLink.h"

SnakeLink::SnakeLink(glm::vec3 pos) {
    position = glm::vec4(pos.x, pos.y, pos.z, 1.0f);
}

glm::vec3 SnakeLink::getPosition() {
    return glm::vec3(position.x, position.y, position.z);
}

void SnakeLink::draw() {

}
