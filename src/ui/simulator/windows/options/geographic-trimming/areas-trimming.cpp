
#include "areas-trimming.h"
#include <wx/statline.h>
#include "../../../application/study.h"
#include "../../../application/main.h"
#include "../../../toolbox/create.h"
#include "../../../toolbox/components/wizardheader.h"
#include "../../../toolbox/components/datagrid/renderer/areas-trimming-grid.h"

using namespace Yuni;



namespace Antares
{
namespace Window
{
namespace Options
{

	BEGIN_EVENT_TABLE(geographicTrimming, wxDialog)
		EVT_MOTION(geographicTrimming::mouseMoved)
	END_EVENT_TABLE()


	geographicTrimming::geographicTrimming(
			wxFrame* parent,
			Component::Datagrid::Renderer::areasTrimmingGrid* renderer) 
			: wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(1000, 400), wxCLOSE_BOX | wxCAPTION)
	{
		pRenderer = new Component::Datagrid::Renderer::areasTrimmingGrid();
		
		assert(parent);

		// The current study
		auto study = Data::Study::Current::Get();

		// Title of the Form
		SetLabel(wxT("Areas trimming"));
		SetTitle(GetLabel());

		auto* sizer = new wxBoxSizer(wxVERTICAL);

		// Header
		sizer->Add(Toolbox::Components::WizardHeader::Create(this, wxT("Options"),
			"images/32x32/orderedlist.png", wxT("Configure geographic trimming")), 0, wxALL | wxEXPAND | wxFIXED_MINSIZE);

		sizer->AddSpacer(20);

		if (study->parameters.geographicTrimming)
		{
			// Grid renderer
			pRenderer = renderer;
			pRenderer->study = study;

			auto* grid = new Component::Datagrid::Component(this, pRenderer);

			sizer->Add(grid, 1, wxALIGN_CENTER_HORIZONTAL);
			pRenderer->control(grid);
			grid->forceRefresh();
		}
		else
		{
			wxString s = wxT("All geographic results will be printed on disk.");
			wxWindow*  info = Component::CreateLabel(this, s, true, true, +1);
			sizer->Add(info, 1, wxLEFT | wxRIGHT | wxEXPAND | wxALIGN_TOP, 25);
		}
		
		sizer->AddSpacer(5);
		sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND, 8);

		wxButton* btn;
		wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);

		btn = Component::CreateButton(this, wxT("  Close  "), this, &geographicTrimming::onClose);
		hz->AddStretchSpacer();
		hz->Add(btn, 0, wxEXPAND | wxALL);
		hz->Add(20, 5);
		sizer->Add(hz, 0, wxALL | wxEXPAND);
		sizer->AddSpacer(8);

		sizer->Layout();
		SetSizer(sizer);

		// Recentre the window
		Centre(wxBOTH);

		SetAutoLayout(true);
	}

	geographicTrimming::~geographicTrimming() {}

	void geographicTrimming::onClose(void*)
	{
		Dispatcher::GUI::Close(this);
	}

	void geographicTrimming::mouseMoved(wxMouseEvent&)
	{
		// Notify other components as well
		Antares::Component::Panel::OnMouseMoveFromExternalComponent();
	}


	areasTrimming::areasTrimming(wxFrame* parent) :
		geographicTrimming(parent, new Component::Datagrid::Renderer::areasTrimmingGrid())
	{
		// Title of the Form
		SetLabel(wxT("Areas trimming"));
		SetTitle(GetLabel());
	}


} // namespace Options
} // namespace Window
} // namespace Antares
