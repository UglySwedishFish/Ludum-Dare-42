#include <fstream>
#include <iostream>
#include <Windows.h>
#include "PathTraceBaker.h"
#include "ReflectionProbeRenderer.h"

void LDEngine::Rendering::ReflectionProbes::ReflectionProbeBaker::PrepareReflectionProbeBaking(unsigned int Resolution)
{
	
	DefferedFrameBuffer = Core::MultiPassFrameBufferObject(Vector2i(Resolution), 4, true); 
	LightFrameBufferObject = Core::FrameBufferObject(Vector2i(Resolution), false); 
	DepthFrameBuffer = Core::FrameBufferObject(Vector2i(Resolution), false, true);

	DefferedRenderer = Core::Shader("Shaders/DefferedPass/vert.glsl", "Shaders/DefferedPass/frag.glsl");
	PostProcessDeffered = Core::Shader("Shaders/ReflectionProbePostProcess/vert.glsl", "Shaders/ReflectionProbePostProcess/frag.glsl");
	ShadowStructureShader = Core::Shader("Shaders/ShadowPass/vert.glsl", "Shaders/ShadowPass/frag.glsl");
	DepthConverter = Core::Shader("Shaders/DepthConvert/vert.glsl", "Shaders/DepthConvert/frag.glsl"); 

	Core::PreparePostProcess();
}


void LDEngine::Rendering::ReflectionProbes::ReflectionProbeBaker::BakeReflectionProbes(Window & Window, std::vector<ReflectionProbe>& ReflectionProbes, std::vector<Mesh::Model> Models, Vector2f ShadowDirection)
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

	Matrix4f CubeProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.f);
	

	for (auto ReflectionProbe : ReflectionProbes) {
		//write basic things to file 


		std::string Title = "BakedLighting/ReflectionProbes/" + std::string(ReflectionProbe.OutPutFile) + ".probe"; 
		std::string Folder = "BakedLighting/ReflectionProbes/" + std::string(ReflectionProbe.OutPutFile);

		std::cout << "Saving Probe " << Folder << '\n';


		std::ofstream File;

		File.open(Title.c_str()); 

		File << ReflectionProbe.Position.x << ' ' << ReflectionProbe.Position.y << ' ' << ReflectionProbe.Position.z << ' ' << ReflectionProbe.Resolution << '\n'; 
		
		File.close(); 

		CreateDirectory(Folder.c_str(), NULL); 
		//TODO: make this work on other platforms as well

		Matrix4f CubeViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		for (int i = 0; i < 6; i++) {
			CubeViews[i] = glm::translate(CubeViews[i], -ReflectionProbe.Position); 

			//draw to texture 

			glEnable(GL_DEPTH_TEST);

			DefferedRenderer.Bind(); 

			DefferedFrameBuffer.Bind(); 

			glUniformMatrix4fv(glGetUniformLocation(DefferedRenderer.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(CubeViews[i]));
			glUniformMatrix4fv(glGetUniformLocation(DefferedRenderer.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(CubeProjection));

			for (auto Model : Models) {
				Mesh::DrawModel(Model, DefferedRenderer, Null, Window); 
			}

			DefferedFrameBuffer.UnBind(Window);
			
			DefferedRenderer.UnBind(); 

			glDisable(GL_DEPTH_TEST);

			LightFrameBufferObject.Bind(); 

			PostProcessDeffered.Bind(); 

			glUniform1i(glGetUniformLocation(PostProcessDeffered.ShaderID, "Albedo"), 0);
			glUniform1i(glGetUniformLocation(PostProcessDeffered.ShaderID, "LightMap"), 1);
			glUniform1i(glGetUniformLocation(PostProcessDeffered.ShaderID, "Normal"), 2);
			glUniform1i(glGetUniformLocation(PostProcessDeffered.ShaderID, "Position"), 3);
			glUniform1i(glGetUniformLocation(PostProcessDeffered.ShaderID, "DirectionalShadowMap"), 4);

			glUniformMatrix4fv(glGetUniformLocation(PostProcessDeffered.ShaderID, "DirectionalViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));
			glUniformMatrix4fv(glGetUniformLocation(PostProcessDeffered.ShaderID, "DirectionalProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));

			for (int i = 0; i < 4; i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, DefferedFrameBuffer.ColorBuffers[i]);
			}

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, Shadows.ShadowMaps[0].DepthBuffer);

			Core::DrawPostProcessQuad();

			PostProcessDeffered.UnBind();

			LightFrameBufferObject.UnBind(Window); 

			DepthFrameBuffer.Bind(); 

			DepthConverter.Bind(); 

			glUniform1i(glGetUniformLocation(DepthConverter.ShaderID, "Depth"), 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, DefferedFrameBuffer.DepthBuffer);

			Core::DrawPostProcessQuad(); 

			DepthConverter.UnBind();

			DepthFrameBuffer.UnBind(Window);
			
			std::string File = Folder + "/" + OutPutNames[i]; 
			std::string DepthFile = Folder + "/" + "Depth" + OutPutNames[i];


			std::vector<float> Data = std::vector<float>((DefferedFrameBuffer.Resolution.x*DefferedFrameBuffer.Resolution.y) * 3);
			glBindTexture(GL_TEXTURE_2D, LightFrameBufferObject.ColorBuffer);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &Data[0]);
			glBindTexture(GL_TEXTURE_2D, 0);

			std::vector<float> DepthData = std::vector<float>((DefferedFrameBuffer.Resolution.x*DefferedFrameBuffer.Resolution.y) * 4);
			glBindTexture(GL_TEXTURE_2D, DepthFrameBuffer.ColorBuffer);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &DepthData[0]);
			glBindTexture(GL_TEXTURE_2D, 0);

			SavePixelDataToImageFromFloatVector(File.c_str(), Data, DefferedFrameBuffer.Resolution, true); 
			SavePixelDataToImageFromFloatVector(DepthFile.c_str(), DepthData, DefferedFrameBuffer.Resolution, true, true);

		}



	}

}
