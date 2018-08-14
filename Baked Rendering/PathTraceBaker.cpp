#include "PathTraceBaker.h"
#include <iostream>

void SavePixelDataToImageFromFloatVector(const char *fileName, std::vector<float> data, glm::ivec2 Resolution, bool YInverted, bool Alpha) {
	sf::Image *FinalImage;
	FinalImage = new sf::Image();
	FinalImage->create(Resolution.x, Resolution.y);

	for (int x = 0; x < Resolution.x; x++) {
		for (int y = 0; y < Resolution.y; y++) {

			

			sf::Color c;

			if (!Alpha) {
				c.r = std::min(std::max(int(data[((y*Resolution.y) + x) * 3] * 255), 0), 255);
				c.g = std::min(std::max(int(data[((y*Resolution.y) + x) * 3 + 1] * 255), 0), 255);
				c.b = std::min(std::max(int(data[((y*Resolution.y) + x) * 3 + 2] * 255), 0), 255);
			}
			else {
				c.r = std::min(std::max(int(data[((y*Resolution.y) + x) * 4] * 255), 0), 255);
				c.g = std::min(std::max(int(data[((y*Resolution.y) + x) * 4 + 1] * 255), 0), 255);
				c.b = std::min(std::max(int(data[((y*Resolution.y) + x) * 4 + 2] * 255), 0), 255);
				c.a = std::min(std::max(int(data[((y*Resolution.y) + x) * 4 + 3] * 255), 0), 255);
			}

			FinalImage->setPixel(x, YInverted ? y : (Resolution.y - 1) - y, c);
		}
	}

	FinalImage->saveToFile(fileName);
	delete FinalImage;
}

void LDEngine::Rendering::PathTracing::PathTracingPipeLine::PrepareLightBaker(unsigned int Resolution)
{
	this->Resolution = Resolution; 
	Core::PreparePostProcess(); 
	LightBakerFrameBuffer[0] = Core::FrameBufferObject(Vector2i(Resolution), false); 
	LightBakerFrameBuffer[1] = Core::FrameBufferObject(Vector2i(Resolution), false);
	EdgeFixer = Core::Shader("Shaders/EdgeFixer/vert.glsl", "Shaders/EdgeFixer/frag.glsl");
	ShadowStructureShader = Core::Shader("Shaders/ShadowPass/vert.glsl", "Shaders/ShadowPass/frag.glsl");
	LightBakerProgressive = Core::Shader("Shaders/ProgressivePathTracer/vert.glsl", "Shaders/ProgressivePathTracer/frag.glsl");
}

void LDEngine::Rendering::PathTracing::PathTracingPipeLine::BakeLighting(Window & Window, Mesh::Model & WrapModel, std::vector<Mesh::Model>& Models, const char * OutPutFile, int Samples, int BlurStrength, Vector2f ShadowDirection, bool UseDirectionalShadows, Vector3f SkyBaseColor, Vector3f SkyHorizonColor)
{
	Control::Camera Null;

	ShadowStructure Shadows; 
	Shadows.PrepareShadowStructure(Vector2i(1024)); 
	Shadows.UpdateRotation(ShadowDirection, Null); 

	glEnable(GL_DEPTH_TEST);

	Shadows.ShadowMaps[0].Bind();

	ShadowStructureShader.Bind();
	
	glUniformMatrix4fv(glGetUniformLocation(ShadowStructureShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(ShadowStructureShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));

	for (auto & Model : Models)
		Mesh::DrawModel(Model, ShadowStructureShader, Null, Window);

	ShadowStructureShader.UnBind();

	Shadows.ShadowMaps[0].UnBind(Window);

	glFinish(); 

	//Project to light map data
	Mesh::LightMapData LightMapData = Mesh::ProjectToBakeTexture(WrapModel, Resolution); 

	//Store this in a 2D-Array-Texture

	unsigned int BakeDataTextureID;
	glGenTextures(1, &BakeDataTextureID);

	glBindTexture(GL_TEXTURE_2D_ARRAY, BakeDataTextureID);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB32F, Resolution, Resolution, 6, 0, GL_RGB, GL_FLOAT, NULL);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 0,
		Resolution, Resolution, 1,
		GL_RGB,
		GL_FLOAT,
		&LightMapData.Vertices[0]);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 1,
		Resolution, Resolution, 1,
		GL_RGB,
		GL_FLOAT,
		&LightMapData.Normals[0]);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 2,
		Resolution, Resolution, 1,
		GL_RGB,
		GL_FLOAT,
		&LightMapData.Tangents[0]);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 3,
		Resolution, Resolution, 1,
		GL_RGB,
		GL_FLOAT,
		&LightMapData.TexCoords[0]);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 4,
		Resolution, Resolution, 1,
		GL_RGB,
		GL_FLOAT,
		&LightMapData.TriangleID[0]);

	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		0, 0, 5,
		Resolution, Resolution, 1,
		GL_RGB,
		GL_FLOAT,
		&LightMapData.Colors[0]);

	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	std::vector<Mesh::WrappedModel> WrappedModels; 
	for (auto Model : Models) {
		WrappedModels.push_back(Mesh::WrappedModel());
		WrappedModels[WrappedModels.size() - 1].Wrap(Model);
	}

	LightBakerProgressive.Bind(); 
	
	//uniforms 


	
	glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, "BaseModelData"), 0); 
	glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, "PreviousFrame"), 1);
	glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, "ShadowMap"), 2);
	glUniformMatrix4fv(glGetUniformLocation(LightBakerProgressive.ShaderID, "DirectionalViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(LightBakerProgressive.ShaderID, "DirectionalProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));
	glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, "TotalModels"), Models.size());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, BakeDataTextureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Shadows.ShadowMaps[0].DepthBuffer);

	for (int Model = 0; Model < WrappedModels.size(); Model++) {
		

		std::string SIndex = std::to_string(Model) + "]"; 

		std::string ModelDataData = "ModelData[" + SIndex; 
		std::string VerticeCountData = "VerticeCount[" + SIndex;
		std::string ResolutionData = "Resolution[" + SIndex;

		glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, ModelDataData.c_str()), Model + 3); 
		glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, VerticeCountData.c_str()), Models[Model].ModelData.Vertices.size());
		glUniform2i(glGetUniformLocation(LightBakerProgressive.ShaderID, ResolutionData.c_str()), WrappedModels[Model].MeshDataResolution.x, WrappedModels[Model].MeshDataResolution.y);

		glActiveTexture(GL_TEXTURE3 + Model); 
		glBindTexture(GL_TEXTURE_2D_ARRAY, WrappedModels[Model].MeshData);

	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, LightBakerFrameBuffer[0].ColorBuffer);

	for (int Sample = 0; Sample < Samples; Sample++) {

		std::cout << "Sample: " << Sample << " (out of " << Samples << " samples, " << (float(Sample) / float(Samples))*100.f << "%)\n";  

		LightBakerFrameBuffer[Sample % 2].Bind(); 
		glUniform1i(glGetUniformLocation(LightBakerProgressive.ShaderID, "Frame"), Sample);

		Core::DrawPostProcessQuad(); 

		LightBakerFrameBuffer[Sample % 2].UnBind(Window);

		glFinish(); 

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, LightBakerFrameBuffer[Sample%2].ColorBuffer);
	}

	LightBakerProgressive.UnBind();		

	EdgeFixer.Bind(); 

	LightBakerFrameBuffer[(Samples) % 2].Bind(); 

	glUniform1i(glGetUniformLocation(EdgeFixer.ShaderID, "LightBakedTexture"), 0); 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, LightBakerFrameBuffer[(Samples - 1) % 2].ColorBuffer);

	Core::DrawPostProcessQuad(); 

	LightBakerFrameBuffer[(Samples) % 2].UnBind(Window);

	EdgeFixer.UnBind();

	std::vector<float> Data = std::vector<float>((Resolution*Resolution) * 3);
	glBindTexture(GL_TEXTURE_2D, LightBakerFrameBuffer[(Samples) % 2].ColorBuffer);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &Data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	SavePixelDataToImageFromFloatVector(OutPutFile, Data, Vector2i(Resolution)); 


}
