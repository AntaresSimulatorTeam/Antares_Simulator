// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_H__

#include "../../../input/area.h"
#include "../renderer.h"
#include <yuni/core/event.h>

namespace Antares::Component::Datagrid::Renderer
{
class ARendererArea: public virtual IRenderer
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param parent The parent window
    ** \param notifier The area notifier
    */
    ARendererArea(wxWindow* parent, Toolbox::InputSelector::Area* notifier);
    //! Destructor
    virtual ~ARendererArea();
    //@}

    //! \name Dimensions
    //@{
    //! Get the current width
    virtual int width() const = 0;
    //! Get the current height
    virtual int height() const = 0;
    //@}

    //! \name Captions
    //@{
    /*!
    ** \brief Get the column caption
    ** \param colIndx The column index
    */
    virtual wxString columnCaption(int colIndx) const = 0;
    /*!
    ** \brief Get the row caption
    ** \param rowIndx The row index
    */
    virtual wxString rowCaption(int rowIndx) const = 0;
    //@}

    /*!
    ** \brief Get the string representation of a cell
    ** \param x The X-Coordinate
    ** \param y The Y-Coordinate
    ** \return The value
    */
    virtual wxString cellValue(int x, int y) const = 0;

    /*!
    ** \brief Set the value of a cell
    **
    ** \param x The X-Coordinate
    ** \param y The Y-Coordinate
    ** \param value The new value
    ** \return True if the operation was successful
    */
    virtual bool cellValue(int x, int y, const Yuni::String& value) = 0;

    virtual void resetColors(int x, int y, wxColour& background, wxColour& textForeground) const
      = 0;

    virtual bool valid() const
    {
        return (pArea != NULL);
    }

protected:
    virtual void internalAreaChanged(Data::Area* area);
    //! The study has been closed
    virtual void onStudyClosed() override;
    //! Event: the study has been loaded
    virtual void onStudyLoaded() override;

protected:
    //! The attached control
    wxWindow* pControl;
    //! The current area
    Data::Area* pArea;

private:
    void onAreaChanged(Data::Area* area);
    void onAreaDelete(Data::Area* area);

}; // class ARendererArea

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREA_H__
