#ifndef SFML_CAMERA_CONTROLLER_HPP
#define SFML_CAMERA_CONTROLLER_HPP

#include <SFML/Window.hpp>
#include "Turtle3D.hpp"

class Controller
{
public:
    Controller(){}
    virtual ~Controller(){}
    virtual void handleEvent(sf::Event &event) = 0;
    virtual void update(float dt) = 0;
};//the window?

//derive from controller... sfmlcontrolelr?
class GLCameraController : public Controller
{
public:
    GLCameraController(){}
    virtual ~GLCameraController(){}
    virtual void handleEvent(sf::Event &event) = 0;
    virtual void update(float dt) = 0;
protected:
    glm::mat4 m_viewMatrix, m_projMatrix;//glViewCommand?
};

class SphericalCameraController : public GLCameraController
{
public:
    SphericalCameraController(sf::Vector2u viewSz);
    void handleEvent(sf::Event &event) override;
    void update(float dt) override;
    void updateView(sf::Vector2u viewSz);
    void setZoom(float zoom);
    glm::mat4 const &getVPMat();
private:
    void updateProjMat();
    void updateViewMat();
    bool amUpdated;
    float m_zoomScale;
    float m_viewWidth, m_viewHeight;
    float m_angularSpeed;
    float m_theta, m_phi;
    glm::vec3 m_upVector;
    glm::vec3 m_targetPos;
    glm::mat4 m_VPMatrix;
};

#endif //SFML_CAMERA_CONTROLLER_HPP