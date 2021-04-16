#include "AppViewer.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_impl_opengl3.h"

#include "GLShaders.hpp"

#include <string>

float c_val = 1.f;
float z_val = 0.3f;

std::atomic_bool AppViewer::killMe(false);

AppViewer::AppViewer() : m_camera(nullptr), amRunning(true), amShowingIMGUI(true),
                         m_lsystem(nullptr)
{
    setup();
}

AppViewer::~AppViewer()
{
    cleanup();
}

void AppViewer::run()
{
    sf::Event event;
    while(amRunning)
    {
        render();
        m_window.waitEvent(event);
        handleEvents(event);//modifies internal state
        //that's about it
    }
}

void AppViewer::setup()
{
    threadStarted = false;
    killMe = false;
    //Window Context
{
    unsigned int width = 640;
    unsigned int height = 640;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    m_window.create(sf::VideoMode(width,height),"LS Viewer", sf::Style::Default, settings);
}
    //OpenGL
{
    GLenum error = glewInit();
    if(error != GLEW_OK)
    {
        throw std::runtime_error("Error: " + std::string((char*)glewGetErrorString(error)));
    }

    m_glData.programID = LoadShaders("vertShader.glsl","fragShader.glsl");
    m_glData.MVPUniformID = glGetUniformLocation(m_glData.programID, "MVP");
    glUseProgram(m_glData.programID);//just the one program needed simple viewer

    glGenVertexArrays(1, &m_glData.vaoID);
    glGenBuffers(1, &m_glData.vertexBufferID);

    m_glData.numVertices = 0;
    m_glData.clearColor = {51.f/255.f,102.f/255.f,153.f/255.f};//pretty color
    glClearColor(m_glData.clearColor.r,
                 m_glData.clearColor.g,
                 m_glData.clearColor.b, 1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glLineWidth(2.f);//FLAG
}
    //IMGUI
{
    ImGui::SFML::Init(m_window, static_cast<sf::Vector2f>(m_window.getSize()));
    ImGui_ImplOpenGL3_Init();
    //set Up Theme?????
}
    //Camera .. want to switch it up...
    m_camera = new OrthoCamera(m_window);//.getSize());

    //LSystem
{
    LSYSTEM::LSData LS;
    LS.globalMap = 
    {
        {'c', c_val},
        {'z', z_val}
    };
    LS.productions =
    {
        "A(x,t) : t==0 => A(x*z,2)+A(x*(z*(c-z))^0.5,1)--A(x*(z*(c-z))^0.5,1)+A(x*(c-z),0)",
        "A(x,t) : t >0 => A(x,t-1)"
    };
    LS.homomorphisms = 
    {
        "A(x,)=>F(x)"
    };
    axiom = "A(5,0)";
    A = axiom;
    m_lsystem = new LSYSTEM::LSystem(LS);
    m_lsystem->iterate(A,18);
    oldSentence = &A;
    newSentence = &B;
    
    LineTurtleIntData data;
    data.turnAngle = 88.f;//25.f;
    Turtle3D start;
    start.rotate(90,{0,0,1});
    m_lsInterpreter.setStartTurtle(start);
    m_lsInterpreter.setDefaults(data);
    interpret();
}

}

void AppViewer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glBindVertexArray(m_glData.vaoID);
    glDrawArrays(GL_LINES, 0, m_glData.numVertices);
    glBindVertexArray(0);

    if(amShowingIMGUI)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello");
        ImGui::Text("Press 'H' to show/hide");
        if(ImGui::Button("Reset"))
        {
            if(threadStarted)
            {//kill running thread signal.
                threadStarted = false;
                killMe = true;//signal
                m_threadPtr->join();
                delete m_threadPtr;
                killMe = false;
            }
            (*oldSentence) = axiom;
            interpret();
        }
        if(ImGui::Button("iterate"))
        {
            if(!threadStarted)
            {
                threadStarted = true;
                m_threadPtr = new std::thread(&AppViewer::iterate, this);
            }
        }
        if(threadStarted)
            ImGui::Text("Iterating...");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    m_window.display();
}

//doing dual duty with update(float dt) - event based
void AppViewer::handleEvents(const sf::Event &event)
{
    if(updateMe)
    {
        if(threadStarted){
        m_threadPtr->join();
        delete m_threadPtr;}
        threadStarted = false;
        updateMe = false;
        updateGLBuffers();
    }

    ImGuiIO &imguiIO = ImGui::GetIO();

    switch (event.type)
    {
    case sf::Event::Closed:
        amRunning = false;
        break;
    case sf::Event::KeyPressed:
    {
        if(event.key.code == sf::Keyboard::Key::H)
            amShowingIMGUI = !amShowingIMGUI;
        if(event.key.code == sf::Keyboard::Key::C)
            m_camera->setToBounds(m_lsInterpreter.getBounds());
        if(event.key.code == sf::Keyboard::Key::Space)
        {
            if(!threadStarted)
            {
                threadStarted = true;
                m_threadPtr = new std::thread(&AppViewer::iterate, this);
            }
        }
    }
    default:
        break;
    }
    
    if(!imguiIO.WantCaptureMouse)//OOH!
    {
        m_camera->handleEvent(event);
    }//it is what it is
    if(m_camera)
    {
        m_glData.MVPMatrix = m_camera->getMatrix();
        glUniformMatrix4fv(m_glData.MVPUniformID, 1, GL_FALSE, &m_glData.MVPMatrix[0][0]);
    }

    if(amShowingIMGUI)
    {
        ImGui::SFML::Update(sf::Mouse::getPosition(m_window),
                            static_cast<sf::Vector2f>(m_window.getSize()),
                            sf::seconds(0.16));
        ImGui::SFML::ProcessEvent(event);
    }
}

void AppViewer::iterate()
{
    if(m_lsystem == nullptr)
        return;
    
    auto fcont = []()->bool
    {
        return !AppViewer::killMe;
    };

    m_lsystem->iterate(*oldSentence, *newSentence,fcont);
    oldSentence->clear();
    std::swap(oldSentence, newSentence);
    //m_lsystem->interpret(*oldSentence,m_lsInterpreter,fcont);//?
    interpret();
}

void AppViewer::interpret()
{
    if(m_lsystem == nullptr)
        return;
    
    m_lsystem->interpret(*oldSentence,m_lsInterpreter,[](){return !AppViewer::killMe;});
    //update VAO
    //SHOULD RETURN A STANDARD FORMAT!
    updateMe = true;
}

//some way to notify is done
void AppViewer::updateGLBuffers()
{
    auto &data = m_lsInterpreter.data();
    m_glData.numVertices = data.size();

    glBindVertexArray(m_glData.vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_glData.vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * 2 * sizeof(glm::vec3), &data[0].pos[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,2*sizeof(glm::vec3),(void*)0);//pos
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,2*sizeof(glm::vec3),(void*)offsetof(LineTurtleInterpreter::Vertex, color));

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    if(m_camera)
        m_camera->setToBounds(m_lsInterpreter.getBounds());
}

void AppViewer::cleanup()
{
    if(threadStarted)
    {
        killMe = true;//signal
        m_threadPtr->join();
        delete m_threadPtr;
    }

    glDeleteProgram(m_glData.programID);
    glDeleteVertexArrays(1, &m_glData.vaoID);//hope the order doesn't matter
    glDeleteBuffers(1, &m_glData.vertexBufferID);

    ImGui::SFML::Shutdown();

    if(m_camera != nullptr)
        delete m_camera;
    if(m_lsystem != nullptr)
        delete m_lsystem;

    m_window.close();
}