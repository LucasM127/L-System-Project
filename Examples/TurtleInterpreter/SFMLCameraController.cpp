#include "SFMLCameraController.hpp"
/*
GLCameraController()
{
    float nearClip = 0.1f;
    float farClip = 10.f;
//    m_projMat = glm::ortho(width,height,depth);
    m_projMat = glm::perspective(glm::radians(90.f), float(viewWidth)/float(viewHeigth), nearClip, farClip);
    //view projection
    //resize events
    
    glm::mat4 projMat = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
    glm::mat4 projMat = glm::perspective(glm::radians(90.f),16.f/9.f,0.1f,5.f);
    glm::mat4 MVPMat = projMat *camera.viewMat;//* viewMat;
}*/

SphericalCameraController::SphericalCameraController(sf::Vector2u viewSz)
 : amUpdated(false), m_zoomScale(10.f), m_angularSpeed(90.f), m_theta(0.f), m_phi(90.f),
   m_upVector(0,1,0), m_targetPos(0,0,0)
{
    updateView(viewSz);
    updateViewMat();
}

void SphericalCameraController::updateProjMat()
{
    //use an orthographic view...
    float minX, maxX, minY, maxY, minZ, maxZ;

    float aspectRatio = m_viewWidth/m_viewHeight;

    if(aspectRatio > 1.f)//wider
    {
        minY = -m_zoomScale / 2.f;
        maxY =  m_zoomScale / 2.f;
        minX = aspectRatio * minY;
        maxX = aspectRatio * maxY;
    }
    else
    {
        minX = -m_zoomScale / 2.f;
        maxX =  m_zoomScale / 2.f;
        minY = minX / aspectRatio;
        maxY = maxX / aspectRatio;
    }

    minZ = -m_zoomScale / 2.f;
    maxZ =  m_zoomScale / 2.f;
    
    m_projMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    amUpdated = true;
}

void SphericalCameraController::updateViewMat()
{
    float yPos = cosf(glm::radians(m_phi));
    float xPos = sinf(glm::radians(m_phi)) * sinf(glm::radians(m_theta));
    float zPos = -sinf(glm::radians(m_phi)) * cosf(glm::radians(m_theta));
    glm::vec3 cameraPos(xPos, yPos, zPos);
    
//    m_viewMatrix = glm::lookAt(m_targetPos + cameraPos, m_targetPos, m_upVector);
    m_viewMatrix = glm::lookAt(m_targetPos, m_targetPos + cameraPos, m_upVector);
    amUpdated = true;
}

void SphericalCameraController::updateView(sf::Vector2u viewSz)
{
    m_viewWidth = viewSz.x;
    m_viewHeight = viewSz.y;
    updateProjMat();
}

void SphericalCameraController::setZoom(float zoom)
{
    m_zoomScale = zoom;
    updateProjMat();
}

void SphericalCameraController::handleEvent(sf::Event &event)
{}

void SphericalCameraController::update(float dt)
{
    bool changed = false;
    bool updateProj = false;
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        changed = true;
        m_phi -= m_angularSpeed * dt;
        if(m_phi < 1.f) m_phi = 1.f;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        changed = true;
        m_phi += m_angularSpeed * dt;
        if(m_phi > 179.f) m_phi = 179.f;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        changed = true;
        m_theta += m_angularSpeed * dt;
        if(m_theta > 360.f) m_theta -= 360.f;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        changed = true;
        m_theta -= m_angularSpeed * dt;
        if(m_theta < 0.f) m_theta += 360.f;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        updateProj = true;
        //20% of current
        m_zoomScale += m_zoomScale * dt;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        updateProj = true;
        m_zoomScale -= m_zoomScale * dt;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {//move my camera center!
        changed = true;
        m_targetPos.y += m_zoomScale * dt;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        changed = true;
        m_targetPos.y -= m_zoomScale * dt;
    }
    if(changed)
    {
        updateViewMat();//where the camera is
    }
    if(updateProj)
        updateProjMat();
}

glm::mat4 const &GLCameraController::getVPMat()
{
    if(amUpdated)
    {
        m_VPMatrix = m_projMatrix * m_viewMatrix;
        amUpdated = false;
    }
    return m_VPMatrix;
}