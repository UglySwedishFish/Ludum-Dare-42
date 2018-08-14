#pragma once

#include <glm\common.hpp>
#include <glm\glm.hpp>
#include <glm\mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using Vector2f = glm::vec2;
using Vector2i = glm::ivec2;
using Vector2u = glm::uvec2;
using Vector2d = glm::dvec2;

using Vector3f = glm::vec3;
using Vector3i = glm::ivec3;
using Vector3u = glm::uvec3;
using Vector3d = glm::dvec3;

using Vector4f = glm::vec4;
using Vector4i = glm::ivec4;
using Vector4u = glm::uvec4;
using Vector4d = glm::dvec4;

using Matrix2f = glm::mat2;
using Matrix2d = glm::dmat2;

using Matrix3f = glm::mat3;
using Matrix3d = glm::dmat3;

using Matrix4f = glm::mat4;
using Matrix4d = glm::dmat4;

#include <SFML/Graphics.hpp>

class Window {
	Vector2i Resolution; 
	bool Fullscreen; 
	sf::Window * RawWindow; 
	const char * Title; 
	float FrameTime; 
	float TimeOpened; 
	int FrameCount; 
	public: 
	void SetResolution(Vector2i Resolution); 
	Vector2i GetResolution(); 

	void SetFullscreen(bool Fullscreen); 
	bool GetFullscreen(); 

	void SetTitle(const char * Title); 
	const char * GetTitle(); 

	void SetFrameTime(float FrameTime);
	float GetFrameTime(); 

	void SetTimeOpened(float TimeOpened); 
	float GetTimeOpened();

	void SetFrameCount(int FrameCount); 
	int GetFrameCount(); 

	void CursorVisible(bool Visible); 

	sf::Window * GetRawWindow(); 

	inline Window() : Resolution(0), Fullscreen(false), RawWindow(nullptr), Title("LDEngine²"), FrameCount(0) {}
	Window(Vector2i Resolution, bool Fullscreen); 
	~Window(); 
}; 