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



namespace Yuni
{
namespace Private
{
namespace Variant
{

	/*!
	** \brief Concrete variant data container
	*/
	template<class T>
	class ArrayData final : public IDataHolder
	{
	public:
		ArrayData() {}
		//! Constructor from the variable type
		ArrayData(const ArrayData<T>& rhs) :
			pValue(rhs.pValue)
		{}

		//! Destructor
		virtual ~ArrayData() {}

	protected:
		virtual bool convertUsing(IDataConverter& cvtr) const override
		{ return cvtr.convertFrom(pValue); }

		virtual IDataHolder* clone() const override { return new ArrayData<T>(*this); }

		virtual Yuni::VariantInnerType type() const override {return Yuni::variantTArray;}

		virtual void clear() override { pValue.clear(); }

		virtual void assignList(const IDataHolder::Vector& n) override
		{
			pValue.clear();
			pValue.reserve(n.size());
			for (uint i = 0; i != n.size(); ++i)
				pValue.push_back(n[i]);
		}
		virtual void assign(uint32) override {}
		virtual void assign(sint32) override {}
		virtual void assign(uint64) override {}
		virtual void assign(sint64) override {}
		virtual void assign(double) override {}
		virtual void assign(const String&) override {}
		virtual void assign(bool) override {}
		virtual void assign(char) override {}

		virtual void addList(const IDataHolder::Vector& n) override
		{
			pValue.reserve(pValue.size() + n.size());
			for (uint i = 0; i != n.size(); ++i)
				pValue.push_back(n[i]);
		}
		virtual void add(uint32 n) override { pValue.push_back(new Data<T>(n)); }
		virtual void add(sint32 n) override { pValue.push_back(new Data<T>(n)); }
		virtual void add(uint64 n) override { pValue.push_back(new Data<T>(n)); }
		virtual void add(sint64 n) override { pValue.push_back(new Data<T>(n)); }
		virtual void add(double n) override { pValue.push_back(new Data<T>(n)); }
		virtual void add(const String& n) override { pValue.push_back(new Data<T>(n.to<T>())); }
		virtual void add(bool n) { pValue.push_back(new Data<T>(n)); }
		virtual void add(char n) { pValue.push_back(new Data<T>(n)); }

		virtual void sub(uint32) override {}
		virtual void sub(sint32) override {}
		virtual void sub(uint64) override {}
		virtual void sub(sint64) override {}
		virtual void sub(double) override {}
		virtual void sub(const String&) override {}
		virtual void sub(bool) override {}
		virtual void sub(char) override {}

		virtual void mult(uint32) override {}
		virtual void mult(sint32) override {}
		virtual void mult(uint64) override {}
		virtual void mult(sint64) override {}
		virtual void mult(double) override {}
		virtual void mult(bool) override {}
		virtual void mult(char) override {}
		virtual void mult(const String&) override {}

		virtual void div(uint32) override {}
		virtual void div(sint32) override {}
		virtual void div(uint64) override {}
		virtual void div(sint64) override {}
		virtual void div(double) override {}
		virtual void div(bool) override {}
		virtual void div(char) override {}
		virtual void div(const String&) override {}

		virtual bool isEquals(uint32) const override { return false; }
		virtual bool isEquals(sint32) const override { return false; }
		virtual bool isEquals(uint64) const override { return false; }
		virtual bool isEquals(sint64) const override { return false; }
		virtual bool isEquals(double) const override { return false; }
		virtual bool isEquals(bool) const override { return false; }
		virtual bool isEquals(char) const override { return false; }
		virtual bool isEquals(const String&) const override { return false; }

		//! Method invokation, with no parameter
		virtual IDataHolder* invoke(const String& name) override
		{
			if (name == "size")
			{
				return new Data<uint>(pValue.size());
			}
			if (name == "clear")
			{
				pValue.clear();
				return nullptr;
			}
			return nullptr;
		}
		//! Method invokation, with 1 parameter
		virtual IDataHolder* invoke(const String& name, IDataHolder* a1) override
		{
			if (name == "add" or name == "append")
			{
				if (a1)
					pValue.push_back(a1);
				return nullptr;
			}
			return nullptr;
		}

		virtual IDataHolder* at(uint index) override
		{
			return (index < pValue.size())
				? IDataHolder::Ptr::WeakPointer(pValue[index])
				: nullptr;
		}
		virtual const IDataHolder* at(uint index) const override
		{
			return (index < pValue.size())
				? IDataHolder::Ptr::WeakPointer(pValue[index])
				: nullptr;
		}

		virtual void loopbackAssign(IDataHolder& dataholder) const override { dataholder.assignList(pValue); }
		virtual void loopbackAdd(IDataHolder& dataholder) const override { dataholder.addList(pValue); }
		virtual void loopbackMultiply(IDataHolder&) const override {}
		virtual void loopbackSub(IDataHolder&) const override {}
		virtual void loopbackDiv(IDataHolder&) const override {}
		virtual bool loopbackIsEquals(IDataHolder&) const override { assert(false and "not implemented"); return false; };

	private:
		//! The real data element.
		IDataHolder::Vector  pValue;

	}; // class Data





} // namespace Variant
} // namespace Private
} // namespace Yuni
