#pragma once
#include <Core/Core.h>
#include <array>

namespace LDEngine {
	namespace Collisions {

		const float PlayerHeight = 1.8; 
		const float PlayerSize = 0.25; 

		enum CollisionFace {PositiveX, NegativeX, PositiveY, NegativeY, NegativeZ, PositiveZ};


		struct AABB {
			Vector3f Min, Max, Size;
			AABB(Vector3f Min = Vector3f(0.), Vector3f Max = Vector3f(0.)) : Min(Min), Max(Max) {}

			AABB(float s, Vector3f Center, Vector3f Size) : Min(Center - Size), Max(Center + Size), Size(Size) {}

			bool operator==(AABB Other) { //AABB, AABB intersection
				return Min.x <= Other.Max.x && Max.x >= Other.Min.x &&
					Min.y <= Other.Max.y && Max.y >= Other.Min.y &&
					Min.z <= Other.Max.z && Max.z >= Other.Min.z;
			}
			bool operator==(Vector3f Point) {
				return Point.x >= Min.x && Point.x <= Max.x &&
					Point.y >= Min.y && Point.y <= Max.y &&
					Point.z >= Min.z && Point.z <= Max.z;
			}
			Vector3f Center() {
				return (Min + Max) / 2.f; 
			}

			bool Collision(Vector3f & Position, float ObjectHeight, float ClimbPotential, std::array<bool, 6> & SideCollisions) {
				
				if (AABB(Min - Vector3f(0., ObjectHeight, 0.), Max) == Position) { //checks for intersection betwen  position and box

					//find normal 

					Vector3f NewSize = abs(Max - (Min - Vector3f(0., ObjectHeight, 0.))); 
					Vector3f NewCenter = (Max + (Min - Vector3f(0., ObjectHeight, 0.)))/2.f;

					Vector3f Normal = (NewCenter - Position) / NewSize;

					if (abs(Normal.x) > abs(Normal.y) && abs(Normal.x) > abs(Normal.z)) {
						
						if (abs(Max.y - Position.y) < ClimbPotential) {
							Position.y = Max.y;
							SideCollisions[PositiveY] = true; 
						}
						else {
							SideCollisions[NewCenter.x > Position.x ? NegativeX : PositiveX] = true;
							Position.x = NewCenter.x > Position.x ? Min.x : Max.x;
						}

					}
					else if (abs(Normal.y) > abs(Normal.z)) {
						
						SideCollisions[NewCenter.y > Position.y ? NegativeY : PositiveY] = true;
						Position.y = NewCenter.y > Position.y ? (Min.y - ObjectHeight) : Max.y;
					}
					else {
						
						if (abs(Max.y - Position.y) < ClimbPotential) {
							Position.y = Max.y;
							SideCollisions[PositiveY] = true;
						}
						else {
							SideCollisions[NewCenter.z > Position.z ? NegativeZ : PositiveZ] = true;
							Position.z = NewCenter.z > Position.z ? Min.z : Max.z;
						}
					}

					return true; 
					
				}

				return false; 

			}
		};
	}
}