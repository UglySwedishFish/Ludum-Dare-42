#include <iostream>
#include "MeshTextureWrapper.h"

LDEngine::Rendering::Mesh::WrappedModel::WrappedModel()
{
}

void SetPixelByte(std::vector<GLubyte> &Vector, int Index, Vector3u Pixel) {
	Vector[Index] = Pixel.x; 
	Vector[Index + 1] = Pixel.y; 
	Vector[Index + 2] = Pixel.z; 
}

#define EmptyDataVector std::vector<Vector4f>(Resolution*Resolution)

Vector4f To3DFrom2DF(Vector3f v, float f) { return Vector4f(v.x, v.y, v.z, f); }


bool LDEngine::Rendering::Mesh::WrappedModel::Wrap(Model &WrapModel)
{

	MaterialData = WrapModel.MaterialData; 
	BVHNode * Tree = ConstructBoundingVolumeHieracy(WrapModel.ModelData); 


	const unsigned int Resolution = ceil(	sqrt(	WrapModel.ModelData.Vertices.size()		)	); 

	std::cout << WrapModel.ModelData.Vertices.size() << std::endl;
	std::cout << Resolution * Resolution << '\n'; 

	MeshDataResolution = Vector2i(Resolution, Resolution); 

	//MODEL DATA: 

	std::vector<Vector4f> ModelData[4] = { EmptyDataVector,EmptyDataVector,EmptyDataVector,EmptyDataVector };
	// Vertices, Normals, Tangents, TextureCoordinates + Material

	int CurrentIndex = 0; //current model index

	for (int i = 0; i < WrapModel.ModelData.Vertices.size(); i++) {
		ModelData[0][i] = Vector4f(WrapModel.ModelData.Vertices[i], 0.); //vertices
		ModelData[1][i] = Vector4f(WrapModel.ModelData.Normals[i], 0.); //normals
		ModelData[2][i] = Vector4f(WrapModel.ModelData.Tangents[i], 0.); //tangents 
		ModelData[3][i] = Vector4f(WrapModel.MaterialData.Materials[floor(WrapModel.ModelData.TextureCoordinates[i].z * 100.f + .1f)].AlbedoMultiplier,0.f); //texture coordinates and material id 
	}

	const int _i = 6499; 

	

	glGenTextures(1, &MeshData); //generate the texture for the mesh data 
	glBindTexture(GL_TEXTURE_2D_ARRAY, MeshData); 
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, Resolution, Resolution, 5, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (int SubTexture = 0; SubTexture < 4; SubTexture++)
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, SubTexture, Resolution, Resolution, 1, GL_RGBA, GL_FLOAT, &ModelData[SubTexture][0]); //store the mesh data
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 4, Resolution, Resolution, 1, GL_RGBA, GL_FLOAT, &StoreBVHInImage(Tree, Resolution, Resolution).ImageData[0]);
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	//MATERIAL DATA: 
	
	return true; 



}
