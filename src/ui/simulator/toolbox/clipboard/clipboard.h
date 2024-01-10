/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
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
