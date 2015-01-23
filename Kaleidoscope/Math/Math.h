#pragma once
#include <Utility/Typedefs.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <glm/gtc/constants.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/gtx/norm.hpp>

namespace kaleidoscope
{
	namespace math = glm;



	namespace kmath
	{
			const F32 PI = 3.141592653589793f;


			/*
			* kaleidoscope::math::mat4 kaleidoscope::kmath::scaleMat4(math::vec3 scaleValues)
			*
			* In: vec3 scaleValues : The scales for each of the x, y, and z axes.
			* Out: mat4 : A matrix representing the desired scaling along the x, y, and z axis
			*/
			inline math::mat4 scaleMat4(math::vec3 scaleValues)
			{
				return math::mat4(scaleValues.x, 0.0f, 0.0f, 0.0f,
								  0.0f, scaleValues.y, 0.0f, 0.0f,
								  0.0f, 0.0f, scaleValues.z, 0.0f,
								  0.0f, 0.0f, 0.0f, 1.0);
			}


			/*
			* kaleidoscope::math::mat4 kaleidoscope::kmath::translationMat4(math::vec3 translation)
			*
			* In: vec3 translation : The translation.
			* Out: mat4 : A matrix representing the provided translation.
			*/
			inline math::mat4 translationMat4(math::vec3 translation)
			{
				return math::mat4(1.0f, 0.0f, 0.0f, 0.0f,
								  0.0f, 1.0f, 0.0f, 0.0f,
								  0.0f, 0.0f, 1.0f, 0.0f,
								  translation.x, translation.y, translation.z, 1.0f);
			}


			/*
			* kaleidoscope::math::quat kaleidoscope::kmath::rotationAboutAxisQuat(math::vec3 a, F32 theta)
			*
			* In: vec3 a : The normalized axis to rotate about.
			* In: F32 theta : The angle in radians to rotate about the axis.
			* Out: quat : A quaternion representing the desired rotation about the desired radius.
			*
			* AXIS MUST BE NORMALIZED
			* ANGLE MUST BE IN RADIANS
			*/
			inline math::quat rotationAboutAxisQuat(math::vec3 a, F32 theta)
			{
				F32 sinThetaOver2 = sin(theta / 2.0f);
				return math::quat(cos(theta / 2.0f), a.x * sinThetaOver2, a.y * sinThetaOver2, a.z * sinThetaOver2);
			}


			/*
			* kaleidoscope::math::vec3 kaleidoscope::kmath::eulerAnglesNoGimalProtect(math::quat q)
			*
			* In: quat q: A quaternion representing an arbitrary rotation.
			* Out: vec3 : A vec3 with the corresponding rotation represented as euler angles.
			* 
			* THE OUTPUT IS IN RADIANS
			* THE OUTPUT IS NOT ADJUSTED TO AVOID GIMBAL CONDITIONS. 
			*/
			inline math::vec3 eulerAnglesNoGimalProtect(math::quat q)
			{
				F32 qxSquare = q.x * q.x;
				F32 qySquare = q.y * q.y;
				F32 qzSquare = q.z * q.z;

				F32 bank = atan2(2.0f * q.x * q.w - 2.0f * q.y * q.z, 1.0f - 2.0f * qxSquare - 2.0f * qzSquare);
				F32 heading = atan2(2.0f * q.y * q.w - 2.0f * q.x * q.z, 1.0f - 2.0f * qySquare - 2.0f * qzSquare);
				F32 attitude = asin(2.0f * q.x * q.y + 2.0f * q.z * q.w);

				return math::vec3(bank, heading, attitude);
			}
	}
}