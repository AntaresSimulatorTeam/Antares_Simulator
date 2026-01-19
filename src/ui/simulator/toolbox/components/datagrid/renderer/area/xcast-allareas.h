// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_H__

#include "../../renderer.h"
#include <antares/study/study.h>
#include <yuni/core/event.h>

namespace Antares::Component::Datagrid::Renderer
{
template<enum Data::TimeSeriesType T>
class XCastAllAreas final: public Renderer::IRenderer
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    XCastAllAreas(wxWindow* parent, Toolbox::InputSelector::Area* notifier);

    //! Destructor
    virtual ~XCastAllAreas();
    //@}

    virtual int width() const;

    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int, int) const;

    virtual double cellNumericValue(int, int) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;
    virtual wxColour cellBackgroundColor(int, int) const;

    virtual bool valid() const;

protected:
    virtual void onUpdate()
    {
    }

    void onAreaRenamed(Data::Area* area);

private:
    wxWindow* pControl;
    Toolbox::InputSelector::Area* pNotifier;

}; // class XCastAllAreas

} // namespace Antares::Component::Datagrid::Renderer

#include "xcast-allareas.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_XCAST_ALL_AREAS_H__
