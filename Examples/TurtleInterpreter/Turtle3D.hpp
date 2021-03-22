#ifndef THREEDEE_TURTLE_HPP
#define THREEDEE_TURTLE_HPP

#include <glm/gtc/quaternion.hpp>
//geometric Interpretation...

struct Turtle3D
{
    Turtle3D() : pos(0,0,0), dir(0,0,1), up(0,1,0), scale(1.f), ctr(0) {}
    //Position, Direction, Up (Orthonormal)
    Turtle3D(glm::vec3 p, glm::vec3 d, glm::vec3 u) : pos(p), dir(d), up(u), scale(1.f), ctr(0) {}
    Turtle3D(const Turtle3D &T) = default;//just copy over
    
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;//my up vector
    float scale;
    int ctr;
    void setDir(glm::vec3 v)
    {//rotate to dir // as long as not 'parallel'
        v = glm::normalize(v);
        float angle = acosf(glm::dot(v,dir));
        glm::quat q = glm::angleAxis(angle, glm::cross(v, dir));
        dir = dir * q;
        up = up * q;
/*
        dir = glm::normalize(v);
        //as long as up and 'v' are not parallel, the perpendicular component => new 'up'
        up = up - (glm::dot(up, dir) * dir);
        up = glm::normalize(up);*/
    }
    void advance(float x)
    {
        pos += dir * x;
    }
    //Angle in degrees, rotationAxis normalized
    void rotate(float angle, const glm::vec3 &rotationAxis)
    {
        fix(angle);
        //rotationAxis = glm::normalize(rotationAxis);
        glm::quat q = glm::angleAxis(angle, rotationAxis);
        dir = dir * q;
        up = up * q;
    }
    void pitch(float angle)
    {
        fix(angle);
        glm::quat q = glm::angleAxis(angle, glm::cross(up,dir));//around 'left axis'
        dir = dir * q;
        up = up * q;
    }
    void roll(float angle)
    {
        fix(angle);
        glm::quat q = glm::angleAxis(angle, dir);
        up = up * q;
    }
    void turn(float angle)
    {
        fix(angle);//necessary????
        glm::quat q = glm::angleAxis(angle, up);
        dir = dir * q;
    }
    void fix(float &angle)
    {
        angle = glm::radians(angle);
        /*
        ctr++;
        if(ctr > 100)
        {
            dir = glm::normalize(dir);
            up = glm::normalize(up);
            ctr -= 100;
        }*/
    }
};

#endif //THREEDEE_TURTLE_HPP