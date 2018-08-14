#include "wraptobaketexture.h"
#include <SFML/Graphics.hpp>
#include <iostream>

struct TriangleData {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
	glm::vec3 Tangent;

	TriangleData(glm::vec3 Position = glm::vec3(0.), glm::vec3 Normal = glm::vec3(0.), glm::vec3 Tangent = glm::vec3(0.), glm::vec2 TexCoord = glm::vec2(0.), int id = 0) :
		Position(Position),
		Normal(Normal),
		Tangent(Tangent),
		TexCoord(TexCoord)
	{}
	TriangleData operator*(float f) {
		Position *= f;
		Normal *= f;
		TexCoord *= f;
		Tangent *= f;
		return *this;
	}
	TriangleData operator+(TriangleData data) {
		Position += data.Position;
		Normal += data.Normal;
		TexCoord += data.TexCoord;
		Tangent += data.Tangent;
		return *this;
	}
};

TriangleData BaryCentric(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 pos, TriangleData vert1, TriangleData vert2, TriangleData vert3) {
	float det = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
	float l1 = ((p2.y - p3.y) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.y)) / det;
	float l2 = ((p3.y - p1.y) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.y)) / det;
	float l3 = 1.0f - l1 - l2;
	return vert1 * l1 + vert2 * l2 + vert3 * l3;
}

float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle(Vector2f pt, Vector2f v1, Vector2f v2, Vector2f v3)
{
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}

LDEngine::Rendering::Mesh::LightMapData LDEngine::Rendering::Mesh::ProjectToBakeTexture(Model & ModelData, unsigned int Resolution)
{
	LightMapData ReturnData; 
	ReturnData.Vertices = std::vector<Vector3f>(Resolution*Resolution); 
	ReturnData.Normals = std::vector<Vector3f>(Resolution*Resolution);
	ReturnData.Tangents = std::vector<Vector3f>(Resolution*Resolution);
	ReturnData.TexCoords = std::vector<Vector3f>(Resolution*Resolution);
	ReturnData.TriangleID = std::vector<Vector3f>(Resolution*Resolution);
	ReturnData.Colors = std::vector<Vector3f>(Resolution*Resolution);

	std::vector<Vector3f> ModelUVS = ModelData.ModelData.TextureCoordinates;

	for (int UV = 0; UV < ModelUVS.size(); UV+=3) {
		float MaxX = LDEngine::Core::max3(ModelUVS[UV].x, ModelUVS[UV + 1].x, ModelUVS[UV + 2].x); 
		float MaxY = LDEngine::Core::max3(ModelUVS[UV].y, ModelUVS[UV + 1].y, ModelUVS[UV + 2].y);
		float MinX = LDEngine::Core::min3(ModelUVS[UV].x, ModelUVS[UV + 1].x, ModelUVS[UV + 2].x);
		float MinY = LDEngine::Core::min3(ModelUVS[UV].y, ModelUVS[UV + 1].y, ModelUVS[UV + 2].y);

		int MaxXTexel = ceil(MaxX*float(Resolution) + 0.5);
		int MaxYTexel = ceil(MaxY*float(Resolution) + 0.5);
		int MinXTexel = floor(MinX*float(Resolution) - 0.5);
		int MinYTexel = floor(MinY*float(Resolution) - 0.5);

		for (int X = MinXTexel; X < MaxXTexel; X++) {
			for (int Y = MinYTexel; Y < MaxYTexel; Y++) {
				if (PointInTriangle(Vector2f(float(X) / float(Resolution), float(Y) / float(Resolution)), Vector2f(ModelUVS[UV]), Vector2f(ModelUVS[UV + 1]), Vector2f(ModelUVS[UV + 2]))) {
					//this pixel is inside of the triangle! 

					float XRelative = float(X - MinXTexel) / (MaxXTexel - MinXTexel); 
					float YRelative = float(Y - MinYTexel) / (MaxYTexel - MinYTexel);

					std::vector<Vector2f> Corners = { Vector2f(ModelUVS[UV]),Vector2f(ModelUVS[UV + 1]),Vector2f(ModelUVS[UV + 2]) }; 

					for (int Corner = 0; Corner < 3; Corner++) {
						Corners[Corner].x = (Corners[Corner].x - MinX) / (MaxX-MinX);
						Corners[Corner].y = (Corners[Corner].y - MinY) / (MaxY-MinY);
					}

					TriangleData Data = BaryCentric(
						Corners[0], Corners[1], Corners[2], Vector2f(XRelative, YRelative),
						TriangleData(ModelData.ModelData.Vertices[UV], ModelData.ModelData.Normals[UV], ModelData.ModelData.Tangents[UV], ModelData.ModelData.TextureCoordinates[UV]),
						TriangleData(ModelData.ModelData.Vertices[UV + 1], ModelData.ModelData.Normals[UV + 1], ModelData.ModelData.Tangents[UV + 1], ModelData.ModelData.TextureCoordinates[UV + 1]),
						TriangleData(ModelData.ModelData.Vertices[UV + 2], ModelData.ModelData.Normals[UV + 2], ModelData.ModelData.Tangents[UV + 2], ModelData.ModelData.TextureCoordinates[UV + 2])
					);

					const int Texel = Y * Resolution + X;
					const int Triangle = UV / 3;
					const int Material = floor(ModelData.ModelData.TextureCoordinates[Triangle].z * 100.f + .1f);

					ReturnData.Vertices[Texel] = Data.Position; 
					ReturnData.Normals[Texel] = Data.Normal;
					ReturnData.TexCoords[Texel] = Vector3f(Data.TexCoord,0.);
					ReturnData.Tangents[Texel] = Data.Tangent;
					ReturnData.TriangleID[Texel] = Vector3f(0.f);
					ReturnData.Colors[Texel] = ModelData.MaterialData.Materials[Material].AlbedoMultiplier;

				}
			}
		}
	}

	return ReturnData; 

}
