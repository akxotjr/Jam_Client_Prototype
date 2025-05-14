#pragma once
#include "TimeManager.h"

struct DebugDrawObject
{
	enum class Type { Ray, Cube } type;
	glm::vec4 color;
	double expireTime;

	union
	{
		struct { glm::vec3 start, end; } ray;
		struct { glm::vec3 position, rotation, scale; } cube;
	};

	DebugDrawObject(const glm::vec3& s, const glm::vec3& e, const glm::vec4& c, double duration)
		: type(Type::Ray), color(c), expireTime(TimeManager::Instance().GetClientTime() + duration)
	{
		ray = { s, e };
	}

	DebugDrawObject(const glm::vec3& p, const glm::vec3 r, const glm::vec3& s, const glm::vec4& c, double duration)
		: type(Type::Cube), color(c), expireTime(TimeManager::Instance().GetClientTime() + duration)
	{
		cube = { p, r, s };
	}
};


class Renderer
{
	DECLARE_SINGLETON(Renderer)

public:
	void			Init();
	void			Shutdown();

	void			PreRender();
	void			PostRender();

	void			DrawGrid(const glm::vec4& color);
	void			DrawPlane(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color);
	void			DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& color);
	void			DrawRay(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

	void			DrawDebugObject();

	void			DrawUI();
	void			DrawDebugginUI();
	void			DrawRoomUI();
	void			DrawCrosshair();

	void			UpdateCamera(const Vec3& playerPos, float yaw, float pitch, GLfloat cameraDist);

	GLFWwindow*		GetWindow() const { return _window; }

	void			AddDebugDrawObject(const DebugDrawObject& request);

private:
	static void		FramebufferSizeCallbackStatic(GLFWwindow* window, int width, int height);
	void			FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	GLuint			CompileShader(GLenum type, const char* source);

	void			InitGrid();
	void			InitPlane();
	void			InitCube();
	void			InitRay();
	void			InitCrosshair();

private:
	USE_LOCK

	const char*		_vertexShaderSource = nullptr;
	const char*		_fragmentShaderSource = nullptr;
	GLFWwindow*		_window = nullptr;

	GLuint			_shaderProgram = 0;

	GLuint			_gridVAO = 0;
	GLuint			_gridVBO = 0;
	GLsizei			_gridVertexCount = 0;
	bool			_gridInitialized = false;

	GLuint			_planeVAO = 0;
	GLuint			_planeVBO = 0;
	GLuint			_planeEBO = 0;
	bool			_planeInitialized = false;


	GLuint			_cubeVAO = 0;
	GLuint			_cubeVBO = 0;
	GLuint			_cubeEBO = 0;
	bool			_cubeInitialized = false;

	GLuint			_rayVAO = 0;
	GLuint			_rayVBO = 0;
	bool			_rayInitialized = false;

	GLuint			_crossVAO = 0;
	GLuint			_crossVBO = 0;
	bool			_crosshairInitialized = false;


	GLint			_shaderLocMVP;
	GLint			_shaderLocColor;

	glm::mat4		_view;
	glm::mat4		_proj;
	glm::vec3		_cameraUp = { 0, 1, 0 };
	glm::vec4		_cameraOffset = { -0.2f, 0.4f, -1.5f , 1.0f};
	
	Vector<DebugDrawObject> _debugDrawObjects;
};

