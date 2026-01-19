// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_FWD_H__
#define __ANTARES_TOOLBOX_FWD_H__

// Wx stuff
class wxPanel;
class wxDialog;
class wxBoxSizer;
class wxSizer;
class wxWindow;
class wxStaticText;
class wxChoice;
class wxTextCtrl;
class wxButton;
class wxGrid;
class wxColour;
class wxSplitterWindow;
class wxTimer;

namespace Antares::Component::Datagrid
{
// The custom wxGridTableBase used by Component
class VGridHelper;
class DBGrid;
class Component;

} // namespace Antares::Component::Datagrid

namespace Antares::Component::HTMLListbox
{
class Component;

}

namespace Antares::Component
{
class Panel;
class Button;
class Notebook;
class Spotlight;

} // namespace Antares::Component

namespace Antares::Component
{
class MainPanel;

}

#endif // __ANTARES_TOOLBOX_FWD_H__
