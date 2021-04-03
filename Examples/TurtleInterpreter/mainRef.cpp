//Turtle Interpreter...
//Rewrite the classic turtle code i use to create '_' pipe segments (ie copy pasta)
//Should I try add branching joining ???
//Or just transform the rectangles...
//Nah, just simple.  can try a fancy version later if I still care

#include "TurtleInterpreter.hpp"
#include <SFML/Graphics.hpp>//Window.hpp>//Graphics.hpp>
#include <iostream>
#include "../../Containers/OstreamOperators.hpp"
#include <GL/glew.h>
#include "GLShaders.hpp"

#include "SFMLOrthoCamera.hpp"
#include "SFMLSphericalCamera.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui_impl_opengl3.h"

void updateGLBufferData(const float* vertices, uint numVertices, GLuint vao, GLuint vertexBuffer);

bool setUpGlew()
{
    GLenum error = glewInit();
    if(error != GLEW_OK)
    {
        std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
        return false;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\n";
    return true;
}

void updateGLBufferData(const float* vertices, uint numVertices, GLuint vao, GLuint vertexBuffer)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,2*sizeof(glm::vec3),(void*)0);//pos
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,2*sizeof(glm::vec3),(void*)offsetof(LineTurtleInterpreter::Vertex, color));

    glBindVertexArray(0);
}

int main(int argc, char **argv)
{
    
    std::string lsFile = "../../LSFiles/abopSignalPropagationExample.ls";//  foo.ls";

    if(argc >= 2)
    {
        lsFile = argv[1];
    }

    LSFile fileLoader;
    fileLoader.loadFile(lsFile);
    
    LSYSTEM::LSData LS;
    LS.productions =
    {
        "6 => 81++91----71[-81----61]++",
        "7 => +81--91[---61--71]+",
        "8 => -61++71[+++81++91]-",
        "9 => --81++++61[+91++++71]--71",
        "1 =>"
    };
    LS.homomorphisms =
    {
        "6=>F",//~(1,0,0)F",
        "7=>F",//~(1,1,0)F",
        "8=>F",//~(1,0,1)F",
        "9=>F",//~(1,0,0)F"
    };

    LSYSTEM::LSystem lsystem(fileLoader.lsData());//LS);//fileLoader.lsData());

    LSYSTEM::VLSentence A(fileLoader.axiom());//"[7]++[7]++[7]++[7]++[7]");//fileLoader.axiom());
    LSYSTEM::VLSentence B;
    LSYSTEM::VLSentence *oldSentence = &A;
    LSYSTEM::VLSentence *newSentence = &B;

    
    for(uint i = 0; i < 11; ++i)
    {
        lsystem.iterate(*oldSentence, *newSentence);
        oldSentence->clear();
        std::swap(oldSentence,newSentence);
    }

    Turtle3D turtle;
    turtle.pos.x = turtle.pos.y = 0.f;// 10.f;
    LineTurtleInterpreter T(turtle);
    LineTurtleIntData LTID;//read from file somehow
    LTID.turnAngle = 36;//45;
    T.setDefaults(LTID);
    lsystem.interpret(*oldSentence,T);
    std::cout<<oldSentence->getLSentence()<<std::endl;

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    sf::RenderWindow window(sf::VideoMode(800,800),"Tree?", 7U, settings);
//    sf::Window window(sf::VideoMode(800,800),"Tree?", 7U, settings);
    std::cout<<"Window settings: "<<window.getSettings().majorVersion<<" "<<window.getSettings().minorVersion<<" "
                <<window.getSettings().depthBits<<" "<<window.getSettings().stencilBits<<"\n";
    window.setActive(true);
    //window.resetGLStates();
    sf::Event event;

    if(!setUpGlew())
        return -1;

    //IMGUI
    ImGui::SFML::Init(window, static_cast<sf::Vector2f>(window.getSize()));
    ImGui_ImplOpenGL3_Init();

    GLuint programID = LoadShaders("vertShader.glsl","fragShader.glsl");
    GLuint MVPID = glGetUniformLocation(programID, "MVP");
    glUseProgram(programID);

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0));//y is up 
    glm::mat4 projMat = glm::mat4(1.f);

    //set up Buffers....
    GLuint vao;
    GLuint vertexBuffer;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1,&vertexBuffer);

    updateGLBufferData(&T.data()[0].pos[0], T.data().size(), vao, vertexBuffer);

    glClearColor(51.f/255.f,102.f/255.f,153.f/255.f,1.f);//0.7f,.5f,.5f,1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glLineWidth(2.f);
//    OrthoCamera camera(window);
    float dist = fmaxf(fmaxf(T.max_x-T.min_x,T.max_y-T.min_y),T.max_z-T.min_z);
    glm::vec3 pos((T.max_x+T.min_x)/2.f,(T.max_y+T.min_y)/2.f,(T.max_z+T.min_z)/2.f);//centroid

    SphericalCamera camera(window.getSize(),dist,pos);//10,{0,25,0});//, dist, pos);
    //camera.update(T.min_x, T.max_x, T.min_y, T.max_y);
    glm::mat4 MVP = camera.getMatrix();
    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
    std::cout<<T.data().size()<<" vertices\n";//37942 vertices as is for pinecone

    sf::Clock clock;
    bool cameraIsLocked = true;
    bool showImGui = true;
    while (window.isOpen())
    {
        if(showImGui)
        {
        ImGui::SFML::Update(sf::Mouse::getPosition(window),static_cast<sf::Vector2f>(window.getSize()),clock.restart());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello");
        ImGui::Text("Press 'H' to show/hide");
        ImGui::Checkbox("Lock Camera", &cameraIsLocked);
        ImGui::End();
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, T.data().size());
        glBindVertexArray(0);

        if(showImGui)
        {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
//App Loop Crap
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Key::H)
                showImGui = !showImGui;//toggle(showImGui) //inline void
            if(event.key.code == sf::Keyboard::Key::C)
                cameraIsLocked = !cameraIsLocked;//toggle(showImGui) //inline void
            /*
            lsystem.iterate(*oldSentence, *newSentence);
            oldSentence->clear();
            std::swap(oldSentence,newSentence);
            lsystem.interpret(*oldSentence,T);
//            std::cout<<oldSentence->getLSentence()<<"\n";
//            camera.update(T.min_x, T.max_x, T.min_y, T.max_y);
            MVP = camera.getMatrix();
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
            updateGLBufferData(&T.data()[0].pos[0], T.data().size(), vao, vertexBuffer);
            */
        }
        if(!cameraIsLocked) camera.handleEvent(event);
        if(showImGui)
            ImGui::SFML::ProcessEvent(event);
        MVP = camera.getMatrix();
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

    }

    ImGui::SFML::Shutdown();

    return 0;
}

/*********
glm::vec3 boundsVertices[16];
GLuint boundsVAO;
GLuint boundsBuffer;

void updateBounds(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z)
{
    boundsVertices[ 0] = glm::vec3(min_x, min_y, 0);//A
    boundsVertices[ 1] = glm::vec3(1,1,0);
    boundsVertices[ 2] = glm::vec3(min_x, max_y, 0);//B
    boundsVertices[ 3] = glm::vec3(1,1,0);
    boundsVertices[ 4] = glm::vec3(min_x, max_y, 0);//B
    boundsVertices[ 5] = glm::vec3(1,1,0);
    boundsVertices[ 6] = glm::vec3(max_x, max_y, 0);//C
    boundsVertices[ 7] = glm::vec3(1,1,0);
    boundsVertices[ 8] = glm::vec3(max_x, max_y, 0);//C
    boundsVertices[ 9] = glm::vec3(1,1,0);
    boundsVertices[10] = glm::vec3(max_x, min_y, 0);//D
    boundsVertices[11] = glm::vec3(1,1,0);
    boundsVertices[12] = glm::vec3(max_x, min_y, 0);//D
    boundsVertices[13] = glm::vec3(1,1,0);
    boundsVertices[14] = glm::vec3(min_x, min_y, 0);//A
    boundsVertices[15] = glm::vec3(1,1,0);
    updateGLBufferData(&boundsVertices[0][0], 8, boundsVAO, boundsBuffer);
}
***************/