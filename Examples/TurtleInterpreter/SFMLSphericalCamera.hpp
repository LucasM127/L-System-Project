#ifndef SFML_SPHERICAL_CAMERA_HPP
#define SFML_SPHERICAL_CAMERA_HPP

#include "SFMLCameraController.hpp"

//give target 'bounds'

class SphericalCamera : public Camera
{
public:
    SphericalCamera(const sf::Vector2u &winSize, float scaleFactor = 10.f, glm::vec3 targetPos = glm::vec3(0,0,0));
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override {}
    void setToBounds(const Bounds &bounds) override;
    glm::mat4 const &getMatrix() override;
private:
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;
    glm::mat4 m_VPMatrix;

    sf::Vector2u m_winSize;
    sf::Vector2i m_mouseScreenPos, m_lastScreenMousePos;
    sf::Vector2f screenToWorld(const sf::Vector2i &screenPos);
    float m_scaleFactor;//distance 'out'
    float m_phi, m_theta;
    glm::vec3 m_targetPos, m_upVector;

    void zoom(float f);
    void resize(unsigned int newWidth, unsigned int newHeight);
    void pan();//or 'rotate'

    void updateProjMat();
    void updateViewMat();

    bool amUpdated;
};

#endif //SFML_SPHERICAL_CAMERA_HPP