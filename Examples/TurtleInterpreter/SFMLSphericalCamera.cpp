#include "SFMLSphericalCamera.hpp"

#include <GL/glew.h>

SphericalCamera::SphericalCamera(const sf::Vector2u &winSize, float scaleFactor, glm::vec3 targetPos)
    : m_winSize(winSize), m_scaleFactor(scaleFactor),
    m_phi(90.f), m_theta(0.f), m_targetPos(targetPos), m_upVector(0,1,0)
{
    updateProjMat();
    updateViewMat();
}

void SphericalCamera::handleEvent(const sf::Event &event)
{
    switch (event.type)
    {
    case sf::Event::MouseWheelScrolled:
        if(event.mouseWheelScroll.delta > 0)
            zoom(0.8);
        else
            zoom(1.25f);
        break;
    case sf::Event::MouseMoved:
        m_mouseScreenPos.x = event.mouseMove.x;
        m_mouseScreenPos.y = event.mouseMove.y;
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            pan();
        m_lastScreenMousePos = m_mouseScreenPos;
        break;
    case sf::Event::Resized:
        resize(event.size.width, event.size.height);
        break;
    default:
        break;
    }
}

glm::mat4 const &SphericalCamera::getMatrix()
{
    if(amUpdated)
    {
        m_VPMatrix = m_projMatrix * m_viewMatrix;
        amUpdated = false;
    }
    return m_VPMatrix;
}

sf::Vector2f SphericalCamera::screenToWorld(const sf::Vector2i &screenPos)
{
    return {0,0};
}

void SphericalCamera::zoom(float f)
{
    m_scaleFactor *= f;

    updateProjMat();
}

void SphericalCamera::resize(unsigned int newWidth, unsigned int newHeight)
{
    m_winSize.x = newWidth;
    m_winSize.y = newHeight;

    updateProjMat();

    glViewport(0,0,newWidth,newHeight);
}

void SphericalCamera::pan()//or rotate
{
    //updateViewMat
    sf::Vector2i mouseDelta = m_mouseScreenPos - m_lastScreenMousePos;
    float rel_dy = (float)mouseDelta.y / (float)m_winSize.y;
    float rel_dx = (float)mouseDelta.x / (float)m_winSize.x;

    float aspectRatio = (float)m_winSize.x/(float)m_winSize.y;
    float dtheta = rel_dx * 360.f * aspectRatio;
    float dphi = rel_dy * 180.f;

    m_phi += dphi;
    if(m_phi > 179.f)
        m_phi = 179.f;
    if(m_phi < 1.f)
        m_phi = 1.f;
    m_theta += dtheta;
    if(m_theta > 360.f)
        m_theta -= 360.f;
    if(m_theta < 0.f)
        m_theta += 360.f;

    updateViewMat();
}

void SphericalCamera::updateProjMat()
{
    //orthographic view
    float min_x, max_x, min_y, max_y, min_z, max_z;
    float aspectRatio = (float)m_winSize.x/(float)m_winSize.y;

    if(aspectRatio > 1.f)//wider
    {
        min_y = -m_scaleFactor / 2.f;
        max_y =  m_scaleFactor / 2.f;
        min_x = aspectRatio * min_y;
        max_x = aspectRatio * max_y;
    }
    else
    {
        min_x = -m_scaleFactor / 2.f;
        max_x =  m_scaleFactor / 2.f;
        min_y = min_x / aspectRatio;
        max_y = max_x / aspectRatio;
    }

    min_z = -m_scaleFactor / 2.f;
    max_z =  m_scaleFactor / 2.f;

    m_projMatrix = glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);
    amUpdated = true;
}

void SphericalCamera::updateViewMat()
{
    float y_pos =  cosf(glm::radians(m_phi));
    float x_pos =  sinf(glm::radians(m_phi)) * sinf(glm::radians(m_theta));
    float z_pos = -sinf(glm::radians(m_phi)) * cosf(glm::radians(m_theta));
    glm::vec3 cameraPos(x_pos, y_pos, z_pos);

    m_viewMatrix = glm::lookAt(m_targetPos, m_targetPos + cameraPos, m_upVector);
    amUpdated = true;
}

void SphericalCamera::setToBounds(const Bounds &bounds)
{
    m_scaleFactor = bounds.maxDimension();
    m_targetPos = bounds.centroid();
    updateViewMat();
    updateProjMat();
}