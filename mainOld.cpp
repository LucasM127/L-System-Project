#include "LSLoader.hpp"
#include "OpenGLTurtle.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>

#include "GLCamera.hpp"
//maybe i should have the lsloader, create the lsystem
//once is created, the lsystem owns the eval loader so that is fine, 
//the string references to the evals have been copied over or written, so we dont need the lsloader anymore...

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

struct SkyBox
{
    SkyBox();
    GLuint m_vao;
    GLuint m_buffer;
    void draw();
    glm::vec3 m_vertices[12];
};

SkyBox::SkyBox() : m_vertices
    {// pos       color
        {-1,-1,0}, {0.3,1,0.4},
        { 1,-1,0}, {0.3,1,0.4},
        {-1, 1,0}, {0,0,1},
        {-1, 1,0}, {0,0,1},
        { 1,-1,0}, {0.3,1,0.4},
        { 1, 1,0}, {0,0,1}
    }
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_buffer);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(float), (void*)(3*sizeof(float)) );//(sizeof(glm::vec3)));
    glBindVertexArray(0);
}

void SkyBox::draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES,0,12);
    glBindVertexArray(0);
}

int main(int argc, char **argv)
{
    if(argc != 2) return 0;
    std::string file = argv[1];

    LSYSTEM::LSystem *L = nullptr;

    LSYSTEM::LSLoader loader;
    try
    {
        L = loader.loadFile(file);
    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<"\n";
        return 0;
    }
    //I Need something... is it the pnm? 
    //I THINK I NEED TO PASS THE PARANNUMMAP BY VAL
    // i need the solib open, but not the f

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    sf::Window window(sf::VideoMode(1920,1080,32), "Turtle", sf::Style::Fullscreen, settings);//  7U, settings);
    
    GLenum error = glewInit();
    if(error != GLEW_OK)
    {
        std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
        return -1;
    }
    else
    {
        std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\n";
    }
/*
//or transform matrix it :?
    glm::vec3 pos(0.f,-0.3f,-1.2f);
        glm::vec3 dir(0.f,-1.f,0.f);
        glm::vec3  up(0.f,0.f,-1.f);
        Turtle3D T3(pos,dir,up);//not flipping!!!
        
    TurtleGL T;
    T.turtle().pos = pos;
    //T.turtle().dir = dir;
    //T.turtle().turn(90);
    //Text_Turtle textTurtle(std::cout);
*/
    glClearColor(0.f,0.f,0.f,1.f);

    GLuint programID = LoadShaders("vertShader.glsl","fragShader.glsl");
    GLuint MVPID = glGetUniformLocation(programID, "MVP");
    glUseProgram(programID);
    
    //we want to look down
    //WHAT 
    CameraGL camera;
//    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f,25.f,1.f), glm::vec3(0.f,25.f,0.f), glm::vec3(0.f, 1.f,0.f));
//glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f,0.f,1.f), glm::vec3(0.f,0.f,0.f), glm::vec3(0.f, 1.f,0.f));
    //glm::mat4 projMat = glm::ortho(-30.f, 30.f, -30.f, 30.f, -30.f, 30.f);
//    glm::mat4 projMat = glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
    glm::mat4 projMat = glm::perspective(glm::radians(90.f),16.f/9.f,0.1f,5.f);
    glm::mat4 MVPMat = projMat *camera.viewMat;//* viewMat;
    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
    //MVPMat = glm::scale(glm::mat4(), glm::vec3(0.1f,0.1f,0.1f));

    

    sf::Event event;
    srand(time(NULL));

    //CREATE THE SCENE
    
    std::vector<TurtleGL*> turtles;//(2,nullptr);
    //TurtleGL *turtles[2];
    for(uint i = 0; i < 31; ++i)
    {
        //on the xz plane from x -1 - 1
        //z -1 - 1
        //float x = 0;
//        float y = -3.f;
        //float z = -0.4f * i;
        float x = 6.f - 6.f*float(rand()%1000)/500.f;
        float z = 6.f - 6.f*float(rand()%1000)/500.f;
        float y = 0.1f - 0.1f*float(rand()%1000)/500.f;
        glm::vec3 pos(x,y,z);
        glm::vec3 dir(0.f,1.f,0.f);
        glm::vec3  up(0.f,0.f,1.f);
        Turtle3D T3(pos,dir,up);
        TurtleGL *T = new TurtleGL(T3);
        turtles.push_back(T);
//        turtles[i]=T;//.push_back(T);
    }//why only 4???
    for(uint j = 0 ; j < turtles.size(); ++j)
    {
        int it = rand()%40;
        TurtleGL *T = turtles[j];
        //T->init();
        L->reset();
        for(int i =0; i<40; ++i) L->iterate();
        L->feedTurtle(*T);
        T->setupGL();
    }
    
   /*
    for(int i =0; i<28; ++i) L->iterate();
//move down 5
    //T.init();
    L->feedTurtle(T);
    T.setupGL();
    */

    SkyBox skybox;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
//I WANT TO ROTATE AROUND NOW!
    sf::Clock clock;
    float dt;
    float curAngle = 0.f;
    float dist = 1.f;
    float speed = 1.f;

    float angleSpeed = 45.f;
    sf::Clock fpsClock;
    int frameCtr = 0;

    //srand(time(NULL));

    //vary tropism as a f(t)
    float tropX, tropZ;//-5
    float totalT = 0;

    while (window.isOpen())
    {
        totalT += dt;
        tropX = sin(totalT);
        tropZ = cos(totalT);
        for(auto T : turtles)
        {//dynamic tropism hit?
            glm::vec3 trop(tropX, 2, tropZ);
            trop = glm::normalize(trop);
            T->m_tropismVector = trop;
            T->init();
            L->feedTurtle(*T);
            T->setupGL();
        }

        frameCtr++;
        glClear(GL_COLOR_BUFFER_BIT);

        MVPMat = glm::mat4(1);
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        skybox.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
        MVPMat = projMat * camera.viewMat;
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        for(auto T : turtles) T->draw();
        //turtles[0]->draw();
        //turtles[1]->draw();
        //T.draw();
        window.display();
        

        while(window.pollEvent(event))
        {
        if(event.type == sf::Event::Closed)
            window.close();
        
        if(event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Space:
                {
/*                    L->iterate();
                    T.init();
                    L->feedTurtle(T);
                    //L->feedTurtle(textTurtle);
                    //textTurtle.draw();
                    T.setupGL();*/
                }
                break;
            case sf::Keyboard::Escape:
                window.close();
                continue;
                break;
            /*
            case sf::Keyboard::Q:
                {
                    camera.orientationTurtle.pos.y += 0.1f;
                    //camera.orientationTurtle.pitch(angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::E:
                {
                    camera.orientationTurtle.pos.y -= 0.1f;
                    //camera.orientationTurtle.pitch(-angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::W:
                {
                    camera.orientationTurtle.advance(0.1f);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::S:
                {
                    camera.orientationTurtle.advance(-0.1f);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
                //left right
            case sf::Keyboard::A:
                {//strafe left
                    auto left = glm::cross(camera.orientationTurtle.up, camera.orientationTurtle.dir);
                    camera.orientationTurtle.pos += 0.1f * left;
                    //camera.orientationTurtle.turn(-angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::D:
                {
                    auto left = glm::cross(camera.orientationTurtle.up, camera.orientationTurtle.dir);
                    camera.orientationTurtle.pos -= 0.1f * left;
                    //camera.orientationTurtle.turn(angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::Left:
                {
                    camera.orientationTurtle.turn(-angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::Right:
                {
                    camera.orientationTurtle.turn(angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::Up:
                {
                    camera.orientationTurtle.pitch(angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            case sf::Keyboard::Down:
                {
                    camera.orientationTurtle.pitch(-angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
                break;
            */
            default:
                break;
            }
        }
        }
        dt = clock.restart().asSeconds();
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    camera.orientationTurtle.pos.y += speed * dt;
                    //camera.orientationTurtle.pitch(angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    camera.orientationTurtle.pos.y -= speed *dt;
                    //camera.orientationTurtle.pitch(-angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    camera.orientationTurtle.advance(speed*dt);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    camera.orientationTurtle.advance(-speed*dt);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                {//strafe left
                    auto left = glm::cross(camera.orientationTurtle.up, camera.orientationTurtle.dir);
                    camera.orientationTurtle.pos += speed * left *dt;
                    //camera.orientationTurtle.turn(-angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                {
                    auto left = glm::cross(camera.orientationTurtle.up, camera.orientationTurtle.dir);
                    camera.orientationTurtle.pos -= speed * left*dt;
                    //camera.orientationTurtle.turn(angleSpeed);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    camera.orientationTurtle.turn(-angleSpeed*dt);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    camera.orientationTurtle.turn(angleSpeed*dt);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    camera.orientationTurtle.pitch(angleSpeed*dt);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    camera.orientationTurtle.pitch(-angleSpeed*dt);
                    camera.update();
                    MVPMat = projMat * camera.viewMat;
                    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
                }

/*
        auto turnRight = [&]()->void
        {
            curAngle -= 5.f;
            viewMat = glm::lookAt(glm::vec3(dist*sinf(glm::radians(curAngle)),25.f,dist*cosf(glm::radians(curAngle))), glm::vec3(0.f,25.f,0.f), glm::vec3(0.f, 1.f,0.f));
            glm::mat4 MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        };
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
        {
            dist -= 1.f;
            viewMat = glm::lookAt(glm::vec3(dist*sinf(glm::radians(curAngle)),25.f,dist*cosf(glm::radians(curAngle))), glm::vec3(0.f,25.f,0.f), glm::vec3(0.f, 1.f,0.f));
            glm::mat4 MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        }
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
        {
            dist += 1.f;
            viewMat = glm::lookAt(glm::vec3(dist*sinf(glm::radians(curAngle)),25.f,dist*cosf(glm::radians(curAngle))), glm::vec3(0.f,25.f,0.f), glm::vec3(0.f, 1.f,0.f));
            glm::mat4 MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        }
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
        {//x z rotation
            curAngle += 10.f;
            viewMat = glm::lookAt(glm::vec3(dist*sinf(glm::radians(curAngle)),25.f,dist*cosf(glm::radians(curAngle))), glm::vec3(0.f,25.f,0.f), glm::vec3(0.f, 1.f,0.f));
            glm::mat4 MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        }
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
        {
            curAngle -= 10.f;
            viewMat = glm::lookAt(glm::vec3(dist*sinf(glm::radians(curAngle)),25.f,dist*cosf(glm::radians(curAngle))), glm::vec3(0.f,25.f,0.f), glm::vec3(0.f, 1.f,0.f));
            glm::mat4 MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        }*/
        /*
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
        {
            auto print = [&](int n)->void
            {
                sf::RenderTexture tex;
            tex.create(960,960,settings);
            tex.setActive(true);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            MVPMat = glm::mat4(1);
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        //skybox.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
            projMat = glm::ortho(-30.f, 0.f, -30.f, 0.f, -30.f, 30.f);
            MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
            T.draw();
            tex.display();
            sf::Image I1 = tex.getTexture().copyToImage();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            MVPMat = glm::mat4(1);
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        //skybox.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
            projMat = glm::ortho(-30.f, 0.f, 0.f, 30.f, -30.f, 30.f);
            MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
            T.draw();
            tex.display();
            sf::Image I2 = tex.getTexture().copyToImage();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            MVPMat = glm::mat4(1);
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        //skybox.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
            projMat = glm::ortho(0.f, 30.f, 0.f, 30.f, -30.f, 30.f);
            MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
            T.draw();
            tex.display();
            sf::Image I3 = tex.getTexture().copyToImage();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            MVPMat = glm::mat4(1);
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        //skybox.draw();
        glClear(GL_DEPTH_BUFFER_BIT);
            projMat = glm::ortho(0.f, 30.f, -30.f, 0.f, -30.f, 30.f);
            MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
            T.draw();
            tex.display();
            sf::Image I4 = tex.getTexture().copyToImage();

            window.setActive(true);
            projMat = glm::ortho(-30.f, 30.f, -30.f, 30.f, -30.f, 30.f);
            MVPMat = projMat * viewMat;
            glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);

            sf::Image I;
            I.create(960*2,960*2);
            for(uint i = 0; i < 960; i++)
                for(uint j = 0; j < 960; j++)
                    I.setPixel(i,j,I2.getPixel(i,j));
            
            for(uint i = 0; i < 960; i++)
                for(uint j = 0; j < 960; j++)
                    I.setPixel(i+960,j,I3.getPixel(i,j));
            
            for(uint i = 0; i < 960; i++)
                for(uint j = 0; j < 960; j++)
                    I.setPixel(i,j+960,I1.getPixel(i,j));
            
            for(uint i = 0; i < 960; i++)
                for(uint j = 0; j < 960; j++)
                    I.setPixel(i+960,j+960,I4.getPixel(i,j));
            
            I.saveToFile("Out"+std::to_string(n)+".png");
            };
            for(int i = 0; i < 72; ++i)
            {
                print(i);
                turnRight();
            }
            //print(0);
        }*/
    }

    float fpsTime = fpsClock.getElapsedTime().asSeconds();
    float fps = float(frameCtr)/fpsTime;
    std::cout<<"Avg fps: "<<fps<<std::endl;
    
    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<float> elapsed = end - start;
    //std::cout<<"Elapsed "<<elapsed.count() <<" secs\n";

    delete L;

    return 0;

}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		//getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}