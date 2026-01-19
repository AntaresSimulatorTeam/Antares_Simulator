// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_LAYER_HXX__
#define __ANTARES_WINDOWS_OUTPUT_LAYER_HXX__

namespace Antares::Window::OutputViewer
{
inline const wxString& Layer::alternativeCaption() const
{
    return pAlternativeCaption;
}

inline bool Layer::isVirtual() const
{
    return type != ltOutput;
}

} // namespace Antares::Window::OutputViewer

#endif // __ANTARES_WINDOWS_OUTPUT_LAYER_HXX__
