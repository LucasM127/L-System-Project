#include "SFMLOrthoCamera.hpp"
//#include <iostream>
//sf::Vector2f mapMousePos(int x, int, y, unsigned int szx, unsigned int szy, bounds);
#include <GL/glew.h>

OrthoCamera::OrthoCamera(sf::Window &_window) : window(_window)
{
    m_winSize = window.getSize();
    m_viewPos = sf::Vector2f(-1.f,-1.f);
    m_viewSize = sf::Vector2f(2.f,2.f);
}

glm::mat4 const &OrthoCamera::getMatrix()
{
    return m_orthoMatrix;
}

void OrthoCamera::handleEvent(const sf::Event &event)
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

void OrthoCamera::setToBounds(const Bounds &bounds)
{
    //width height and ... aspectRatio! winSize! (figure that out later :/)
    float width = (bounds.max_x - bounds.min_x);
    float height = (bounds.max_y - bounds.min_y);

    if(width > height)
    {
        m_viewPos.x = bounds.min_x;
        m_viewSize.x = bounds.max_x - bounds.min_x;
        float y_offset = (width - height) / 2.f;
        m_viewPos.y = bounds.min_y - y_offset;
        m_viewSize.y = m_viewSize.x;//1:1
    }
    else
    {
        m_viewPos.y = bounds.min_y;
        m_viewSize.y = bounds.max_y - bounds.min_y;
        float x_offset = (height - width) / 2.f;
        m_viewPos.x = bounds.min_x - x_offset;
        m_viewSize.x = m_viewSize.y;//1:1
    }
    
    scale(1.1f);

    updateMatrix();
}

void OrthoCamera::resize(unsigned int newWidth, unsigned int newHeight)
{
    //aspect ratio?
    //keep same 'scale' factor
    float widthRatio = (float)newWidth/(float)m_winSize.x;
    float heightRatio = (float)newHeight/(float)m_winSize.y;

    m_winSize.x = newWidth;
    m_winSize.y = newHeight;

    //resize size of bounds...
    m_viewSize.x *= widthRatio;
    m_viewSize.y *= heightRatio;

    glViewport(0,0,newWidth,newHeight);
//    glScissor(0,0,newWidth,newHeight);
    updateMatrix();
}

void OrthoCamera::scale(float f)
{
    //like zoom but based on middle of view bounds pos
    sf::Vector2f oldSz = m_viewSize;
    m_viewSize *= f;
    sf::Vector2f offset = (m_viewSize - oldSz)/2.f;
    m_viewPos -= offset;
}

void OrthoCamera::zoom(float f)
{
    sf::Vector2f oldMousePos = screenToWorld(m_mouseScreenPos);

    sf::Vector2f newSize = m_viewSize * f;
    m_viewSize = newSize;

    sf::Vector2f updatedMousePos = screenToWorld(m_mouseScreenPos);
    
    sf::Vector2f offset = oldMousePos - updatedMousePos;
    m_viewPos += offset;

    updateMatrix();
}

sf::Vector2f OrthoCamera::screenToWorld(const sf::Vector2i &screenPos)
{
    sf::Vector2f relPos;
    relPos.x = (float)screenPos.x / (float)m_winSize.x;
    relPos.y = (float)screenPos.y / (float)m_winSize.y;
    relPos.y = 1.f - relPos.y;//opengl transform

    sf::Vector2f worldPos;
    worldPos.x = (relPos.x * m_viewSize.x) + m_viewPos.x;
    worldPos.y = (relPos.y * m_viewSize.y) + m_viewPos.y;

    //std::cout<<relPos.x<<" "<<relPos.y<<" ";

    return worldPos;
}

void OrthoCamera::pan()
{
    sf::Vector2f pos = screenToWorld(m_mouseScreenPos);
    sf::Vector2f lastPos = screenToWorld(m_lastScreenMousePos);
    sf::Vector2f offset = pos - lastPos;
    m_viewPos -= offset;

    updateMatrix();
}

void OrthoCamera::updateMatrix()
{
    m_orthoMatrix = glm::ortho( m_viewPos.x, m_viewPos.x + m_viewSize.x,
                                m_viewPos.y, m_viewPos.y + m_viewSize.y,
                                -1.f,1.f);
}

void OrthoCamera::update(float dt){}