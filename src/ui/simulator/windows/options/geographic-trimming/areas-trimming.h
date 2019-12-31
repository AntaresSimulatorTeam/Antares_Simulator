#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_H__

// # include <antares/wx-wrapper.h> // gp : used for CreateButton ?
# include "../../../toolbox/components/datagrid/component.h"
# include <wx/dialog.h>
// # include <wx/checkbox.h> // gp : used for CreateButton ?


namespace Antares
{
namespace Window
{
namespace Options
{
	
	class areasTrimming final : public wxDialog
	{
	public:
		areasTrimming(wxFrame* parent);
		virtual ~areasTrimming();

	protected:
		void mouseMoved(wxMouseEvent& evt);

	protected:
		void onClose(void*);
		void updateCaption();
		void onBlabla(void *);

	private:
		Component::Datagrid::Component* pGrid;
		wxWindow* pPanel;
		wxWindow* pInfo;

		DECLARE_EVENT_TABLE()

	}; // class areasTrimming


} // namespace Options
} // namespace Window
} // namespace Antares
#endif
