#include "RenderPipeLine.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <Baked Rendering/reflectionproberenderer.h>

void GetFilesInDirectory(std::vector<std::string> &out, const std::string &directory)
{
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + "/*.probe").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const std::string file_name = file_data.cFileName;
		const std::string full_file_name = directory + "/" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		out.push_back(full_file_name);
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
}

void LDEngine::Rendering::Pipeline::RenderPipeline::Prepare(Window & Window, Control::Camera & Camera, std::vector<Mesh::Model> & Models, std::vector<Mesh::AnimatedKeyFrameModel> & AnimatedKeyFrameModels, Vector2f Direction)
{
	this->Direction = Direction; 
	glClearColor(0., 0., 0., -1.); 
	glDepthFunc(GL_LEQUAL); 
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 

	Core::PreparePostProcess(); 
	
	this->Models = Models; 
	this->AnimatedKeyFrameModels = AnimatedKeyFrameModels; 
	DefferedShader = Core::Shader("Shaders/DefferedPass/vert.glsl", "Shaders/DefferedPass/frag.glsl"); 
	Compositing = Core::Shader("Shaders/Compositing/vert.glsl", "Shaders/Compositing/frag.glsl"); 
	ShadowPass = Core::Shader("Shaders/ShadowPass/vert.glsl", "Shaders/ShadowPass/frag.glsl"); 
	DefferedLightingShader = Core::Shader("Shaders/DefferedLighting/vert.glsl", "Shaders/DefferedLighting/frag.glsl"); 
	Particles = Core::Shader("Shaders/ParticleSystem/vert.glsl", "Shaders/ParticleSystem/frag.glsl");
	FinalLighting = Core::Shader("Shaders/FinalLighting/vert.glsl", "Shaders/FinalLighting/frag.glsl"); 
	EnemySystem.PrepareEnemySystem(); 

	DefferedLighting = Core::MultiPassFrameBufferObject(Window.GetResolution(), 2, false); 
	ParticleSystemFrameBuffer = Core::FrameBufferObject(Window.GetResolution(), false);
	FinalLightingFrameBuffer = Core::FrameBufferObject(Window.GetResolution(), false); 

	DefferedFrameBuffer = Core::MultiPassFrameBufferObject(Window.GetResolution(), 4, true); 
	RefractiveDefferedFrameBuffer = Core::MultiPassFrameBufferObject(Window.GetResolution(), 4, true);

	DefferedLightingShader.Bind(); 

	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "Albedo"), 0);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "LightMap"), 1);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "Normal"), 2);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "Position"), 3);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "DirectionalShadowMap"), 4);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "ReflectionProbes"), 5);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "ReflectionProbePositions"), 6);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "ReflectionProbesWorldPosition"), 7);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "Particles"), 8);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "ParticleDepth"), 9);
	glUniform1i(glGetUniformLocation(DefferedLightingShader.ShaderID, "Depth"), 10);

	DefferedLightingShader.UnBind();

	Compositing.Bind(); 

	glUniform1i(glGetUniformLocation(Compositing.ShaderID, "Lighting"), 0); 
	glUniform1i(glGetUniformLocation(Compositing.ShaderID, "UI"), 1);

	Compositing.UnBind(); 

	Particles.Bind(); 

	glUniform1i(glGetUniformLocation(Particles.ShaderID, "Positions"), 1);

	Particles.UnBind(); 

	FinalLighting.Bind(); 

	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "CurrentLighting"), 0);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "ViewPosition"), 1);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "Normals"), 2);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "ParticleDepth"), 3);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "AlbedoDepth"), 4);

	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "RefractiveAlbedo"), 5);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "RefractiveNormal"), 6);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "RefractiveWorldPosition"), 7);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "RefractiveDepth"), 8);


	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "ReflectionProbes"), 9);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "ReflectionProbePositions"), 10);
	glUniform1i(glGetUniformLocation(FinalLighting.ShaderID, "ReflectionProbesWorldPosition"), 11);

	FinalLighting.UnBind(); 

	Shadows.UpdateRotation(Direction, Camera);
	Shadows.PrepareShadowStructure(Vector2i(4096)); 

	//load reflection probes 

	std::vector<std::string> Probes; 
	GetFilesInDirectory(Probes, "BakedLighting/ReflectionProbes"); 

	glGenTextures(1, &ReflectionProbes.Probes); 
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.Probes); 

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_RGBA8, 256, 256, Probes.size() * 6, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.ProbeWorldPosition);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_RGB32F, 256, 256, Probes.size() * 6, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);


	int ProbeIndex = 0;

	Matrix4f CubeProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 1000.f);

	for (auto & Probe : Probes) {

		//load positions 

		std::ifstream File(Probe.c_str()); 
		
		std::string Line; 
		
		std::getline(File, Line); 

		Vector3f Position; 
		unsigned int Resolution; 

		std::sscanf(Line.c_str(), "%f %f %f %d", &Position.x, &Position.y, &Position.z, &Resolution); 

		ReflectionProbes.Positions.push_back(Position); 

		std::cout << Position.x << ' ' << Position.y << ' ' << Position.z << '\n'; 

		std::string Folder = Probe; 
		for (int i = 0; i < 6; i++)
			Folder.pop_back(); 

		std::string Names[6]; 
		std::string NamesDepth[6];

		for (int i = 0; i < 6; i++) {
			Names[i] = Folder + '/' + OutPutNames[i];
			NamesDepth[i] = Folder + '/' + "Depth" + OutPutNames[i];
		}

		


		Matrix4f CubeViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		for (int Face = 0; Face < 6; Face++) {

			CubeViews[Face] = glm::translate(CubeViews[Face], -Position);

			sf::Image image;
			image.loadFromFile(Names[Face]);
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.Probes);
			glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, ProbeIndex * 6 + Face, 256, 256, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);



			Vector3f WorldPositions[65536];
			sf::Image DepthImage; 
			DepthImage.loadFromFile(NamesDepth[Face]); 

			for (int x = 0; x < 256; x++) {
				for (int y = 0; y < 256; y++) {
					sf::Color Color = DepthImage.getPixel(x, y); 

					int DepthInt = Color.r + Color.g * 255 + Color.b * 65536; 

					float ActualDepth = float(DepthInt) / 1000.f; 

					//convert depth to world position

					float z = ActualDepth * 2. - 1.; 

					Vector2f TexCoord = Vector2f(float(x) / 256, float(y) / 256); 

					Vector3f IncidentRayDirection = glm::normalize(Vector3f(glm::inverse(CubeProjection * Matrix4f(Matrix3f(CubeViews[Face]))) * Vector4f(TexCoord * 2.f - 1.f, 1.f, 1.f)));

					WorldPositions[y * 256 + x] = Position + IncidentRayDirection * ActualDepth; 
				
				}
			}

			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.ProbeWorldPosition);

			glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, ProbeIndex * 6 + Face, 256, 256, 1, GL_RGB, GL_FLOAT, WorldPositions);

			glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

		}
		ProbeIndex++;

	}

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.Probes);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.ProbeWorldPosition);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	glGenTextures(1, &ReflectionProbes.ProbePositions); 
	glBindTexture(GL_TEXTURE_1D, ReflectionProbes.ProbePositions);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, Probes.size(), 0, GL_RGB, GL_FLOAT, &ReflectionProbes.Positions[0]);
	glBindTexture(GL_TEXTURE_1D, 0);


}

void LDEngine::Rendering::Pipeline::RenderPipeline::Draw(Window & Window, Control::Camera & Camera, Game::Weapon * Weapon, UI::UIPipeLine & UI)
{

	

	for (auto & KeyFrameModel : AnimatedKeyFrameModels)
		KeyFrameModel.UpdateAnimation(Window.GetFrameTime()); 

	Shadows.UpdateRotation(Direction, Camera);
	glEnable(GL_DEPTH_TEST);

	Shadows.ShadowMaps[0].Bind(); 

	ShadowPass.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(ShadowPass.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(ShadowPass.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));

	for (auto & Model : Models)
		Mesh::DrawModel(Model, ShadowPass, Camera, Window);

	for (auto & KeyFrameModel : AnimatedKeyFrameModels)
		Mesh::DrawAnimatedKeyFrameModel(ShadowPass, KeyFrameModel, 0);

	EnemySystem.DrawEnemies(Window, Camera, ShadowPass);

	ShadowPass.UnBind(); 

	Particles.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(Particles.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));

	EnemySystem.DrawEnemyParticleSystem(Camera.View, Particles);

	Particles.UnBind(); 

	Shadows.ShadowMaps[0].UnBind(Window);

	DefferedShader.Bind(); 

	DefferedFrameBuffer.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(DefferedShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(DefferedShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));

	for (auto & Model : Models)
		Mesh::DrawModel(Model, DefferedShader, Camera, Window); 

	for (auto & KeyFrameModel : AnimatedKeyFrameModels)
		Mesh::DrawAnimatedKeyFrameModel(DefferedShader, KeyFrameModel, 0); 

	EnemySystem.DrawEnemies(Window, Camera, DefferedShader); 

	Weapon->Draw(Camera, Window, DefferedShader, sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)); 

	glUniformMatrix4fv(glGetUniformLocation(DefferedShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));

	DefferedFrameBuffer.UnBind(Window); 

	RefractiveDefferedFrameBuffer.Bind(); 

	for (auto & Model : Models)
		Mesh::DrawRefractiveModel(Model, DefferedShader, Camera, Window); 

	for (auto & KeyFrameModel : AnimatedKeyFrameModels)
		Mesh::DrawAnimatedKeyFrameModel(DefferedShader, KeyFrameModel, 0, true);

	RefractiveDefferedFrameBuffer.UnBind(Window); 

	DefferedShader.UnBind();

	ParticleSystemFrameBuffer.Bind(); 

	Particles.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(Particles.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));

	EnemySystem.DrawEnemyParticleSystem(Camera.View, Particles); 

	Particles.UnBind();

	ParticleSystemFrameBuffer.UnBind(Window); 

	glDisable(GL_DEPTH_TEST);

	DefferedLighting.Bind(); 

	DefferedLightingShader.Bind(); 

	glUniformMatrix4fv(glGetUniformLocation(DefferedLightingShader.ShaderID, "DirectionalViewMatrix"), 1, false, glm::value_ptr(Shadows.ViewMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(DefferedLightingShader.ShaderID, "DirectionalProjectionMatrix"), 1, false, glm::value_ptr(Shadows.ProjectionMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(DefferedLightingShader.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(DefferedLightingShader.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));
	glUniform3f(glGetUniformLocation(DefferedLightingShader.ShaderID, "CameraPosition"), Camera.Position.x, Camera.Position.y + 1.8, Camera.Position.z);

	for (int i = 0; i < 4; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, DefferedFrameBuffer.ColorBuffers[i]);
	}

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Shadows.ShadowMaps[0].DepthBuffer);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.Probes);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_1D, ReflectionProbes.ProbePositions);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.ProbeWorldPosition);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, ParticleSystemFrameBuffer.ColorBuffer);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, ParticleSystemFrameBuffer.DepthBuffer);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, DefferedFrameBuffer.DepthBuffer);

	Core::DrawPostProcessQuad();

	DefferedLightingShader.UnBind(); 

	DefferedLighting.UnBind(Window);

	FinalLightingFrameBuffer.Bind(); 

	FinalLighting.Bind(); 

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, DefferedLighting.ColorBuffers[0]); 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, DefferedLighting.ColorBuffers[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, DefferedFrameBuffer.ColorBuffers[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ParticleSystemFrameBuffer.DepthBuffer);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, DefferedFrameBuffer.DepthBuffer);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, RefractiveDefferedFrameBuffer.ColorBuffers[0]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, RefractiveDefferedFrameBuffer.ColorBuffers[2]);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, RefractiveDefferedFrameBuffer.ColorBuffers[3]);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, RefractiveDefferedFrameBuffer.DepthBuffer);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.Probes);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_1D, ReflectionProbes.ProbePositions);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ReflectionProbes.ProbeWorldPosition);


	glUniformMatrix4fv(glGetUniformLocation(FinalLighting.ShaderID, "ViewMatrix"), 1, false, glm::value_ptr(Camera.View));
	glUniformMatrix4fv(glGetUniformLocation(FinalLighting.ShaderID, "ProjectionMatrix"), 1, false, glm::value_ptr(Camera.Project));
	glUniform3f(glGetUniformLocation(FinalLighting.ShaderID, "CameraPosition"), Camera.Position.x, Camera.Position.y + 1.8, Camera.Position.z);


	Core::DrawPostProcessQuad(); 

	FinalLighting.UnBind(); 

	FinalLightingFrameBuffer.UnBind(Window); 

	Compositing.Bind(); 

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FinalLightingFrameBuffer.ColorBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, UI.UIFrameBuffer.ColorBuffer);

	Core::DrawPostProcessQuad(); 

	Compositing.UnBind(); 

	Window.GetRawWindow()->display(); 
}

void LDEngine::Rendering::Pipeline::RenderPipeline::AddEnemy(Game::Enemy Enemy)
{
	EnemySystem.AddEnemy(Enemy); 
}
