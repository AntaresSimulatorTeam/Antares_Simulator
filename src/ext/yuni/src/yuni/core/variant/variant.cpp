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
#include "variant.h"


namespace Yuni
{

	Variant::Variant(const Private::Variant::IDataHolder* rhs, bool ref)
		: pData(const_cast<Private::Variant::IDataHolder*>(rhs))
		, pShareContent(ref)
	{
	}


	Variant::Variant(Private::Variant::IDataHolder* rhs, bool ref)
		: pData(rhs)
		, pShareContent(ref)
	{
	}


	VariantInnerType  Variant::type() const
	{
		return (not pData) ? variantTNil : pData->type();
	}


	void Variant::assign(uint32 rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<uint32>(rhs);
	}


	void Variant::assign(sint32 rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<sint32>(rhs);
	}


	void Variant::assign(uint64 rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<uint64>(rhs);
	}


	void Variant::assign(sint64 rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<sint64>(rhs);
	}


	void Variant::assign(char rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<char>(rhs);
	}


	void Variant::assign(bool rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<bool>(rhs);
	}


	void Variant::assign(double rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<double>(rhs);
	}


	void Variant::assign(const String& rhs)
	{
		if (pShareContent and !(!pData))
			pData->assign(rhs);
		else
			pData = new Private::Variant::Data<String>(rhs);
	}


	void Variant::assign(const Variant& rhs)
	{
		if (pShareContent and !rhs.isnil())
		{
			pData->loopbackAssign(*rhs.pData);
		}
		else
		{
			// if rhs has a shared content, we must clone it
			if (rhs.pShareContent)
				pData = rhs.pData->clone();
			else
				pData = rhs.pData;
		}
	}


	void Variant::add(const char* value)
	{
		if (!(!pData))
		{
			deepCopyIfNonUnique();
			pData->add(String(value));
		}
		else
			assign(value);
	}


	void Variant::add(const Variant& value)
	{
		if (!(!pData))
		{
			if (!(!value.pData))
			{
				deepCopyIfNonUnique();
				value.pData->loopbackAdd(*pData);
			}
		}
		else
			assign(value);
	}


	void Variant::sub(const char* value)
	{
		if (!(!pData))
		{
			deepCopyIfNonUnique();
			pData->sub(String(value));
		}
		else
		{
			assign(value);
			mult(-1);
		}
	}


	void Variant::sub(const Variant& value)
	{
		if (!(!pData) and !(!value.pData))
		{
			deepCopyIfNonUnique();
			value.pData->loopbackSub(*pData);
		}
	}


	void Variant::mult(const Variant& value)
	{
		if (!(!pData) and !(!value.pData))
		{
			deepCopyIfNonUnique();
			value.pData->loopbackMultiply(*pData);
		}
	}


	void Variant::div(const Variant& value)
	{
		if (!(!pData) and !(!value.pData))
		{
			deepCopyIfNonUnique();
			value.pData->loopbackDiv(*pData);
		}
	}


	void Variant::clear()
	{
		pData = nullptr;
		pShareContent = false;
	}


	bool Variant::operator == (const Variant& rhs) const
	{
		if (not pData)
			return rhs.isnil();
		if (rhs.isnil())
			return false;

		return pData->loopbackIsEquals(*rhs.pData);
	}


	Variant Variant::operator [] (uint index)
	{
		if (!pData)
		{
			return nullptr;
		}
		else
		{
			deepCopyIfNonUnique();
			return Variant(pData->at(index), true);
		}
	}


	const Variant Variant::operator [] (uint index) const
	{
		return not pData ? nullptr : Variant(pData->at(index), true);
	}


	Variant&  Variant::operator = (const Private::Variant::IDataHolder* rhs)
	{
		pData = const_cast<Private::Variant::IDataHolder*>(rhs);
		return *this;
	}


	void Variant::shareContentFrom(const Variant& rhs)
	{
		if (rhs.isnil())
		{
			pShareContent = false;
			pData = nullptr;
		}
		else
		{
			pData = rhs.pData;
			pShareContent = true;
		}
	}


	Variant Variant::operator () (const String& method)
	{
		return (!pData) ? nullptr : pData->invoke(method);
	}


	Variant Variant::operator () (const String& method, const Variant& a1)
	{
		IDataHolder* p1 = IDataHolder::Ptr::WeakPointer(a1.pData);
		return (!pData) ? nullptr : pData->invoke(method, p1);
	}


	Variant Variant::operator () (const String& method, const Variant& a1, const Variant& a2)
	{
		IDataHolder* p1 = IDataHolder::Ptr::WeakPointer(a1.pData);
		IDataHolder* p2 = IDataHolder::Ptr::WeakPointer(a2.pData);
		return (!pData) ? nullptr : pData->invoke(method, p1, p2);
	}


	Variant Variant::operator () (const String& method, const Variant& a1, const Variant& a2, const Variant& a3)
	{
		IDataHolder* p1 = IDataHolder::Ptr::WeakPointer(a1.pData);
		IDataHolder* p2 = IDataHolder::Ptr::WeakPointer(a2.pData);
		IDataHolder* p3 = IDataHolder::Ptr::WeakPointer(a3.pData);
		return (!pData) ? nullptr : pData->invoke(method, p1, p2, p3);
	}


	Variant Variant::operator () (const String& method, const Variant& a1, const Variant& a2, const Variant& a3, const Variant& a4)
	{
		IDataHolder* p1 = IDataHolder::Ptr::WeakPointer(a1.pData);
		IDataHolder* p2 = IDataHolder::Ptr::WeakPointer(a2.pData);
		IDataHolder* p3 = IDataHolder::Ptr::WeakPointer(a3.pData);
		IDataHolder* p4 = IDataHolder::Ptr::WeakPointer(a4.pData);
		return (!pData) ? nullptr : pData->invoke(method, p1, p2, p3, p4);
	}



} // namespace Yuni
