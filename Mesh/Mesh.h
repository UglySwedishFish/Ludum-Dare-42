#pragma once
#include <unordered_map>
#include <vector>
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Core/Camera.h"
#include "Core Rendering/Shader.h"
#include "Core Rendering/Texture.h"
#include "Core Rendering/Window.h"

namespace LDEngine {
	namespace Rendering {
		namespace Mesh {

			enum Material_Texture_Types { TEXTURE_ALBEDOMAP, TEXTURE_NORMALMAP, TEXTURE_SPECULARMAP, TEXTURE_ROUGHNESSMAP, TEXTURE_PARALLAXMAP, TEXTURE_AOMAP };

			struct Mesh { //note; this is NOT a model. It is only a collection of the data required to construct a model 

				struct MeshEntry {
					unsigned int NumIndices;
					unsigned int BaseVertex;
					unsigned int BaseIndex;
					unsigned int MaterialIndex;
					inline MeshEntry() :
						NumIndices(0),
						BaseVertex(0),
						BaseIndex(0),
						MaterialIndex(0)
					{}
				};


				std::vector<glm::vec3> Vertices;
				std::vector<glm::vec3> TextureCoordinates;
				std::vector<glm::vec3> Normals;
				std::vector<glm::vec3> Tangents;
				std::vector<unsigned int> Indices;
				std::vector<MeshEntry> MeshEntries;
				std::vector<unsigned int> Materials;
				void ClearVectors();
				~Mesh();

			};

			struct Material {
				Vector3f AlbedoMultiplier;
				Vector3f ValueTwo;
				inline Material(Vector3f c = Vector3f(1.)) : AlbedoMultiplier(c), ValueTwo(0.) {}
			};

			struct MeshMaterials {
				std::vector<Material> Materials;
				unsigned int MaterialTexture;
				void PrepareMaterials();
				void UpdateMaterials();
				~MeshMaterials();
			};

			bool InitMaterials(const aiScene* pScene, const std::string& Filename, MeshMaterials &Materials);
			bool InitMesh(const aiMesh * aiMesh, Mesh & mesh);

			void LoadMeshData(const char * file, Mesh & Model, Mesh & RefractiveModelData, MeshMaterials & Materials, std::vector<int> RefractiveMaterials);

			struct Model {
				Mesh ModelData; //where all of the mesh data is stored
				Mesh RefractiveModelData; 
				MeshMaterials MaterialData; //where all of the material data is stored
				LDEngine::Rendering::Core::Texture2D LightMap; //contains a light map of all the baked lighting for the mesh 
				bool HasLightMap; //if the model has a lightmap or not
				Vector3f Position = Vector3f(0.), Rotation = Vector3f(0.);


				unsigned int Vao, Vbo[5];
				unsigned int RefractiveVao, RefractiveVbo[5]; 

				inline Model(Mesh ModelData, MeshMaterials MaterialData) :
					Vao(),
					Vbo{},
					ModelData(ModelData),
					MaterialData(MaterialData)
				{}
				inline Model(const char *file, const char * lightmap = "none", std::vector<int> RefractiveMaterials = {}) :
					Vao(),
					Vbo{},
					ModelData(Mesh()),
					MaterialData(MeshMaterials()),
					HasLightMap(lightmap != "none")
				{
					LoadMeshData(file, ModelData, RefractiveModelData, MaterialData, RefractiveMaterials);
					PrepareForRendering(); 
					if (HasLightMap) {
						LightMap = LDEngine::Rendering::Core::LoadTexture2D(lightmap, true, false); 
					}
				}

				inline Model() :
					Vao(),
					Vbo{},
					ModelData(Mesh()),
					MaterialData(MeshMaterials())
				{}

				void SetAlbedoMultiplier(unsigned int index, Vector3f Multiplier);
				void SetTexture(unsigned int index, unsigned int texture);
				void PrepareForRendering();
			};


			void DrawRefractiveModel(Model & Model, Core::Shader Shader, Control::Camera Camera, Window Window); 
			void DrawModel(Model & Model, Core::Shader Shader, Control::Camera Camera, Window Window);
		}
	}
}