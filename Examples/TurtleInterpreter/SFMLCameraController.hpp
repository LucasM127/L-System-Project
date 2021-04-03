#ifndef SFML_CAMERA_HPP
#define SFML_CAMERA_HPP

#include <SFML/Window.hpp>
#include "Turtle3D.hpp"

class Camera
{
public:
    Camera(){}
    virtual ~Camera(){}
    virtual void handleEvent(const sf::Event &event) = 0;
    virtual void update(float dt) = 0;
    virtual void setToBounds(const Bounds &bounds) = 0;
    virtual glm::mat4 const &getMatrix() = 0;
};//the window?

#include "SFMLSphericalCamera.hpp"
#include "SFMLOrthoCamera.hpp"

/*
class SphericalCameraController : public GLCameraController
{
public:
    SphericalCameraController(sf::Vector2u viewSz);
    void handleEvent(const sf::Event &event) override;
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
};*/

#endif //SFML_CAMERA_CONTROLLER_HPP