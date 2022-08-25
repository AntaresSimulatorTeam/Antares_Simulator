#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_ADEQUACY_PATCH_AREAS_GRID_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_ADEQUACY_PATCH_AREAS_GRID_H__

#include <antares/wx-wrapper.h>
#include "../renderer.h"
#include "../../../../application/study.h"

namespace Antares::Component::Datagrid::Renderer
{
class AdequacyPatchAreaGrid : public IRenderer
{
public:
    AdequacyPatchAreaGrid();
    ~AdequacyPatchAreaGrid() override;

    int width() const override
    {
        return 1;
    }
    virtual uint gridSize() const;

    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    void resetColors(int, int, wxColour&, wxColour&) const override
    {
        // Do nothing
    }

    virtual bool valid() const;

    uint maxWidthResize() const override
    {
        return 0;
    }
    IRenderer::CellStyle cellStyle(int col, int row) const override;

    void control(wxWindow* control)
    {
        pControl = control;
    }

public:
    //! An item has been updated
    Yuni::Bind<void()> onTriggerUpdate;

private:
    wxWindow* pControl;

}; // class AdequacyPatchAreaGrid

}

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_ADEQUACY_PATCH_AREAS_GRID_H__