#pragma once

#include <xscModule.h>

namespace xsc {

class Object;

class XSC_API PointerBase
{
protected:
	static std::unordered_map<Object*, int> msTable;
};

template <typename T>
class Pointer : public PointerBase
{
public:
    Pointer(T* object = nullptr)
	{
		mObject = object;
		if (mObject)
		{
			std::unordered_map<Object*, int>::iterator itr = msTable.find(mObject);
			if (itr != msTable.end())
			{
				++itr->second;
			}
			else
			{
				msTable[mObject] = 1;
			}
		}
	}

	Pointer(const Pointer& pointer)
	{
		mObject = pointer.mObject;
		if (mObject)
		{
			std::unordered_map<Object*, int>::iterator itr = msTable.find(mObject);
			if (itr != msTable.end())
			{
				++itr->second;
			}
		}
	}

    ~Pointer()
	{
		if (mObject)
		{
			std::unordered_map<Object*, int>::iterator itr = msTable.find(mObject);
			if (itr != msTable.end())
			{
				if (--itr->second == 0)
				{
					msTable.erase(itr);
					delete mObject;
				}
			}
		}
	}

	inline T* Get() const
	{
		return mObject;
	}

    inline T* operator->() const
	{
		return mObject;
	}

	inline operator T*() const
	{
		return mObject;
	}

    Pointer& operator=(T* object)
	{
		if (mObject != object)
		{
			std::unordered_map<Object*, int>::iterator itr;

			if (object)
			{
				itr = msTable.find(object);
				if (itr != msTable.end())
				{
					++itr->second;
				}
				else
				{
					msTable[object] = 1;
				}
			}

			if (mObject)
			{
				itr = msTable.find(mObject);
				if (itr != msTable.end())
				{
					if (--itr->second == 0)
					{
						msTable.erase(itr);
						delete mObject;
					}
				}
			}

			mObject = object;
		}
		return (*this);
	}

    Pointer& operator=(const Pointer& pointer)
	{
		if (mObject != pointer.mObject)
		{
			std::unordered_map<Object*, int>::iterator itr;

			if (pointer.mObject)
			{
				itr = msTable.find(pointer.mObject);
				if (itr != msTable.end())
				{
					++itr->second;
				}
			}

			if (mObject)
			{
				itr = msTable.find(mObject);
				if (itr != msTable.end())
				{
					if (--itr->second == 0)
					{
						msTable.erase(itr);
						delete mObject;
					}
				}
			}

			mObject = pointer.mObject;
		}
		return (*this);
	}

    inline bool operator==(T* object) const
	{
		return (mObject == object);
	}

	inline bool operator==(const Pointer& pointer) const
	{
		return (mObject == pointer.mObject);
	}

    inline bool operator!=(T* object) const
	{
		return (mObject != object);
	}
	
    inline bool operator!=(const Pointer& pointer) const
	{
		return (mObject != pointer.mObject);
	}

protected:
    T* mObject;
};

} // namespace xsc
