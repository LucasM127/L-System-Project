#ifndef SFML_ORTHO_CAMERA_HPP
#define SFML_ORTHO_CAMERA_HPP

#include "SFMLCameraController.hpp"

//2D case
class OrthoCamera : public Camera
{
public:
    OrthoCamera(sf::Window &_window);
    void handleEvent(const sf::Event &event) override;
    void update(float dt) override;
    void setToBounds(const Bounds &bounds);
    glm::mat4 const &getMatrix() override;
private:
    glm::mat4 m_orthoMatrix;
    sf::Window &window;

    sf::Vector2f m_viewPos;
    sf::Vector2f m_viewSize;
    sf::Vector2u m_winSize;
    sf::Vector2i m_mouseScreenPos, m_lastScreenMousePos;

    sf::Vector2f screenToWorld(const sf::Vector2i &screenPos);

    void updateMatrix();
    void zoom(float f);
    void scale(float f);
    void resize(unsigned int newWidth, unsigned int newHeight);
    void pan();
    //void updateMousePos(int mouse_x, int mouse_y);
};

#endif //SFML_ORTHO_CAMERA_HPP