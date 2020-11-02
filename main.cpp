#include "LSLoader.hpp"
#include "OpenGLTurtle.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>

#include "SFMLCameraController.hpp"
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

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    sf::Window window(sf::VideoMode(900,900,32), "Turtle", 7U, settings);//  7U, settings);
    //sf::Window window(sf::VideoMode(1920,1080,32), "Turtle", sf::Style::Fullscreen, settings);//  7U, settings);
    
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
     
    TurtleGL T;
    SphericalCameraController camera(window.getSize());
    
    glClearColor(0.f,0.f,0.f,1.f);

    GLuint programID = LoadShaders("vertShader.glsl","fragShader.glsl");
    GLuint MVPID = glGetUniformLocation(programID, "MVP");
    glUseProgram(programID);
    
    glm::mat4 MVPMat = camera.getVPMat();
    glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
    
    sf::Event event;
    srand(time(NULL));

    SkyBox skybox;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    sf::Clock clock;
    float dt;
    
    sf::Clock fpsClock;
    int frameCtr = 0;

    while (window.isOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        MVPMat = glm::mat4(1);
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        skybox.draw();
        glClear(GL_DEPTH_BUFFER_BIT);

        MVPMat = camera.getVPMat();
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVPMat[0][0]);
        T.draw();
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
                    L->iterate();
                    T.reset();
                    L->feed(T);
                    T.updateGLBuffers();
                }
                break;
            case sf::Keyboard::Escape:
                window.close();
                continue;
                break;
            default:
                break;
            }
        }
        }
        dt = clock.restart().asSeconds();
        camera.update(dt);
    }

    //loat fpsTime = fpsClock.getElapsedTime().asSeconds();
    //float fps = float(frameCtr)/fpsTime;
    //std::cout<<"Avg fps: "<<fps<<std::endl;

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