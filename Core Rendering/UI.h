#pragma once

#include "Texture.h"
#include "FrameBuffer.h"
#include "Shader.h"

namespace LDEngine {
	namespace Rendering {
		namespace UI {

			struct UIElement {
				Vector2f Position; 
				Vector2f Scale; 

				Core::Texture2D Texture; 


				UIElement(Vector2f Position = Vector2f(0.), Vector2f Scale = Vector2f(0.), Core::Texture2D Texture = Core::Texture2D()) : 
					Position(Position), Scale(Scale), Texture(Texture) {}

				void Draw(Core::Shader & Shader, Window & Window); 

			};

			class UIPipeLine {
				std::vector<UIElement> Elements; 
				Core::Shader UIShader; 
			public: 
				Core::FrameBufferObject UIFrameBuffer;
				void AllocateElements(std::vector<UIElement> Elements); 
				void AllocateElement(UIElement Element);
				void PrepareUiPipeLine(Window & Window); 
				void DrawUI(Window & Window, bool UseFrameBuffer = true);
				void Remove_Last(); 

			};
		}
	}
}