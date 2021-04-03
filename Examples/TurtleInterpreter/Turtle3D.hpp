#ifndef THREEDEE_TURTLE_HPP
#define THREEDEE_TURTLE_HPP

#include <glm/gtc/quaternion.hpp>
//geometric Interpretation...

struct Bounds
{
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;
    inline float maxDimension() const
    {
        return fmaxf(fmaxf(max_x-min_x,max_y-min_y),max_z-min_z);
    }
    inline glm::vec3 centroid() const
    {
        return glm::vec3((max_x+min_x)/2.f,(max_y+min_y)/2.f,(max_z+min_z)/2.f);
    }
};

//A 3D 'Orientation' in space.  Not a transform (ie stretch/squish)
//Takes input in degrees not radians
struct Turtle3D
{
    Turtle3D() : pos(0,0,0), dir(0,1,0), up(0,0,1), ctr(0) {}
    //Position, Direction, Up (Orthonormal)
    Turtle3D(glm::vec3 p, glm::vec3 d, glm::vec3 u) : pos(p), dir(d), up(u), ctr(0) {}
    Turtle3D(const Turtle3D &T) = default;//just copy over
    
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
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
    void rotate(float angle, const glm::vec3 &rotationAxis)
    {
        fix(angle);
        glm::quat q = glm::angleAxis(angle, rotationAxis);
        dir = dir * q;
        up = up * q;
    }
    void pitch(float angle)
    {//ie look up or down
        fix(angle);
        glm::quat q = glm::angleAxis(angle, glm::cross(up,dir));//around 'left axis'
        dir = dir * q;
        up = up * q;
    }
    void roll(float angle)
    {//motion sickness one
        fix(angle);
        glm::quat q = glm::angleAxis(angle, dir);
        up = up * q;
    }
    void turn(float angle)
    {//left right
        fix(angle);
        glm::quat q = glm::angleAxis(angle, up);
        dir = dir * q;
    }
    void fix(float &angle)
    {
        angle = glm::radians(angle);
        //seems to work
        ctr++;
        if(ctr > 100)
        {
            dir = glm::normalize(dir);
            up = glm::normalize(up);
            ctr -= 100;
        }
    }
};

#endif //THREEDEE_TURTLE_HPP