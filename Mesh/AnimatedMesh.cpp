#include "AnimatedMesh.h"
#include <GL/glew.h>
#include <iostream>

void LDEngine::Rendering::Mesh::DrawAnimatedMeshModel(Window & Window, Core::Shader Shader, Control::Camera Camera, AnimatedMeshModel & Model, unsigned char Frame)
{
	DrawModel(Model.Models[Frame], Shader, Camera, Window); 
}

LDEngine::Rendering::Mesh::AnimatedMeshModel LDEngine::Rendering::Mesh::LoadAnimatedMeshModel(const std::string & File, unsigned char Frames, float Speed)
{
	AnimatedMeshModel Return; 
	for (int Frame = 1; Frame <= Frames; Frame++) {
		std::string Title = File; 
		Title += ((Frame < 10) ? "_00000" : (Frame < 100) ? "_0000" : (Frame < 1000) ? "_000" : "_00") + std::to_string(Frame) +  ".obj"; 
		Return.Models.push_back(Model(Title.c_str())); 
	}
	return Return; 
}

const float tau = 6.28318;
const float halfpi = 1.5707; 

Vector3f Interpolate(Vector3f A, Vector3f B, float MixFactor,  LDEngine::Rendering::Mesh::InterpolationTypes Type) {
	return glm::mix(A, B, glm::clamp(Type == LDEngine::Rendering::Mesh::SINE ? (std::sin((tau * MixFactor) - halfpi) * .5 + .5): MixFactor,0.,1.));
}

void LDEngine::Rendering::Mesh::DrawAnimatedKeyFrameModel(Core::Shader Shader, AnimatedKeyFrameModel & Model, unsigned short Frame, bool Refractive)
{
	Vector3f Position = Vector3f(0.), Rotation = Vector3f(0.), Scale = Vector3f(1.);

	if (Model.KeyFrames.size() == 1) {
		Position = Model.KeyFrames[0].Position; 
		Rotation = Model.KeyFrames[0].Rotation;
		Scale = Model.KeyFrames[0].Scale;
	}
	else if (Model.KeyFrames.size() >= 2) {


		
		
		KeyFrame ClosestAbove, ClosestBelow; 
		bool HasClosestAbove = false, HasClosestBelow = false; 
		

		bool HitKeyFrame = false; //if current frame was exactly one keyframes frame
		unsigned short CurrentFrame = Frame;


		for (auto & KeyFrame : Model.KeyFrames) {
			if (CurrentFrame == KeyFrame.PositionInAnimation) {
				HitKeyFrame = true; 
				Position = KeyFrame.Position;
				Rotation = KeyFrame.Rotation;
				Scale = KeyFrame.Scale;



			}
			else {
				if (KeyFrame.PositionInAnimation > CurrentFrame && (KeyFrame.PositionInAnimation < ClosestAbove.PositionInAnimation || !HasClosestAbove)) {
					HasClosestAbove = true; 
					ClosestAbove = KeyFrame; 
				}
				else if (KeyFrame.PositionInAnimation < CurrentFrame && (KeyFrame.PositionInAnimation > ClosestAbove.PositionInAnimation || !HasClosestBelow)) {
					HasClosestBelow = true;
					ClosestBelow = KeyFrame;
				}
			}
		}

		if (!HitKeyFrame) {

			

			float Interpolation = float(CurrentFrame - ClosestBelow.PositionInAnimation) / float(ClosestAbove.PositionInAnimation - ClosestBelow.PositionInAnimation); 

			Position = Interpolate(ClosestBelow.Position, ClosestAbove.Position, Interpolation, Model.Interpolations[0]); 
			Rotation = Interpolate(ClosestBelow.Rotation, ClosestAbove.Rotation, Interpolation, Model.Interpolations[1]);
			Scale = Interpolate(ClosestBelow.Scale, ClosestAbove.Scale, Interpolation, Model.Interpolations[2]);
		}
	}

	Matrix4f ModelMatrix = LDEngine::Core::ModelMatrix(Position + Model.Model.Position, Rotation + Model.Model.Rotation);
	glUniform1i(glGetUniformLocation(Shader.ShaderID, "HasLightMap"), false);
	glUniform1i(glGetUniformLocation(Shader.ShaderID, "LightMap"), 1);
	glUniform1i(glGetUniformLocation(Shader.ShaderID, "Materials"), 0);
	glUniformMatrix4fv(glGetUniformLocation(Shader.ShaderID, "ModelMatrix"), 1, false, glm::value_ptr(ModelMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, Model.Model.MaterialData.MaterialTexture);

	glBindVertexArray(Refractive ? Model.Model.RefractiveVao : Model.Model.Vao);

	glDrawElements(GL_TRIANGLES, Refractive ? Model.Model.RefractiveModelData.Vertices.size() : Model.Model.ModelData.Vertices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

}

void LDEngine::Rendering::Mesh::AnimatedKeyFrameModel::AttatchKeyFrame(KeyFrame KeyFrame)
{
	KeyFrames.push_back(KeyFrame); 
	AnimationLength = std::max(int(AnimationLength), KeyFrame.PositionInAnimation + 1); 
}

void LDEngine::Rendering::Mesh::AnimatedKeyFrameModel::UpdateAnimation(float FrameTime)
{
	TimeSinceLastFrameChange += FrameTime; 
	if (TimeSinceLastFrameChange > Speed) {
		Frame++; 
		TimeSinceLastFrameChange = 0.; 
		if (Frame >= AnimationLength-1)
			Frame = 0; 
	}
}
