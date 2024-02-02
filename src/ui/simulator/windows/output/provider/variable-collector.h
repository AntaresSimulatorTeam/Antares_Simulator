/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_COLLECTOR_H__
#define __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_COLLECTOR_H__

#include <ui/common/component/spotlight.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
class VariableCollector
{
public:
    //! Spotlight (alias)
    using Spotlight = Antares::Component::Spotlight;

public:
    VariableCollector(Spotlight::IItem::Vector& out,
                      const Spotlight::SearchToken::Vector& tokens,
                      wxBitmap* bmp,
                      wxBitmap* thermalBmp,
                      const String& selection);

    void add(const AnyString& name, const AnyString& /*unit*/, const AnyString& /*comments*/);

    void addCluster(const String& name);

private:
    //! The results
    Spotlight::IItem::Vector& pOut;
    //! Search tokens
    const Spotlight::SearchToken::Vector& pTokens;
    //! Bitmap
    wxBitmap* pBmp;
    wxBitmap* pThermalBmp;
    //!
    const String& pCurrentSelection;

}; // class VariableCollector

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_COLLECTOR_H__