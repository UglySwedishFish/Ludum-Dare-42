#include "Core.h"
#include <iostream> 


template<typename T>
T LDEngine::Core::Interpolate(T A, T B, T X)
{
	T FT = x * static_cast<T>(3.1415);
}





template<typename T>
T LDEngine::Core::Lerp(T A, T B, T F)
{
	return A + F * (B - A); 
}

Matrix4f LDEngine::Core::ViewMatrix(Vector3f Position, Vector3f Rotation)
{
	Matrix4f Temp;
	Temp = glm::rotate(Temp, glm::radians(Rotation.x), { 1, 0, 0 });
	Temp = glm::rotate(Temp, glm::radians(Rotation.y), { 0, 1, 0 });
	Temp = glm::rotate(Temp, glm::radians(Rotation.z), { 0, 0, 1 });

	Temp = glm::translate(Temp, Vector3f(-Position.x, -Position.y, -Position.z));

	return Temp;
}

Matrix4f LDEngine::Core::ModelMatrix(Vector3f Position, Vector3f Rotation)
{
	Matrix4f Temp;

	Temp = glm::translate(Temp, Vector3f(Position.x, Position.y, Position.z));

	Temp = glm::rotate(Temp, glm::radians(Rotation.x), { 1, 0, 0 });
	Temp = glm::rotate(Temp, glm::radians(Rotation.y), { 0, 1, 0 });
	Temp = glm::rotate(Temp, glm::radians(Rotation.z), { 0, 0, 1 });


	return Temp;
}

Matrix4f LDEngine::Core::ShadowOrthoMatrix(float edge, float znear, float zfar)
{
	return glm::ortho(-edge, edge, -edge, edge, znear, zfar);
}

float Seed = 0.f; 

float randf() {	return glm::fract(sin(Seed += .1)*43758.5453123);}

Vector3f LDEngine::Core::RandUnit()
{
	return glm::normalize(Vector3f(randf() * 2. - 1., randf() * 2. - 1., randf() * 2. - 1.));
}
