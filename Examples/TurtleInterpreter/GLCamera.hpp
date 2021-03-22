#ifndef GLCAMERA_HPP
#define GLCAMERA_HPP

#include "Turtle3D.hpp"

struct CameraGL
{
public:
    CameraGL() : orientationTurtle({0,0,1}, {0,0,-1}, {0,1,0})
    {
        update();
    }
    Turtle3D orientationTurtle;//just access it?
    glm::mat4 viewMat;
    void update()
    {
        viewMat = glm::lookAt(orientationTurtle.pos, orientationTurtle.pos + orientationTurtle.dir, orientationTurtle.up);
    }
};

#endif //GLCAMERA_HPP