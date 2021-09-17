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

#pragma once

#include <antares/wx-wrapper.h>
#include "../../renderer.h"
#include "../../../../input/area.h"
#include "../../../../../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
    // Check on value ...

    struct NoCheck
    {
        template<class T>
        static bool Validate(const T&)
        {
            return true;
        }
    };

    struct CheckMinUpDownTime
    {
        static bool Validate(uint f)
        {
            return (f == 1) || (f == 24) || (f == 168);
        }
    };

    struct CheckUnitCount
    {
        static bool Validate(uint& f)
        {
            if (f > 100)
                f = 100;
            return true;
        }
    };

    // Refresh after update ...
    struct RefeshInspector
    {
        static void refresh()
        {
            OnInspectorRefresh(nullptr);
        }
    };

    struct RefeshInspectorAndMarkAsModified
    {
        static void refresh()
        {
            MarkTheStudyAsModified();
            OnInspectorRefresh(nullptr);
        }
    };


    // Update value ...

    template<class T, class CheckT, class RefreshT>
    bool Update(T& value, const String& str)
    {
        T v;
        if (str.to(v))
        {
            if (not Math::Equals<T>(value, v) and CheckT::Validate(v))
            {
                value = v;
                RefreshT::refresh();
                return true;
            }
        }
        return false;
    }


    // Single area common cluster summary renderer

    class CommonClusterSummarySingleArea : public IRenderer
    {
    public:
        CommonClusterSummarySingleArea(wxWindow* control, Toolbox::InputSelector::Area* notifier);

        virtual ~CommonClusterSummarySingleArea();

        virtual int width() const = 0;
        virtual int height() const = 0;

        virtual wxString columnCaption(int colIndx) const = 0;

        virtual wxString rowCaption(int rowIndx) const = 0;

        virtual wxString cellValue(int x, int y) const = 0;

        virtual double cellNumericValue(int x, int y) const = 0;

        virtual bool cellValue(int x, int y, const Yuni::String& v) = 0;

        virtual void resetColors(int, int, wxColour&, wxColour&) const
        { 
            // Does nothing
        }

        virtual IRenderer::CellStyle cellStyle(int col, int row) const;

        virtual uint maxWidthResize() const
        {
            return 0;
        }
        virtual uint maxHeightResize() const
        {
            return 0;
        }

        virtual bool valid() const
        {
            return (pArea != NULL);
        }

    protected:
        virtual void onAreaChanged(Antares::Data::Area* area);
        virtual void onStudyClosed() override;
        void onStudyAreaDelete(Antares::Data::Area* area);

    protected:
        Antares::Data::Area* pArea;
        wxWindow* pControl;
        Toolbox::InputSelector::Area* pAreaNotifier;

    }; // class CommonClusterSummarySingleArea

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
