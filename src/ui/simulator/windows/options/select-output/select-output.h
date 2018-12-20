#ifndef __ANTARES_APPLICATION_WINDOWS_SELECTOUTPUT_H__
# define __ANTARES_APPLICATION_WINDOWS_SELECTOUTPUT_H__

# include <antares/wx-wrapper.h>
# include "../../../toolbox/components/datagrid/component.h"
# include <wx/dialog.h>
# include <wx/checkbox.h>


namespace Antares
{
namespace Window
{
namespace Options
{

	/*!
	** \brief Startup Wizard User Interface
	**
	** The Startup Wizard is an easy to use, quick way to begin using Antares.
	** It provides the most common File Operations on a Study.
	** By default, you will be prompted to use the Startup Wizard when Antares is opened.
	*/
	class SelectOutput final : public wxDialog
	{
	public:
		enum IDs
		{
			mnIDEnabled = wxID_HIGHEST + 1,
		};

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		**
		** \param parent The parent window
		*/
		SelectOutput(wxFrame* parent);
		//! Destructor
		virtual ~SelectOutput();
		//@}


	protected:
		void mouseMoved(wxMouseEvent& evt);

	protected:
		//! Select all MC Years
		void onSelectAll(void*);
		void onUnselectAll(void*);
		void onToggle(void*);
		//! Event: Close the window
		void onClose(void*);
		void onUpdateStatus();
		void evtEnabled(wxCommandEvent& evt);
		void updateCaption();

	private:
		Component::Datagrid::Component* pGrid;
		wxCheckBox* pStatus;
		wxWindow* pPanel;
		wxWindow* pInfo;
		// Event table
		DECLARE_EVENT_TABLE()

	}; // class SelectOutput






} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_SELECTOUTPUT_H__
