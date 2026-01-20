// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_CLIPBOARD_CLIPBOARD_H__
#define __ANTARES_TOOLBOX_CLIPBOARD_CLIPBOARD_H__

#include <antares/array/matrix.h>
#include <memory>
#include <vector>
#include "../components/datagrid/gridhelper.h"


namespace Antares::Toolbox
{
class Clipboard final
{
public:
    enum Type
    {
        typeText,
        typeHTML
    };

    static void GetFromClipboard(Yuni::String& out);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Clipboard();
    //! Destructor
    ~Clipboard();
    //@}

    void clear();
    void add(const Matrix<>& m);
    void add(Antares::Component::Datagrid::VGridHelper* m,
             uint offsetX = 0,
             uint offsetY = 0,
             uint offsetX2 = (uint)-1,
             uint offsetY2 = (uint)-1,
             bool withStyle = true,
             bool withHeaders = true);

    void add(const Yuni::String& text);
    void add(const Yuni::String::Ptr& text);

    void copy();

private:
    class Item final
    {
    public:
        using Ptr = std::shared_ptr<Item>;

    public:
        Item(const Type t, Yuni::String::Ptr d) : type(t), data(d)
        {
        }
        const Type type;
        Yuni::String::Ptr data;
    };

    using List = std::vector<Item::Ptr>;
    List pList;

}; // class Clipboard

} // namespace Antares::Toolbox


#endif // __ANTARES_TOOLBOX_CLIPBOARD_CLIPBOARD_H__
