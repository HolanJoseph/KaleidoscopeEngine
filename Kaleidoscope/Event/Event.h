#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

namespace kaleidoscope
{
	class VariantType
	{
	public:
		enum ArgType
		{
			I8_ARG,
			I16_ARG,
			I32_ARG,
			I64_ARG,

			U8_ARG,
			U16_ARG,
			U32_ARG,
			U64_ARG,

			F32_ARG,
			F64_ARG,

			BOOL_ARG,
			STRINGID_ARG,

			VEC3_ARG,
			VEC4_ARG,
			QUAT_ARG,

			// Possibility of handles being added.

			COUNT_ARG
		};

		ArgType type;

		union
		{
			I8  asI8;
			I16 asI16;
			I32 asI32;
			I64 asI64;

			U8  asU8;
			U16 asU16;
			U32 asU32;
			U64 asU64;

			F32 asF32;
			F64 asF64;

			bool asBool;
			StringID asStringID;

			F32 asVec3[3];
			F32 asVec4[4];
			F32 asQuat[4];

		};
	};

	class Event
	{
	private:
		static const U32 MAXARGS = 8;
		StringID mType;
		VariantType mArgs[MAXARGS];

	public:
		Event(){};
		~Event(){};

		void setEventType(StringID eventName){ mType = eventName; }; // Set the event name. This name corresponds to the function to call on the GOs Scripts.
		StringID getEventType() const { return mType; };
		U32 maxSize() const { return MAXARGS; }; // Return the maximum number of arguments an event can possibly have.


		/*
		* void kaleidoscope::Event::addX(U32 index, X x)
		*
		* In: U32 index : What argument to change.
		* In: X x : The new value to set the argument to.
		* Out: void :
		* 
		* Applies to all functions of the form above.
		*/
		void addF32(U32 index, F32 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::F32_ARG;
				mArgs[index].asF32 = f;
			}
		};


		/*
		* X kaleidoscope::Event::getX(U32 index)
		*
		* In: U32 index : What argument to get.
		* Out: X : The desired argument as X
		*
		* If the argument at index is not of type X then some default value is returned.
		*
		* Applies to all functions of the form above.
		*/
		F32 getF32(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::F32_ARG)
			{
				return mArgs[index].asF32;
			}
			else
			{
				return 0.0f;
			}
		};

		void addF64(U32 index, F64 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::F64_ARG;
				mArgs[index].asF64 = f;
			}
		};

		F64 getF64(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::F64_ARG)
			{
				return mArgs[index].asF64;
			}
			else
			{
				return 0.0;
			}
		};

		void addU8(U32 index, U8 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::U8_ARG;
				mArgs[index].asU8 = f;
			}
		};

		U8 getU8(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::U8_ARG)
			{
				return mArgs[index].asU8;
			}
			else
			{
				return 0;
			}
		};

		void addU16(U32 index, U16 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::U16_ARG;
				mArgs[index].asU16 = f;
			}
		};

		U16 getU16(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::U16_ARG)
			{
				return mArgs[index].asU16;
			}
			else
			{
				return 0;
			}
		};

		void addU32(U32 index, U32 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::U32_ARG;
				mArgs[index].asU32 = f;
			}
		};

		U32 getU32(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::U32_ARG)
			{
				return mArgs[index].asU32;
			}
			else
			{
				return 0;
			}
		};

		void addU64(U32 index, U64 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::U64_ARG;
				mArgs[index].asU64 = f;
			}
		};

		U64 getU64(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::U64_ARG)
			{
				return mArgs[index].asU64;
			}
			else
			{
				return 0;
			}
		};

		void addI8(U32 index, I8 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::I8_ARG;
				mArgs[index].asI8 = f;
			}
		};

		I8 getI8(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::I8_ARG)
			{
				return mArgs[index].asI8;
			}
			else
			{
				return 0;
			}
		};

		void addI16(U32 index, I16 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::I16_ARG;
				mArgs[index].asI16 = f;
			}
		};

		I16 getI16(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::I16_ARG)
			{
				return mArgs[index].asI16;
			}
			else
			{
				return 0;
			}
		};

		void addI32(U32 index, I32 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::I32_ARG;
				mArgs[index].asI32 = f;
			}
		};

		I32 getI32(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::I32_ARG)
			{
				return mArgs[index].asI32;
			}
			else
			{
				return 0;
			}
		};

		void addI64(U32 index, I64 f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::I64_ARG;
				mArgs[index].asI64 = f;
			}
		};

		I64 getI64(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::I64_ARG)
			{
				return mArgs[index].asI64;
			}
			else
			{
				return 0;
			}
		};

		void addBool(U32 index, bool f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::BOOL_ARG;
				mArgs[index].asBool = f;
			}
		};

		bool getBool(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::BOOL_ARG)
			{
				return mArgs[index].asBool;
			}
			else
			{
				return false;
			}
		};

		void addStringID(U32 index, StringID f)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::STRINGID_ARG;
				mArgs[index].asStringID = f;
			}
		};

		StringID getStringID(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::STRINGID_ARG)
			{
				return mArgs[index].asStringID;
			}
			else
			{
				return 0;
			}
		};

		void addVec3(U32 index, kaleidoscope::math::vec3 v)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::VEC3_ARG;
				mArgs[index].asVec3[0] = v.x;
				mArgs[index].asVec3[1] = v.y;
				mArgs[index].asVec3[2] = v.z;
			}
		};

		kaleidoscope::math::vec3 getVec3(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::VEC3_ARG)
			{
				return kaleidoscope::math::vec3(mArgs[index].asVec3[0], mArgs[index].asVec3[1], mArgs[index].asVec3[2]);
			}
			else
			{
				return kaleidoscope::math::vec3();
			}
		};

		void addVec4(U32 index, kaleidoscope::math::vec4 v)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::VEC4_ARG;
				mArgs[index].asVec4[0] = v.x;
				mArgs[index].asVec4[1] = v.y;
				mArgs[index].asVec4[2] = v.z;
				mArgs[index].asVec4[3] = v.w;
			}
		};

		kaleidoscope::math::vec4 getVec4(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::VEC4_ARG)
			{
				return kaleidoscope::math::vec4(mArgs[index].asVec4[0], mArgs[index].asVec4[1], mArgs[index].asVec4[2], mArgs[index].asVec4[3]);
			}
			else
			{
				return kaleidoscope::math::vec4();
			}
		};

		void addQuat(U32 index, kaleidoscope::math::quat v)
		{
			if (index >= 0 && index < MAXARGS)
			{
				mArgs[index].type = VariantType::QUAT_ARG;
				mArgs[index].asQuat[0] = v.w;
				mArgs[index].asQuat[1] = v.x;
				mArgs[index].asQuat[2] = v.y;
				mArgs[index].asQuat[3] = v.z;
			}
		};

		kaleidoscope::math::quat getQuat(U32 index) const
		{
			if (index >= 0 && index < MAXARGS && mArgs[index].type == VariantType::QUAT_ARG)
			{
				return kaleidoscope::math::quat(mArgs[index].asVec4[0], mArgs[index].asVec4[1], mArgs[index].asVec4[2], mArgs[index].asVec4[3]);
			}
			else
			{
				return kaleidoscope::math::quat();
			}
		};


	};
}