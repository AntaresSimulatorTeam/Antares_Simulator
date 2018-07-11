/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "variant.h"

#define YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(TYPE)  \
	template<> struct Variant::SupportedType<TYPE> \
	{ \
		enum \
		{ \
			yes = 1, no = 0 \
		}; \
		\
		typedef TYPE OperatorType; \
	};



namespace Yuni
{

	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(char*);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(const char*);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(char);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(bool);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(String);

	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(float);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(double);


	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(sint32);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(sint64);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(uint32);
	YUNI_VARIANT_DEFINE_SUPPORTED_TYPE(uint64);




	template<class T>
	inline Variant Variant::New<T>::Instance()
	{
		assert(false and "Variant<T>::New : not implemented");
		return Variant();
	}


	template<> inline Variant Variant::New<char*>::Instance()       { return Variant(String()); }
	template<> inline Variant Variant::New<const char*>::Instance() { return Variant(String()); }
	template<> inline Variant Variant::New<String>::Instance()      { return Variant(String()); }

	template<> inline Variant Variant::New<float>::Instance()       { return Variant(double()); }
	template<> inline Variant Variant::New<double>::Instance()      { return Variant(double()); }

	template<> inline Variant Variant::New<uint32>::Instance()      { return Variant(uint32()); }
	template<> inline Variant Variant::New<uint64>::Instance()      { return Variant(uint64()); }
	template<> inline Variant Variant::New<sint32>::Instance()      { return Variant(sint32()); }
	template<> inline Variant Variant::New<sint64>::Instance()      { return Variant(sint64()); }

	template<> inline Variant Variant::New<bool>::Instance()        { return Variant(bool()); }
	template<> inline Variant Variant::New<char>::Instance()        { return Variant(char()); }


	template<class T>
	inline Variant Variant::New<T>::Array()
	{
		// We must specify the type to avoid invalid cast
		Private::Variant::IDataHolder* holder =
			new Private::Variant::ArrayData<typename Static::Remove::Const<T>::Type>();
		return holder;
	}




	inline Variant::Variant()
		: pShareContent(false)
	{}


	inline Variant::Variant(const NullPtr&)
		: pShareContent(false)
	{}


	inline Variant::Variant(const Variant& rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	template<uint ChunkT, bool ExpT>
	inline Variant::Variant(const CString<ChunkT, ExpT>& rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(const char* rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(bool rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(char rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}



	inline Variant::Variant(sint32 rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(sint64 rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(uint32 rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(uint64 rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(float rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}


	inline Variant::Variant(double rhs)
		: pShareContent(false)
	{
		assign(rhs);
	}



	inline void Variant::assign(const char* rhs)
	{
		assign(String(rhs));
	}


	template<class T>
	inline void Variant::add(const T& value)
	{
		if (!pData)
		{
			assign(value);
		}
		else
		{
			deepCopyIfNonUnique();
			pData->add(value);
		}
	}


	template<class T>
	inline void Variant::mult(const T& value)
	{
		if (!(!pData))
		{
			deepCopyIfNonUnique();
			pData->mult(value);
		}
	}


	inline void Variant::mult(const char*)
	{
		// do nothing
	}


	template<class T>
	inline void Variant::sub(const T& value)
	{
		if (!(!pData))
		{
			deepCopyIfNonUnique();
			pData->sub(value);
		}
		else
		{
			assign(value);
			mult(-1);
		}
	}


	template<class T>
	inline void Variant::div(const T& value)
	{
		if (!(!pData))
		{
			deepCopyIfNonUnique();
			pData->div(value);
		}
	}


	inline void Variant::div(const char*)
	{
		// do nothing
	}


	template<class T>
	inline T Variant::to() const
	{
		return (!(!pData)) ? pData->to<T>() : T();
	}


	template<class T>
	inline bool Variant::to(T& out) const
	{
		if (!(!pData))
			return pData->to(out);
		out = T();
		return false;
	}


	inline bool Variant::empty() const
	{
		return not pData;
	}


	inline bool Variant::isnil() const
	{
		return not pData;
	}

	inline bool Variant::operator ! () const
	{
		return isnil();
	}


	inline Variant& Variant::operator = (const Variant& rhs)
	{
		assign(rhs);
		return *this;
	}


	inline Variant& Variant::operator = (const NullPtr&)
	{
		pData = nullptr;
		pShareContent = false;
		return *this;
	}


	inline Variant&  Variant::operator = (Private::Variant::IDataHolder* rhs)
	{
		assign(Variant(rhs));
		return *this;
	}


	template<class T>
	inline Variant& Variant::operator = (const T& rhs)
	{
		assign(rhs);
		return *this;
	}


	template<class T>
	inline Variant& Variant::operator += (const T& rhs)
	{
		add(rhs);
		return *this;
	}


	template<class T>
	inline Variant& Variant::operator -= (const T& rhs)
	{
		sub(rhs);
		return *this;
	}


	template<class T>
	inline Variant& Variant::operator *= (const T& rhs)
	{
		mult(rhs);
		return *this;
	}


	template<class T>
	inline Variant& Variant::operator /= (const T& rhs)
	{
		div(rhs);
		return *this;
	}


	template<class U>
	inline bool Variant::operator == (const U& rhs) const
	{
		return !pData ? false : pData->isEquals(rhs);
	}

	template<class U>
	inline bool Variant::operator != (const U& rhs) const
	{
		return not this->operator == (rhs);
	}


	inline void Variant::shareContentFrom(const NullPtr&)
	{
		pShareContent = false;
		pData = nullptr;
	}


	inline void Variant::deepCopyIfNonUnique()
	{
		// pValue must not null
		if (/*!(!pValue) &&*/ not pShareContent) //  and not pData->unique())
		{
			Private::Variant::IDataHolder* pointer = Private::Variant::IDataHolder::Ptr::WeakPointer(pData);
			pData = pointer->clone();
		}
	}




} // namespace Yuni





inline Yuni::Variant operator + (const Yuni::Variant& lhs, const Yuni::Variant& rhs)
{
	return Yuni::Variant(lhs) += rhs;
}

inline Yuni::Variant operator - (const Yuni::Variant& lhs, const Yuni::Variant& rhs)
{
	return Yuni::Variant(lhs) -= rhs;
}

inline Yuni::Variant operator * (const Yuni::Variant& lhs, const Yuni::Variant& rhs)
{
	return Yuni::Variant(lhs) *= rhs;
}

inline Yuni::Variant operator / (const Yuni::Variant& lhs, const Yuni::Variant& rhs)
{
	return Yuni::Variant(lhs) /= rhs;
}


# define YUNI_VARIANT_OPERATOR_TYPE \
	const \
	typename Yuni::Static::Remove::Volatile< \
		typename Yuni::Static::Remove::Const< \
			typename Yuni::Static::Remove::RefOnly<T>::Type >::Type>::Type \
	&  \


template<class T>
inline Yuni::Variant operator + (const Yuni::Variant& lhs, YUNI_VARIANT_OPERATOR_TYPE rhs)
{
	return (Yuni::Variant(lhs) += rhs);
}

template<class T>
inline Yuni::Variant operator + (YUNI_VARIANT_OPERATOR_TYPE lhs, const Yuni::Variant& rhs)
{
	return (Yuni::Variant(lhs) += rhs);
}

template<class T>
inline Yuni::Variant operator - (const Yuni::Variant& lhs, YUNI_VARIANT_OPERATOR_TYPE rhs)
{
	return (Yuni::Variant(lhs) -= rhs);
}

template<class T>
inline Yuni::Variant operator - (YUNI_VARIANT_OPERATOR_TYPE lhs, const Yuni::Variant& rhs)
{
	return (Yuni::Variant(lhs) -= rhs);
}

template<class T>
inline Yuni::Variant operator * (const Yuni::Variant& lhs, YUNI_VARIANT_OPERATOR_TYPE rhs)
{
	return (Yuni::Variant(lhs) *= rhs);
}

template<class T>
inline Yuni::Variant operator * (YUNI_VARIANT_OPERATOR_TYPE lhs, const Yuni::Variant& rhs)
{
	return (Yuni::Variant(lhs) *= rhs);
}

template<class T>
inline Yuni::Variant operator / (const Yuni::Variant& lhs, YUNI_VARIANT_OPERATOR_TYPE rhs)
{
	return (Yuni::Variant(lhs) /= rhs);
}

template<class T>
inline Yuni::Variant operator / (YUNI_VARIANT_OPERATOR_TYPE lhs, const Yuni::Variant& rhs)
{
	return (Yuni::Variant(lhs) /= rhs);
}




inline std::ostream& operator << (std::ostream& out, const Yuni::Variant& rhs)
{
	out << rhs.to<Yuni::String>();
	return out;
}



#undef YUNI_VARIANT_OPERATOR_TYPE
#undef YUNI_VARIANT_DEFINE_SUPPORTED_TYPE
