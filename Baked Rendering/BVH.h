#pragma once
#include <vector>
#include "Core/Core.h"
#include "Mesh/Mesh.h"

namespace LDEngine {
	namespace Rendering {
		namespace Mesh {
			struct TriangleBoundingBox : LDEngine::Core::BoundingBox {
				unsigned int Index;
				unsigned int Index2;
			};
			using TriangleBoundingBoxEntry = std::vector<TriangleBoundingBox>;
			struct BVHNode : LDEngine::Core::BoundingBox {
				bool Leaf = false; 
				BVHNode * Left = nullptr, *Right = nullptr;
				TriangleBoundingBoxEntry LeafEntry;
				unsigned int StartIndex;
			};
			

			

			BVHNode * ConstructBoundingVolumeHieracy(Mesh & Mesh);

			struct BSTImageData {
				std::vector<Vector4f> ImageData;
				void Construct(unsigned int Width, unsigned int Height) {
					ImageData = std::vector<Vector4f>(Width*Height, Vector4f(0.));
				}
			};

			BSTImageData StoreBVHInImage(BVHNode * Node, unsigned int Width, unsigned int Height); 

		}
	}
}




