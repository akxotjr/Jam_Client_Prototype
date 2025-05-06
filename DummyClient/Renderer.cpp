#include "pch.h"
#include "Renderer.h"

void Renderer::Init()
{
	_vertexShaderSource =
		R"(
			#version 330 core
			layout(location = 0) in vec3 aPos;       

			uniform mat4 uMVP;                       

			void main()
			{
			    gl_Position = uMVP * vec4(aPos, 1.0); 
			}
		)";

	_fragmentShaderSource =
		R"(
			#version 330 core
			out vec4 FragColor;
			uniform vec4 uColor;                    

			void main()
			{
			    FragColor = uColor; 
			}
		)";


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);        
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    _window = glfwCreateWindow(800, 600, "Cube View", nullptr, nullptr);
    if (!_window) {
        std::cout << "[Rendering] Failed to create window";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(_window);
    glfwSetFramebufferSizeCallback(_window, FramebufferSizeCallbackStatic);
    glfwSetWindowUserPointer(_window, this);


    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "[Rendering] Failed to initialize GLAD\n";
        return;
    }

    glEnable(GL_DEPTH_TEST); 

    GLuint vs = CompileShader(GL_VERTEX_SHADER, _vertexShaderSource);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, _fragmentShaderSource);

    _shaderProgram = glCreateProgram();            
    glAttachShader(_shaderProgram, vs);
    glAttachShader(_shaderProgram, fs);
    glLinkProgram(_shaderProgram);
    glDeleteShader(vs);
	glDeleteShader(fs);

    _shaderLocMVP = glGetUniformLocation(_shaderProgram, "uMVP");
    _shaderLocColor = glGetUniformLocation(_shaderProgram, "uColor");

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    InitCube();
}

void Renderer::Shutdown()
{
    glDeleteVertexArrays(1, &_cubeVAO);
    glDeleteBuffers(1, &_cubeVBO);
    glDeleteBuffers(1, &_cubeEBO);
    glDeleteProgram(_shaderProgram);

    if (_window)
        glfwDestroyWindow(_window);

    glfwTerminate();
}

//void Renderer::Render()
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    UpdateCamera(Vec3(0, 0, 0), Vec3(0, 0, -1), 5.f);
//    DrawCube(Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(1, 1, 1), glm::vec4(1, 0, 0, 1));
//
//    glfwSwapBuffers(_window);
//    glfwPollEvents();
//}

void Renderer::PreRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::PostRender()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void Renderer::DrawCube(const Vec3& position, const Vec3& rotation, const Vec3& size, const glm::vec4& color)
{
    int success;
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(_shaderProgram, 512, nullptr, info);
        std::cout << "[Rendering] Shader Link Error: " << info << '\n';
        return;
    }


    glUseProgram(_shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(size.x, size.y, size.z));

    glm::mat4 mvp = _proj * _view * model;

    glUniformMatrix4fv(_shaderLocMVP, 1, GL_FALSE, glm::value_ptr(mvp));


    glUniform4f(_shaderLocColor, color.x, color.y, color.z, color.w);

    glBindVertexArray(_cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << err << '\n';
    }
}

void Renderer::UpdateCamera(const Vec3& playerPos, const Vec3& playerDir, GLfloat cameraDist)
{
    // playerDir = 플레이어가 바라보는 방향

    // todo
    //glm::vec3 directionXZ = glm::normalize(glm::vec3(playerDir.x, 0, -playerDir.z));
    //glm::vec3 cameraOffset = glm::normalize(directionXZ + glm::vec3(0, heightRatio, 0)) * cameraDist;


	glm::vec3 cameraOffset = glm::normalize(glm::vec3(-playerDir.x, 1, -playerDir.z)) * cameraDist;
    glm::vec3 cameraPos = glm::vec3(playerPos.x, playerPos.y, playerPos.z) + cameraOffset;

    _view = glm::lookAt(cameraPos, glm::vec3(playerPos.x, playerPos.y, playerPos.z), _cameraUp);
    _proj = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);
}


void Renderer::FramebufferSizeCallbackStatic(GLFWwindow* window, int width, int height)
{
	if (Renderer* self = static_cast<Renderer*>(glfwGetWindowUserPointer(window)))
        self->FramebufferSizeCallback(window, width, height);
}

void Renderer::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);     
}

GLuint Renderer::CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);                          
    glShaderSource(shader, 1, &source, nullptr);      
    glCompileShader(shader);                                      

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info); 
        std::cout << "[Rendering] Shader Compile Error:\n" << info << '\n';
    }
	
    return shader;
}

void Renderer::InitCube()
{
    if (_cubeInitialized)
        return;

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, // 0
         0.5f, -0.5f, -0.5f, // 1
         0.5f,  0.5f, -0.5f, // 2
        -0.5f,  0.5f, -0.5f, // 3
        -0.5f, -0.5f,  0.5f, // 4
         0.5f, -0.5f,  0.5f, // 5
         0.5f,  0.5f,  0.5f, // 6
        -0.5f,  0.5f,  0.5f  // 7
    };

    unsigned int indices[] = {
        4, 5, 6, 6, 7, 4,  // front
        0, 3, 2, 2, 1, 0,  // back
        0, 4, 7, 7, 3, 0,  // left
        1, 2, 6, 6, 5, 1,  // right
        3, 7, 6, 6, 2, 3,  // top
        0, 1, 5, 5, 4, 0   // bottom
    };

    glGenVertexArrays(1, &_cubeVAO);
    glGenBuffers(1, &_cubeVBO);
    glGenBuffers(1, &_cubeEBO);

    glBindVertexArray(_cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // vertex attribute: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    _cubeInitialized = true;
}



