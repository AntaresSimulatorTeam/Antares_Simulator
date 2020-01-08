#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_H__

# include "../../../toolbox/components/datagrid/component.h"
# include "../../../toolbox/components/datagrid/renderer/geographic-trimming-grid.h"
# include <wx/dialog.h>


namespace Antares
{
namespace Window
{
namespace Options
{
	
	class geographicTrimming : public wxDialog
	{
	public:
		geographicTrimming(wxFrame* parent, Component::Datagrid::Renderer::geographicTrimmingGrid* renderer);
		virtual ~geographicTrimming();

	protected:
		void mouseMoved(wxMouseEvent& evt);
		void onClose(void*);

	private:
	 	Component::Datagrid::Renderer::geographicTrimmingGrid* pRenderer;

		DECLARE_EVENT_TABLE()

	}; // class geographicTrimming



	class areasTrimming final : public geographicTrimming
	{
	public:
		areasTrimming(wxFrame* parent);
	}; // class areasTrimming



	class linksTrimming final : public geographicTrimming
	{
	public:
		linksTrimming(wxFrame* parent);
	}; // class areasTrimming

} // namespace Options
} // namespace Window
} // namespace Antares
#endif
