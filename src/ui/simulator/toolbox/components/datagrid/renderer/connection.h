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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_CONNECTION_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_CONNECTION_H__

#include "../../../input/connection.h"
#include "../renderer.h"
#include <antares/date/date.h>
#include "matrix.h"
#include <yuni/core/event.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class Connection : public Renderer::Matrix<>
{
public:
    Connection(wxWindow* parent, Toolbox::InputSelector::Connections* notifier);
    virtual ~Connection();

    virtual int width() const
    {
        return Renderer::Matrix<>::width();
    }
    virtual int height() const
    {
        return Renderer::Matrix<>::height();
    }

    virtual wxString rowCaption(int rowIndx) const;

    wxString cellValue(int x, int y) const override
    {
        return Renderer::Matrix<>::cellValue(x, y);
    }

    virtual double cellNumericValue(int x, int y) const
    {
        return Renderer::Matrix<>::cellNumericValue(x, y);
    }

    void resetColors(int, int, wxColour&, wxColour&) const override
    { /* Do nothing*/
    }

    virtual wxColour horizontalBorderColor(int x, int y) const;

    virtual bool valid() const
    {
        return Renderer::Matrix<>::valid();
    }

    uint maxWidthResize() const override
    {
        return 1000;
    }

    Date::Precision precision() override
    {
        return Date::hourly;
    }

protected:
    wxWindow* pControl;
    bool mUseLoopFlow = false;

private:
    virtual void setMatrix(Data::AreaLink* link) = 0;
    void onConnectionChanged(Data::AreaLink* link);

}; // class Connection

// ===========================
// Parameters grid renderer
// ===========================
class connectionParameters final : public Connection
{
public:
    connectionParameters(wxWindow* parent, Toolbox::InputSelector::Connections* notifier);
    ~connectionParameters() override = default;
    wxString columnCaption(int colIndx) const override;
    bool cellValue(int x, int y, const Yuni::String& value) override;
    IRenderer::CellStyle cellStyle(int col, int row) const override;

private:
    void setMatrix(Data::AreaLink* link) override;

    Antares::Matrix<>* direct_ntc_ = nullptr;
    Antares::Matrix<>* indirect_ntc_ = nullptr;
};

// ===========================
// NTC grid renderer
// ===========================
class connectionNTC : public Connection
{
public:
    connectionNTC(wxWindow* parent, Toolbox::InputSelector::Connections* notifier);
    ~connectionNTC() override = default;
    bool cellValue(int x, int y, const Yuni::String& value) override;
    IRenderer::CellStyle cellStyle(int col, int row) const override;
    virtual bool checkLoopFlow(double ntcIndirect, double loopFlow) const = 0;

protected:
    Antares::Matrix<>::ColumnType* mLoopFlowData = nullptr;
};

// ----------------
// Direct
// ----------------
class connectionNTCdirect : public connectionNTC
{
public:
    connectionNTCdirect(wxWindow* parent, Toolbox::InputSelector::Connections* notifier);
    ~connectionNTCdirect() override = default;
    bool checkLoopFlow(double ntcIndirect, double loopFlow) const override;

private:
    void setMatrix(Data::AreaLink* link) override;
};

// ----------------
// Indirect
// ----------------
class connectionNTCindirect : public connectionNTC
{
public:
    connectionNTCindirect(wxWindow* parent, Toolbox::InputSelector::Connections* notifier);
    ~connectionNTCindirect() override = default;
    bool checkLoopFlow(double ntcIndirect, double loopFlow) const override;

private:
    void setMatrix(Data::AreaLink* link) override;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_CONNECTION_H__
