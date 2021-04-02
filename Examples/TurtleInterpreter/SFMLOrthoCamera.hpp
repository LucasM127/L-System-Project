#ifndef SFML_ORTHO_CAMERA_HPP
#define SFML_ORTHO_CAMERA_HPP

#include "SFMLCameraController.hpp"

struct Bounds
{
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;

    sf::Vector2f pos;
    sf::Vector2f size;
};

//2D case
class OrthoCamera : public Controller
{
public:
    OrthoCamera(sf::Window &_window);
    void handleEvent(sf::Event &event) override;
    void update(float dt) override;
    void update(float min_x, float max_x, float min_y, float max_y);
    glm::mat4 const &getMatrix();
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