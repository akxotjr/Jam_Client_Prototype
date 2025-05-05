#pragma once


class Renderer
{
	DECLARE_SINGLETON(Renderer)

public:
	void			Init();
	void			Shutdown();
	void			Render();

	void			DrawCube(const Vec3& position, const Vec3& rotation, const Vec3& size, const glm::vec4& color);

	void			UpdateCamera(const Vec3& playerPos, const Vec3& playerDir, GLfloat cameraDist);

	GLFWwindow*		GetWindow() const { return _window; }

private:
	static void		FramebufferSizeCallbackStatic(GLFWwindow* window, int width, int height);
	void			FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	GLuint			CompileShader(GLenum type, const char* source);

	void			InitCube();

private:
	const char*		_vertexShaderSource = nullptr;
	const char*		_fragmentShaderSource = nullptr;
	GLFWwindow*		_window = nullptr;

	GLuint			_shaderProgram = 0;
	GLuint			_cubeVAO = 0;
	GLuint			_cubeVBO = 0;
	GLuint			_cubeEBO = 0;
	bool			_cubeInitialized = false;

	GLint			_shaderLocMVP;
	GLint			_shaderLocColor;

	glm::mat4		_view;
	glm::mat4		_proj;
	glm::vec3		_cameraUp = { 0, 1, 0 };
};

