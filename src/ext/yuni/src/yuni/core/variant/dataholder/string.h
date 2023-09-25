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
** \brief Concrete variant data container (string)
*/
template<>
class Data<String> final : public IDataHolder
{
public:
    //! Constructor from the variable type
    explicit Data(const String& src) : pValue(src)
    {
    }

    //! Destructor
    virtual ~Data()
    {
    }

protected:
    virtual bool convertUsing(IDataConverter& cvtr) const override
    {
        return cvtr.convertFrom(pValue);
    }

    virtual IDataHolder* clone() const override
    {
        return new Data<String>(pValue);
    }

    virtual Yuni::VariantInnerType type() const override
    {
        return Yuni::variantTString;
    }

    virtual void clear() override
    {
        pValue.clear();
    }

    virtual IDataHolder* at(uint index) override
    {
        return index < pValue.size() ? new Data<char>(pValue[index]) : NULL;
    }
    virtual const IDataHolder* at(uint index) const override
    {
        return index < pValue.size() ? new Data<char>(pValue[index]) : NULL;
    }

    virtual void assignList(const IDataHolder::Vector&) override
    {
    }
    virtual void assign(uint32_t n) override
    {
        pValue = n;
    }
    virtual void assign(int32_t n) override
    {
        pValue = n;
    }
    virtual void assign(uint64_t n) override
    {
        pValue = n;
    }
    virtual void assign(int64_t n) override
    {
        pValue = n;
    }
    virtual void assign(double n) override
    {
        pValue = n;
    }
    virtual void assign(const String& n) override
    {
        pValue = n;
    }
    virtual void assign(bool n) override
    {
        pValue = n;
    }
    virtual void assign(char n) override
    {
        pValue = n;
    }

    virtual void addList(const IDataHolder::Vector&) override
    {
    }
    virtual void add(uint32_t n) override
    {
        pValue += n;
    }
    virtual void add(int32_t n) override
    {
        pValue += n;
    }
    virtual void add(uint64_t n) override
    {
        pValue += n;
    }
    virtual void add(int64_t n) override
    {
        pValue += n;
    }
    virtual void add(double n) override
    {
        pValue += n;
    }
    virtual void add(const String& n) override
    {
        pValue += n;
    }
    virtual void add(bool n) override
    {
        pValue += n;
    }
    virtual void add(char n) override
    {
        pValue += n;
    }

    virtual void sub(uint32_t) override
    {
    }
    virtual void sub(int32_t) override
    {
    }
    virtual void sub(uint64_t) override
    {
    }
    virtual void sub(int64_t) override
    {
    }
    virtual void sub(double) override
    {
    }
    virtual void sub(const String& n) override
    {
        pValue.replace(n, "");
    }
    virtual void sub(bool) override
    {
    }
    virtual void sub(char) override
    {
    }

    virtual void mult(uint32_t) override
    {
    }
    virtual void mult(int32_t) override
    {
    }
    virtual void mult(uint64_t) override
    {
    }
    virtual void mult(int64_t) override
    {
    }
    virtual void mult(double) override
    {
    }
    virtual void mult(bool) override
    { /* do nothing*/
    }
    virtual void mult(char) override
    {
    }
    virtual void mult(const String&) override
    {
    }

    virtual void div(uint32_t) override
    {
    }
    virtual void div(int32_t) override
    {
    }
    virtual void div(uint64_t) override
    {
    }
    virtual void div(int64_t) override
    {
    }
    virtual void div(double) override
    {
    }
    virtual void div(bool) override
    {
    }
    virtual void div(char) override
    {
    }
    virtual void div(const String&) override
    {
    }

    virtual bool isEquals(uint32_t n) const override
    {
        return pValue.to<uint32_t>() == n;
    }
    virtual bool isEquals(int32_t n) const override
    {
        return pValue.to<int32_t>() == n;
    }
    virtual bool isEquals(uint64_t n) const override
    {
        return pValue.to<uint64_t>() == n;
    }
    virtual bool isEquals(int64_t n) const override
    {
        return pValue.to<int64_t>() == n;
    }
    virtual bool isEquals(double n) const override
    {
        return Math::Equals(pValue.to<double>(), n);
    }
    virtual bool isEquals(bool n) const override
    {
        return n == pValue.to<bool>();
    }
    virtual bool isEquals(char n) const override
    {
        return pValue.size() == 1 and n == pValue[0];
    }
    virtual bool isEquals(const String& n) const override
    {
        return pValue == n;
    }

    virtual void loopbackAssign(IDataHolder& dataholder) const override
    {
        dataholder.assign(pValue);
    }
    virtual void loopbackAdd(IDataHolder& dataholder) const override
    {
        dataholder.add(pValue);
    }
    virtual void loopbackMultiply(IDataHolder& dataholder) const override
    {
        dataholder.mult(pValue);
    }
    virtual void loopbackSub(IDataHolder& dataholder) const override
    {
        dataholder.sub(pValue);
    }
    virtual void loopbackDiv(IDataHolder& dataholder) const override
    {
        dataholder.div(pValue);
    }
    virtual bool loopbackIsEquals(IDataHolder& dataholder) const override
    {
        return dataholder.isEquals(pValue);
    };

private:
    //! The real data element.
    String pValue;

}; // class Data<String>

} // namespace Variant
} // namespace Private
} // namespace Yuni
