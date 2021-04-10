#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "TurtleInterpreter.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_impl_opengl3.h"
#include "GLShaders.hpp"

struct LSBufferData//for ImGui
{
    static const unsigned int maxBufferLength = 128;//arbitrary
    static const unsigned int maxNumProductions = 8;
    unsigned int numProductions = 0;
    unsigned int numDecompositions = 0;
    unsigned int numHomomorphisms = 0;
    unsigned int numGlobals = 0;

    //globals?
    std::pair<char, float> globals[maxNumProductions];

    std::array<char, maxNumProductions * maxBufferLength> productionCharData;
    std::array<char, maxNumProductions * maxBufferLength> decompositionCharData;
    std::array<char, maxNumProductions * maxBufferLength> homomorphismCharData;
} buffer;

LSYSTEM::LSData convert(LSBufferData &B)
{
    LSYSTEM::LSData L;
    return L;
    //for(auto i : B.productions)
    //    L.productions.emplace_back(&B.data[i]);
    //for(auto i : B.decompositions)
    //    L.decompositions.emplace_back(&B.data[i]);
    //for(auto i : B.homomorphisms)
    //    L.homomorphisms.emplace_back(&B.data[i]);
}

//No validation or anything yet
//Living on the edge lol
void imguiLSDataEditor(LSBufferData &B)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Creator");
    
    ImGui::Text("Globals");
    for(unsigned int i = 0; i <= B.numGlobals; ++i)
    {
        ImGui::PushItemWidth(16.f);
        ImGui::PushID(i);
        ImGui::InputText("id", &B.globals[i].first, 2);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(128.f);
        ImGui::InputFloat("f", &B.globals[i].second);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::SameLine();
    if(ImGui::Button("add##G")) ++B.numGlobals;

    ImGui::PushItemWidth(256.f);
    ImGui::Text("Productions:");
    for(unsigned int i = 0; i <= B.numProductions; ++i)
    {
        ImGui::PushID(i+8);
        ImGui::InputText(std::to_string(i).c_str(), &B.productionCharData[128*i], 128);
        ImGui::PopID();
    }
    ImGui::SameLine();
    if(ImGui::Button("add##P")) ++B.numProductions;

    ImGui::Text("Decompositions:");
    for(unsigned int i = 0; i <= B.numDecompositions; ++i)
    {
        ImGui::PushID(i+16);
        ImGui::InputText(std::to_string(i).c_str(), &B.decompositionCharData[128 * i], 128);
        ImGui::PopID();
    }
    ImGui::SameLine();
    if(ImGui::Button("add##D")) ++B.numDecompositions;

    ImGui::Text("Homomorphisms:");
    for(unsigned int i = 0; i <= B.numHomomorphisms; ++i)
    {
        ImGui::PushID(i+24);
        ImGui::InputText(std::to_string(i).c_str(), &B.homomorphismCharData[128 * i], 128);
        ImGui::PopID();
    }
    ImGui::SameLine();
    if(ImGui::Button("add##H")) ++B.numHomomorphisms;
    ImGui::PopItemWidth();
    ImGui::End();
}

void convertToImGui(const LSYSTEM::LSData &lsData)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin(lsData.label.c_str());
    ImGui::Text("Axiom:");
    ImGui::Text("FF");
    if(lsData.globalMap.size())
    {
        ImGui::Text("Globals");//function of t? function of i?
        for(auto &pair : lsData.globalMap)
        {
            //ImGui::Text();
            float f = pair.second;
            ImGui::InputFloat(std::string(1,pair.first).c_str(),&f);
        }
    }
    if(lsData.productions.size())
    {
        ImGui::Text("Productions:");
        for(auto &s : lsData.productions)
        {
            ImGui::Text(s.c_str());
        }
    }
    if(lsData.decompositions.size())
    {
        ImGui::Text("Decompositions:");
        for(auto &s : lsData.decompositions)
        {
            ImGui::Text("%s", s.c_str());
        }
    }
    if(lsData.homomorphisms.size())
    {
        ImGui::Text("Homomorphisms:");
        for(auto &s : lsData.homomorphisms)
        {
            ImGui::Text("%s", s.c_str());
        }
    }
    if(ImGui::Button("Iterate"));
    ImGui::End();
}

sf::Window window;
bool isRunning = true;
LSYSTEM::LSData lsData;
//HMMMM

struct GLData
{
    GLuint programID;
    GLuint vertexBufferID;
    GLuint vaoID;
    GLuint MVPUniformID;
    glm::vec3 clearColor;
    glm::mat4 MVPMatrix;
    unsigned int numVertices;
} glData;

void setup()
{
    //Window Context
{
    unsigned int width = 640;
    unsigned int height = 640;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    window.create(sf::VideoMode(width,height),"LS Viewer", sf::Style::Default, settings);
}
    //OpenGL
{
    GLenum error = glewInit();
    if(error != GLEW_OK)
    {
        throw std::runtime_error("Error: " + std::string((char*)glewGetErrorString(error)));
    }

    glData.programID = LoadShaders("vertShader.glsl","fragShader.glsl");
    glData.MVPUniformID = glGetUniformLocation(glData.programID, "MVP");
    glUseProgram(glData.programID);//just the one program needed simple viewer

    glGenVertexArrays(1, &glData.vaoID);
    glGenBuffers(1, &glData.vertexBufferID);

    glData.numVertices = 0;
    glData.clearColor = {51.f/255.f,102.f/255.f,153.f/255.f};//pretty color
    glClearColor(glData.clearColor.r,
                 glData.clearColor.g,
                 glData.clearColor.b, 1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glLineWidth(2.f);//FLAG
}
    //IMGUI
{
    ImGui::SFML::Init(window, static_cast<sf::Vector2f>(window.getSize()));
    ImGui_ImplOpenGL3_Init();
    //set Up Theme?????
}

}

void cleanup()
{
    glDeleteProgram(glData.programID);
    glDeleteVertexArrays(1, &glData.vaoID);//hope the order doesn't matter
    glDeleteBuffers(1, &glData.vertexBufferID);

    ImGui::SFML::Shutdown();

    window.close();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    //display
    glBindVertexArray(glData.vaoID);
    glDrawArrays(GL_LINES, 0, glData.numVertices);
    glBindVertexArray(0);

    imguiLSDataEditor(buffer);
//    convertToImGui(lsData);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window.display();
}

void handleEvents()
{
    ImGuiIO &imguiIO = ImGui::GetIO();
    sf::Event event;
    window.waitEvent(event);

    switch (event.type)
    {
    case sf::Event::Closed:
        isRunning = false;
        break;
    case sf::Event::KeyPressed:
    {
        break;
    }
    default:
        break;
    }
    
    ImGui::SFML::Update(sf::Mouse::getPosition(window),
                        static_cast<sf::Vector2f>(window.getSize()),
                        sf::seconds(0.16));
    ImGui::SFML::ProcessEvent(event);
}

int main()
{
    //I DONT LIKE IT :!
    lsData.label = "Sierpinski Triangle";
    lsData.globalMap = 
    {
        {'a', 5.f},
        {'b', 4.f}
    };
    lsData.productions = 
    {
        "F => F-G+F+G-F",
        "G => GG"
    };
    lsData.homomorphisms = 
    {
        "G => F"
    };

    for(auto &c : buffer.productionCharData) c = '\0';
    for(auto &c : buffer.decompositionCharData) c = '\0';
    for(auto &c : buffer.homomorphismCharData) c = '\0';

    setup();

    while(isRunning)
    {
        render();
        handleEvents();
    }

    return 0;
}