// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_E_EXECUTE_H__
#define __ANTARES_TOOLBOX_E_EXECUTE_H__

#include <wx/wx.h>

namespace Antares::Toolbox::Process
{
class Execute
{
public:
    //@{
    /*!
    ** \brief Default constructor
    */
    Execute();
    /*!
    ** \brief Destructor
    */
    ~Execute();

    //@}

    //! \name Command to execute
    //@{
    //! Get the command
    const wxString& command() const
    {
        return pCommand;
    }

    //! Set the command to execute
    void command(const wxString& v)
    {
        pCommand = v;
    }

    //@}

    //! \name Working directory
    //@{
    //! Get the working directory to use before executing the command
    const wxString& workingDirectory() const
    {
        return pWDirectory;
    }

    //! Set the working directory
    void workingDirectory(const wxString& v)
    {
        pWDirectory = v;
    }

    //@}

    //! \name Title
    //@{
    //! Get the sub-title value
    const wxString& title() const
    {
        return pTitle;
    }

    //! Set the title
    void title(const wxString& v)
    {
        pTitle = v;
    }

    //@}

    //! \name Title
    //@{
    //! Get the sub-title value
    const wxString& subTitle() const
    {
        return pSubTitle;
    }

    //! Set the sub-title
    void subTitle(const wxString& v)
    {
        pSubTitle = v;
    }

    //@}

    //! \name The progress bar
    //@{
    //! Get if the progress bar can be displayed
    bool displayProgressBar() const
    {
        return pDisplayProgressBar;
    }

    //! Set if the progress bar can be displayed
    void displayProgressBar(const bool v)
    {
        pDisplayProgressBar = v;
    }

    //@}

    //! \name Icon
    //@{
    //! Get the relative address to the icon in the resources folder
    const char* icon() const
    {
        return pIcon;
    }

    //! Set the relative address to the icon in the resources folder
    void icon(const char* v)
    {
        pIcon = v;
    }

    //@}

    //! \name Execution workflow
    //@{
    /*!
    ** \brief Run the command
    */
    bool run();
    //@}

private:
    //! The title
    wxString pTitle;
    //! The sub-title
    wxString pSubTitle;
    //! The command to execute
    wxString pCommand;
    //! The working directory
    wxString pWDirectory;
    //!
    bool pDisplayProgressBar;
    //! The icon
    const char* pIcon;

}; // class Execute

} // namespace Antares::Toolbox::Process

#endif // __ANTARES_TOOLBOX_E_EXECUTE_H__
