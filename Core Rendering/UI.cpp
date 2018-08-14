#include "UI.h"

void LDEngine::Rendering::UI::UIElement::Draw(Core::Shader & Shader, Window & Window)
{

	glUniform2f(glGetUniformLocation(Shader.ShaderID, "Scale"), Scale.x, Scale.y); 
	glUniform2f(glGetUniformLocation(Shader.ShaderID, "Position"), Position.x, Position.y);

	Texture.Bind(0);

	Core::DrawPostProcessQuad(); 

}

void LDEngine::Rendering::UI::UIPipeLine::AllocateElements(std::vector<UIElement> Elements)
{
	this->Elements = Elements; 
}

void LDEngine::Rendering::UI::UIPipeLine::AllocateElement(UIElement Element)
{
	Elements.push_back(Element); 
}

void LDEngine::Rendering::UI::UIPipeLine::PrepareUiPipeLine(Window & Window	)
{
	UIShader = Core::Shader("Shaders/UI/vert.glsl", "Shaders/UI/frag.glsl"); 
	UIFrameBuffer = Core::FrameBufferObject(Window.GetResolution(), false); 
}

void LDEngine::Rendering::UI::UIPipeLine::DrawUI(Window & Window, bool UseFrameBuffer)
{
	if(UseFrameBuffer)
	UIFrameBuffer.Bind(); 
	UIShader.Bind(); 

	for (auto & Element : Elements)
		Element.Draw(UIShader, Window); 

	UIShader.UnBind(); 
	if(UseFrameBuffer)
	UIFrameBuffer.UnBind(Window); 
}

void LDEngine::Rendering::UI::UIPipeLine::Remove_Last()
{
	Elements.pop_back(); 
}
