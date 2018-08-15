#pragma once
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "BVH.h"
#include "Mesh/Mesh.h"

#define MATERIAL_TEXTURE_QUALITY 256 //the texture maps' resolution (they all have to be the same) 
#define MATERIAL_TEXTURE_QUALITY_SQUARED MATERIAL_TEXTURE_QUALITY * MATERIAL_TEXTURE_QUALITY
//^ could probably be constants 
namespace LDEngine { 
	namespace Rendering {
		namespace Mesh {
			struct WrappedModel {
				unsigned int MeshData, KDTreeData; 
				Vector2i MeshDataResolution; 
				WrappedModel();
				MeshMaterials MaterialData; 
				bool Wrap(Model &WrapModel); 
			}; 
		}
	}
}