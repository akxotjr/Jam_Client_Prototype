#include "pch.h"
#include "Renderer.h"
#include "SessionManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameScene.h"
#include "GameTcpSession.h"
#include "Player.h"
#include "ServerPacketHandler.h"


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


    // ImGui init
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



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

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    InitGrid();
    InitCube();
    InitPlane();
    InitCrosshair();
}

void Renderer::Shutdown()
{
    if (_gridInitialized)
    {
        glDeleteVertexArrays(1, &_gridVAO);
        glDeleteBuffers(1, &_gridVBO);
    }

    if (_cubeInitialized)
    {
        glDeleteVertexArrays(1, &_cubeVAO);
        glDeleteBuffers(1, &_cubeVBO);
        glDeleteBuffers(1, &_cubeEBO);
    }

    if (_planeInitialized)
    {
        glDeleteVertexArrays(1, &_planeVAO);
        glDeleteBuffers(1, &_planeVBO);
        glDeleteBuffers(1, &_planeEBO);
    }

    if (_rayInitialized)
    {
        glDeleteVertexArrays(1, &_rayVAO);
        glDeleteBuffers(1, &_rayVBO);
    }

    if (_crosshairInitialized)
    {
        glDeleteVertexArrays(1, &_crossVAO);
        glDeleteBuffers(1, &_crossVBO);
    }

    glDeleteProgram(_shaderProgram);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (_window)
        glfwDestroyWindow(_window);

    glfwTerminate();
}


void Renderer::PreRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::PostRender()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void Renderer::DrawGrid(const glm::vec4& color)
{
    if (!_gridInitialized)
        InitGrid();

    glUseProgram(_shaderProgram);
    glUniform4f(_shaderLocColor, color.r, color.g, color.b, color.a);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = _proj * _view * model;
    glUniformMatrix4fv(_shaderLocMVP, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(_gridVAO);
    glDrawArrays(GL_LINES, 0, _gridVertexCount);
    glBindVertexArray(0);
}

void Renderer::DrawPlane(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color)
{
    if (!_planeInitialized)
        InitPlane();

    glUseProgram(_shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale.x, 1.0f, scale.y));

    glm::mat4 mvp = _proj * _view * model;

    glUniformMatrix4fv(_shaderLocMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4f(_shaderLocColor, color.r, color.g, color.b, color.a);

    glBindVertexArray(_planeVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void Renderer::DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color)
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
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    glm::mat4 mvp = _proj * _view * model;

    glUniformMatrix4fv(_shaderLocMVP, 1, GL_FALSE, glm::value_ptr(mvp));

    glUniform4f(_shaderLocColor, color.r, color.g, color.b, color.a);

    glBindVertexArray(_cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    //GLenum err;
    //while ((err = glGetError()) != GL_NO_ERROR)
    //{
    //    std::cout << "OpenGL Error: " << err << '\n';
    //}
}

void Renderer::DrawRay(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
    if (!_rayInitialized)
        InitRay();

    glm::vec3 rayVertices[2] = { start, end };

    glBindBuffer(GL_ARRAY_BUFFER, _rayVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rayVertices), rayVertices);

    glUseProgram(_shaderProgram);
    glUniform4f(_shaderLocColor, color.r, color.g, color.b, color.a);

    glm::mat4 mvp = _proj * _view * glm::mat4(1.0f);
    glUniformMatrix4fv(_shaderLocMVP, 1, GL_FALSE, glm::value_ptr(mvp));

    glLineWidth(2.0f);
    glBindVertexArray(_rayVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

void Renderer::DrawDebugObject()
{
    WRITE_LOCK
    std::erase_if(_debugDrawObjects, [](const DebugDrawObject& obj) {return obj.expireTime <= TimeManager::Instance().GetClientTime(); });

    for (auto& obj : _debugDrawObjects)
    {
	    switch (obj.type)
	    {
        case DebugDrawObject::Type::Ray:
            DrawRay(obj.ray.start, obj.ray.end, obj.color);
            break;
	    case DebugDrawObject::Type::Cube:
            DrawCube(obj.cube.position, obj.cube.rotation, obj.cube.scale, obj.color);
            break;
	    }
    }
}

void Renderer::DrawUI()
{
    DrawCrosshair();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawDebugginUI();
    DrawRoomUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::DrawDebugginUI()
{
    ImGui::Begin("Debug Information");

    double clientTime = TimeManager::Instance().GetClientTime();
    ImGui::Text("Client Time : %lf", clientTime);

    double avgRtt = TimeManager::Instance().GetAvgRTT();
    ImGui::Text("AvgRTT : %lf", avgRtt);

    double jitter = TimeManager::Instance().GetJitter();
    ImGui::Text("Jitter : %lf", jitter);

    uint32 fps = TimeManager::Instance().GetFps();
    ImGui::Text("fps : %u", fps);

    uint32 userId = SessionManager::Instance().GetUserId();
    ImGui::Text("User ID : %u", userId);

    auto gameScene = static_pointer_cast<GameScene>(SceneManager::Instance().GetCurrentScene());
    if (auto player = gameScene->GetPlayer())
    {
        Vec3 position = player->GetPosition();
        ImGui::Text("Position (%.5f, %.5f, %.5f)", position.x, position.y, position.z);
    }

    if (ImGui::Button("Exit"))
    {
        running = false;
    }


    ImGui::End();
}

void Renderer::DrawRoomUI()
{
    ImGui::Begin("Room Menu");

    bool isInRoom = SceneManager::Instance().GetIsInRoom();
    bool isInGame = SceneManager::Instance().GetIsInGame();
    uint32 myRoomId = SceneManager::Instance().GetRoomId();

    if (!isInRoom)
    {
        if (ImGui::Button("Room Create"))
        {
            Protocol::C_CREATE_ROOM createRoomPkt;

            auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(createRoomPkt);
            auto session = SessionManager::Instance().GetTcpSession();

            session->Send(sendBuffer);
            std::cout << "[TCP] Send : C_CREATE_ROOM\n";
        }
    }
    else
    {
        ImGui::Text("You are in Room #%u", myRoomId);
    }

    ImGui::Separator();

    if (isInRoom)
    {
	    if (!isInGame)
	    {
            if (ImGui::Button("Game Start"))
            {
                Protocol::C_ENTER_GAME enterGamePkt;
                auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(enterGamePkt);
                auto session = SessionManager::Instance().GetTcpSession();
                session->Send(sendBuffer);

                std::cout << "[TCP] Send : C_ENTER_GAME\n";

                SceneManager::Instance().SetIsInGame(true);
            }
	    }
        else
        {
            ImGui::BeginChild("PlayerList", ImVec2(150, 150), true);

            const auto& playerList = SceneManager::Instance().GetPlayerList();

            for (auto& userId : playerList)
            {
                ImGui::Text("player id : %d", userId);
            }

            ImGui::EndChild();
        }
    }
    else
    {
        ImGui::BeginChild("RoomList", ImVec2(150, 150), true);
        const auto& roomList = SceneManager::Instance().GetRoomList();

        for (const auto& [roomId, playerList] : roomList)
        {
            ImGui::Text("Room %d (%d/%d)", roomId, playerList.size(), 4);
            ImGui::SameLine();
            if (ImGui::Button(("Enter##" + std::to_string(roomId)).c_str()))
            {
                Protocol::C_ENTER_ROOM enterRoomPkt;
                enterRoomPkt.set_roomid(roomId);
                auto sendBuffer = ServerPacketHandler::MakeSendBufferTcp(enterRoomPkt);
                auto session = SessionManager::Instance().GetTcpSession();

                session->Send(sendBuffer);

                std::cout << "[TCP] Send : C_ENTER_ROOM\n";
            }
        }
        ImGui::EndChild();
    }

    ImGui::End();
}

void Renderer::DrawCrosshair()
{
    bool isInGame = SceneManager::Instance().GetIsInGame();
    if (!isInGame) return;

    glDisable(GL_DEPTH_TEST);
    glUseProgram(_shaderProgram); 

    glm::mat4 mvp = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));

    glUniform4f(glGetUniformLocation(_shaderProgram, "uColor"), 0.0f, 0.0f, 0.0f, 1.0f); 
    glLineWidth(3.0f);

    glBindVertexArray(_crossVAO);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::UpdateCamera(const Vec3& playerPos, float yaw, float pitch, GLfloat cameraDist)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, pitch, glm::vec3(1, 0, 0));
    model = glm::rotate(model, yaw, glm::vec3(0, 1, 0));

    glm::vec4 rotatedCameraOffset = model * _cameraOffset * cameraDist;

    glm::vec3 cameraPos = glm::vec3(playerPos.x, playerPos.y, playerPos.z) + glm::vec3(rotatedCameraOffset);

    glm::vec3 targetPos = cameraPos + glm::vec3(cosf(pitch) * sinf(yaw), sin(pitch), cos(pitch) * cos(yaw));
	_view = glm::lookAt(cameraPos, targetPos, _cameraUp);
	_proj = glm::perspective(glm::radians(45.0f), WINDOW_SIZE_X / WINDOW_SIZE_Y, 0.1f, 100.0f);
}


void Renderer::AddDebugDrawObject(const DebugDrawObject& obj)
{
    WRITE_LOCK
    _debugDrawObjects.push_back(obj);
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

void Renderer::InitGrid()
{
    if (_gridInitialized)
        return;

    std::vector<glm::vec3> lines;


    for (int z = WORLD_RANGE_MIN; z <= WORLD_RANGE_MAX; z += WORLD_GRID_SIZE)
    {
        lines.emplace_back(WORLD_RANGE_MIN, 0.05f, z);
        lines.emplace_back(WORLD_RANGE_MAX, 0.05f, z);
    }

    for (int x = WORLD_RANGE_MIN; x <= WORLD_RANGE_MAX; x += WORLD_GRID_SIZE)
    {
        lines.emplace_back(x, 0.05f, WORLD_RANGE_MIN);
        lines.emplace_back(x, 0.05f, WORLD_RANGE_MAX);
    }

    glGenVertexArrays(1, &_gridVAO);
    glGenBuffers(1, &_gridVBO);

    glBindVertexArray(_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _gridVBO);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(glm::vec3), lines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    _gridVertexCount = static_cast<GLsizei>(lines.size());

    _gridInitialized = true;
}

void Renderer::InitPlane()
{
    if (_planeInitialized)
        return;

    float planeVertices[] = {
        // positions         // normals
        -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &_planeVAO);
    glGenBuffers(1, &_planeVBO);
    glGenBuffers(1, &_planeEBO);

    glBindVertexArray(_planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    _planeInitialized = true;
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

void Renderer::InitRay()
{
    if (_rayInitialized)
        return;

    glm::vec3 rayVertices[2] = {
        glm::vec3(0.0f, 0.0f, 0.0f),    // start
        glm::vec3(0.0f, 1.0f, 0.0f)     // end
    };

    glGenVertexArrays(1, &_rayVAO);
    glGenBuffers(1, &_rayVBO);

    glBindVertexArray(_rayVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _rayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rayVertices), rayVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    _rayInitialized = true;
}

void Renderer::InitCrosshair()
{
    if (_crosshairInitialized)
        return;

    float crosshairVertices[] = {
        // vertical
         0.0f, -0.05f, 0.0f,
         0.0f,  0.05f, 0.0f,

         // horizon
         -0.05f,  0.0f, 0.0f,
          0.05f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &_crossVAO);
    glGenBuffers(1, &_crossVBO);

    glBindVertexArray(_crossVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _crossVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    _crosshairInitialized = true;
}



