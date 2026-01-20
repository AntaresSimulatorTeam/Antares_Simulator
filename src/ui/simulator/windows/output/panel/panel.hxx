// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_HXX__
#define __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_HXX__

namespace Antares::Private::OutputViewerData
{
inline const Antares::Window::OutputViewer::Layer* Panel::layer() const
{
    return pLayer;
}

inline Antares::Window::OutputViewer::Layer* Panel::layer()
{
    return pLayer;
}

inline uint Panel::index() const
{
    return pIndex;
}

inline const Yuni::String& Panel::filename() const
{
    return pFilename;
}

} // namespace Antares::Private::OutputViewerData

#endif // __ANTARES_WINDOWS_OUTPUT_PANEL_PANEL_HXX__
