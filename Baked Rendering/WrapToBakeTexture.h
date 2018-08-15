#include <vector>
#include "Core/Core.h"
#include "Mesh/Mesh.h"

namespace LDEngine {
	namespace Rendering {
		namespace Mesh {
			struct LightMapData {
				std::vector<Vector3f> Vertices, Normals, Tangents, TexCoords, TriangleID, Colors; 
			};
			LightMapData ProjectToBakeTexture(Model & ModelData, unsigned int Resolution); 
		}
	}
}