#ifndef APP_VIEWER_HPP
#define APP_VIEWER_HPP

#include <SFML/Window.hpp>
#include <GL/glew.h>

#include "SFMLCameraController.hpp"
#include "TurtleInterpreter.hpp"

#include <atomic>
#include <thread>

struct GLData
{
    GLuint programID;
    GLuint vertexBufferID;
    GLuint vaoID;
    GLuint MVPUniformID;
    glm::vec3 clearColor;
    glm::mat4 MVPMatrix;
    unsigned int numVertices;
};

class AppViewer
{
public:
    AppViewer();
    ~AppViewer();
    void run();
private:
    void setup();
    void render();
    void handleEvents(const sf::Event &event);
    void updateGLBuffers();
    void cleanup();
    sf::Window m_window;
    Camera *m_camera;
    bool amRunning;
    bool amShowingIMGUI;

    //OGL STUFF
    GLData m_glData;

    //LSYSTEM STUFFS
    LineTurtleInterpreter m_lsInterpreter;
    LSYSTEM::LSystem *m_lsystem;
    LSYSTEM::LSystem *m_lsystem2;
    LSYSTEM::VLSentence A,B;
    LSYSTEM::VLSentence axiom;
    LSYSTEM::VLSentence *oldSentence, *newSentence;

    //THREAD STATES
    std::atomic_bool updateMe;//thread is finished
    std::atomic_bool threadStarted;//thread is off. don 't iterate again pls
    static std::atomic_bool killMe;
    std::thread *m_threadPtr;

    void iterate();
    void interpret();
};

#endif //APP_VIEWER_HPP