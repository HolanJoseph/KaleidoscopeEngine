#pragma once
#define _SCL_SECURE_NO_WARNINGS

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <queue>
#include <boost/unordered_map.hpp>

namespace kaleidoscope
{
	class TagPool
	{
	public:
		typedef	boost::unordered_map<StringID, I8>::const_iterator const_iterator;

		TagPool() : mNumTags(64)
		{
			for (I8 i = 0; i < mNumTags; i++)
			{
				mTagOffsetReservoir.push(i);
			}
			mTags = boost::unordered_map<StringID, I8>();
		};

		TagPool(I8 numTags) : mNumTags(numTags)
		{
			for (I8 i = 0; i < mNumTags; i++)
			{
				mTagOffsetReservoir.push(i);
			}
			mTags = boost::unordered_map<StringID, I8>();
		};

		~TagPool(){};

		bool registerTag(const StringID tag)
		{
			if (!mTagOffsetReservoir.empty())
			{
				if (mTags.count(tag))
				{
					return false;
				}
				else
				{
					mTags[tag] = mTagOffsetReservoir.front();
					mTagOffsetReservoir.pop();
					return true;
				}
			}
			else
			{
				return false;
			}
		};

		bool RemoveTag(const StringID tag)
		{
			if (!mTagOffsetReservoir.empty())
			{
				if (mTags.count(tag))
				{
					mTags.erase(tag);
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		
		I8 operator[](const StringID tag)
		{


			if (mTags.count(tag))
			{
				return mTags[tag];
			}
			else
			{
				return -1;
			}
		};

		const_iterator cbegin() const
		{
			return mTags.cbegin();
		}

		const_iterator cend() const
		{
			return mTags.cend();
		}

		



	private:
		I8 mNumTags;
		boost::unordered_map<StringID, I8> mTags;
		std::queue<I8> mTagOffsetReservoir;

	};
}