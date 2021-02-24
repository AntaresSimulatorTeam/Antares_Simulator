/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_TOOLBOX_CLIPBOARD_CLIPBOARD_H__
#define __ANTARES_TOOLBOX_CLIPBOARD_CLIPBOARD_H__

#include <antares/wx-wrapper.h>
#include <antares/array/matrix.h>
#include <vector>
#include "../components/datagrid/gridhelper.h"

namespace Antares
{
namespace Toolbox
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
        typedef Yuni::SmartPtr<Item> Ptr;

    public:
        Item(const Type t, Yuni::String::Ptr d) : type(t), data(d)
        {
        }
        const Type type;
        Yuni::String::Ptr data;
    };

    typedef std::vector<Item::Ptr> List;
    List pList;

}; // class Clipboard

} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_CLIPBOARD_CLIPBOARD_H__
