#ifndef __ANTARES_APPLICATION_WINDOWS_ADEQUACY_PATCH_AREAS_H__
#define __ANTARES_APPLICATION_WINDOWS_ADEQUACY_PATCH_AREAS_H__

#include <antares/wx-wrapper.h>
#include "../../../toolbox/components/datagrid/component.h"
#include <wx/dialog.h>
#include <wx/checkbox.h>

namespace Antares
{
namespace Window
{
namespace Options
{

class AdequacyPatchAreas final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    AdequacyPatchAreas(wxFrame* parent);
    //! Destructor
    virtual ~AdequacyPatchAreas();
    //@}

protected:
    void mouseMoved(wxMouseEvent& evt);

protected:
    void onClose(void*);
    void onUpdateStatus();

private:
    Component::Datagrid::Component* pGrid;
    wxWindow* pPanel;
    DECLARE_EVENT_TABLE()

}; // class AdequacyPatchAreas

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_ADEQUACY_PATCH_AREAS_H__
