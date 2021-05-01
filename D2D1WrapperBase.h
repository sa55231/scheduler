#pragma once
#include <utility>

template<class T>
class CD2D1WrapperBase
{
public:
	CD2D1WrapperBase() :member(nullptr)
	{}
	virtual ~CD2D1WrapperBase()
	{
		release();
	}
	CD2D1WrapperBase(CD2D1WrapperBase&& other)
	{
		swap(other);
	}
	CD2D1WrapperBase& operator=(CD2D1WrapperBase&& other)
	{
		swap(other);
		return *this;
	}

	CD2D1WrapperBase(const CD2D1WrapperBase& other) = delete ;
	CD2D1WrapperBase& operator=(const CD2D1WrapperBase& other) const = delete ;

	T* get() const
	{
		return member;
	}
	T* operator->() const
	{
		return member;
	}
	explicit operator bool() const
	{
		return member != nullptr;
	}
	void swap(CD2D1WrapperBase& other) 
	{			
		release();
		std::swap(this->member, other.member);
		other.member = nullptr;
	}
	void release()
	{
		if (member)
		{
			member->Release();
			member = nullptr;
		}
	}

protected:
	T* member = nullptr;
};