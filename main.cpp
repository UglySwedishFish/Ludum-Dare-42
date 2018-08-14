//#define BAKE_LIGHTMAP
//#define BAKE_REFLECTIONPROBES

#ifdef BAKE_LIGHTMAP
#include <Baked Rendering/PathTraceBaker.h>
#else
#ifdef BAKE_REFLECTIONPROBES
#include <Baked Rendering/reflectionproberenderer.h>

void AddReflectionProbesArray(std::vector<LDEngine::Rendering::ReflectionProbes::ReflectionProbe> & Probes, int & Title, Vector3f BasePosition, Vector3f Offset, int ArrayCount) {

	for (int i = 0; i < ArrayCount; i++)
		Probes.push_back(LDEngine::Rendering::ReflectionProbes::ReflectionProbe(std::to_string(Title++), BasePosition + Offset * Vector3f(i), 64)); 

}

#else
#include <Pipeline/Pipeline.h>
#endif
#endif



int main() {

#ifdef BAKE_LIGHTMAP 


	Window Screen;
	Screen.SetTitle("LDEngine Test 1");
	Screen = Window(Vector2i(100, 100), false);

	LDEngine::Control::Camera Camera = LDEngine::Control::Camera(70.0, 0.01, 1000.0, Vector3f(0.122498, 1.76084, 7.83415), Vector3f(0.), Screen); //fov, znear, zfar, position, rotation, display 



	std::vector<LDEngine::Rendering::Mesh::Model> Models{ LDEngine::Rendering::Mesh::Model("Models/Map/Buildings.obj"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Details.obj"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Ores.obj"),
		LDEngine::Rendering::Mesh::Model("Models/Map/SpaceShips.obj"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Terrain.obj") };


	LDEngine::Rendering::PathTracing::PathTracingPipeLine Pipeline; 

	

	Pipeline.PrepareLightBaker(3072); 


	Pipeline.BakeLighting(Screen, Models[0], Models, "Buildings.png", 512, 3, Vector2f(30., 235.));


#else

#ifdef BAKE_REFLECTIONPROBES
	Window Screen;
	Screen.SetTitle("LDEngine Test 1");
	Screen = Window(Vector2i(100, 100), false);

	std::vector<LDEngine::Rendering::Mesh::Model> Models
	{ LDEngine::Rendering::Mesh::Model("Models/Map/Buildings.obj", "BakedLighting/LightMaps/Buildings.png",{ 1 }),
		LDEngine::Rendering::Mesh::Model("Models/Map/Details.obj", "BakedLighting/LightMaps/Details.png"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Ores.obj", "BakedLighting/LightMaps/Ores.png"),
		LDEngine::Rendering::Mesh::Model("Models/Map/SpaceShips.obj", "BakedLighting/LightMaps/SpaceShips.png"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Terrain.obj", "BakedLighting/LightMaps/Terrain.png") };

	std::vector<LDEngine::Rendering::ReflectionProbes::ReflectionProbe> ReflectionProbes; 
	
	int title = 0; 

	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(-15.2492, 4.39615, 20.8138), Vector3f(0., 0., -3.), 8);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(-7.72972, 3.58149, 20.8138), Vector3f(0., 0., -3.), 8);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(4.92312, 2.40051, 20.8138), Vector3f(0., 0., -3.), 8);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(13.6941, 1.23129, 20.8138), Vector3f(0., 0., -3.), 8);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(40.1044, 2.40051, 20.8138), Vector3f(0., 0., -3.), 8);

	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(17.9306, 4.37698, 7.88091), Vector3f(0., 0., -8.), 2);

	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(23.2134, 8.51854, 8.82486), Vector3f(0., 0., -3.), 4);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(29.1324, 8.51854, 8.82486), Vector3f(0., 0., -3.), 4);

	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(21.2416, 2.457, 0.311063), Vector3f(3., 0., 0.), 3);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(21.2416, 7.88254, 0.311063), Vector3f(3., 0., 0.), 3);

	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(56.3246, 3.11482, -10.6005), Vector3f(-2.1, 0., -.3), 2);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(57.6327, 2.15921, 19.2266), Vector3f(0., 0., -3.), 2);
	AddReflectionProbesArray(ReflectionProbes, title, Vector3f(52.5002, 5.92333, 2.92682), Vector3f(0., 0., -3.), 2);


	LDEngine::Rendering::ReflectionProbes::ReflectionProbeBaker ReflectionProbeBaker; 
	ReflectionProbeBaker.PrepareReflectionProbeBaking(256);
	ReflectionProbeBaker.BakeReflectionProbes(Screen, ReflectionProbes, Models, Vector2f(30., 235.));


	
#else

	Window Screen;
	Screen.SetTitle("LDEngine Test 1");
	Screen = Window(Vector2i(1920, 1080), false);

	LDEngine::Control::Camera Camera = LDEngine::Control::Camera(70.0, 0.01, 1000.0, Vector3f(5.70609, 0.957678, -11.8755), Vector3f(0.), Screen); //fov, znear, zfar, position, rotation, display 

	std::vector<LDEngine::Rendering::Mesh::Model> Models
	{ LDEngine::Rendering::Mesh::Model("Models/Map/Buildings.obj", "BakedLighting/LightMaps/Buildings.png", {1}),
		LDEngine::Rendering::Mesh::Model("Models/Map/Details.obj", "BakedLighting/LightMaps/Details.png"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Ores.obj", "BakedLighting/LightMaps/Ores.png"),
		LDEngine::Rendering::Mesh::Model("Models/Map/SpaceShips.obj", "BakedLighting/LightMaps/SpaceShips.png"),
		LDEngine::Rendering::Mesh::Model("Models/Map/Terrain.obj", "BakedLighting/LightMaps/Terrain.png") };

	std::vector<LDEngine::Rendering::Mesh::AnimatedKeyFrameModel> AnimatedModels{ LDEngine::Rendering::Mesh::AnimatedKeyFrameModel() };
	

	LDEngine::Rendering::Pipeline::Pipeline Pipeline; //the pipeline itself 
	Pipeline.Prepare(Screen, Camera, Models, AnimatedModels, Vector2f(30.,235.));
	Pipeline.Run(Screen, Camera);

#endif
#endif


}


