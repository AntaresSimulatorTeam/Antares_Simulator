#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_GRID_H__
# define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_GRID_H__

# include "../renderer.h"
# include "../../../../application/study.h"
# include <antares/study/fwd.h>

using namespace Antares::Data;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
	class geographicTrimmingGrid : public IRenderer
	{
	public:
		geographicTrimmingGrid();
		virtual ~geographicTrimmingGrid();

		virtual int width() const { return 10; }
		virtual int height() const;

		virtual wxString columnCaption(int colIndx) const;

		virtual wxString rowCaption(int rowIndx) const;

		virtual wxString cellValue(int x, int y) const;

		virtual double cellNumericValue(int x, int y) const;

		virtual bool cellValue(int x, int y, const Yuni::String& value);

		virtual void resetColors(int, int, wxColour&, wxColour&) const
		{
			// Do nothing
		}

		virtual bool valid() const;

		virtual uint maxWidthResize() const { return 0; }
		virtual IRenderer::CellStyle cellStyle(int col, int row) const;

		void control(wxWindow* control) { pControl = control; }

	private:
		virtual uint gridSize() const = 0;
		virtual AreaLinkName getName(int index) const = 0;
		virtual uint& getSynthesisFilter(int index) const = 0;
		virtual uint& getYearByYearFilter(int index) const = 0;

	public:
		//! An item has been updated
		Yuni::Bind<void()> onTriggerUpdate;

	protected:
		wxWindow* pControl;

	}; // class geographicTrimmingGrid


	class areasTrimmingGrid : public geographicTrimmingGrid
	{
	public:
		areasTrimmingGrid() {};
		~areasTrimmingGrid() {};

	private:
		uint gridSize() const;
		AreaLinkName getName(int index) const;
		uint& getSynthesisFilter(int index) const;
		uint& getYearByYearFilter(int index) const;

	};	// class areasTrimmingGrid


	class linksTrimmingGrid : public geographicTrimmingGrid
	{
	public:
		linksTrimmingGrid() {};
		~linksTrimmingGrid() {};

	private:
		uint gridSize() const;
		AreaLinkName getName(int index) const;
		uint& getSynthesisFilter(int index) const;
		uint& getYearByYearFilter(int index) const;

	};	// class areasTrimmingGrid



} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_AREAS_TRIMMING_GRID_H__
