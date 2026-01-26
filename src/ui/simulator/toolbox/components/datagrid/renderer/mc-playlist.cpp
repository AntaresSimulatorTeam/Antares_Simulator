// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <sstream>
#include <iomanip>
#include <string>
#include "mc-playlist.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
MCPlaylist::MCPlaylist():
    pControl(nullptr)
{
}

MCPlaylist::~MCPlaylist()
{
}

bool MCPlaylist::valid() const
{
    if (!study)
    {
        std::cout << "not valid ! \n";
    }
    return !(!study);
}

int MCPlaylist::height() const
{
    return (not study) ? 0 : (int)study->parameters.nbYears;
}

wxString MCPlaylist::columnCaption(int i) const
{
    switch (i)
    {
    case MCPlaylistCol::STATUS:
        return wxT("   Status   ");
    case MCPlaylistCol::WEIGHT:
        return wxT("   Weight   ");
    }
    return wxString();
}

wxString MCPlaylist::rowCaption(int rowIndx) const
{
    return wxString(wxT("MC year  ")) << (rowIndx + 1) << wxT("  ");
}

bool MCPlaylist::cellValue(int x, int y, const Yuni::String& value)
{
    if (!(!study) && (uint)y < study->parameters.nbYears)
    {
        String s = value;
        s.trim();
        s.toLower();

        switch (x)
        {
        case MCPlaylistCol::STATUS:
        {
            bool v = s.to<bool>() || s == "active" || s == "enabled";
            study->parameters.yearsFilter[y] = v;
            break;
        }
        case MCPlaylistCol::WEIGHT:
        {
            float weight;
            if (value.to<float>(weight))
            {
                if (weight >= 0.f)
                {
                    study->parameters.setYearWeight(y, weight);
                }
                else
                {
                    return false;
                }
            }
            break;
        }
        }

        onTriggerUpdate();
        Dispatcher::GUI::Refresh(pControl);
        return true;
    }
    return false;
}

double MCPlaylist::cellNumericValue(int x, int y) const
{
    if (!(!study) && (uint)y < study->parameters.nbYears)
    {
        switch (x)
        {
        case MCPlaylistCol::STATUS:
        {
            return study->parameters.yearsFilter[y];
        }
        case MCPlaylistCol::WEIGHT:
        {
            std::vector<float> yearsWeight = study->parameters.getYearsWeight();
            assert((uint)y < yearsWeight.size());
            return yearsWeight[y];
        }
        }
    }
    return 0.;
}

wxString MCPlaylist::cellValue(int x, int y) const
{
    if (!(!study) && static_cast<uint>(y) < study->parameters.nbYears)
    {
        switch (x)
        {
        case MCPlaylistCol::STATUS:
        {
            return study->parameters.yearsFilter[y] ? wxT("Active") : wxT("skip");
        }
        case MCPlaylistCol::WEIGHT:
        {
            std::vector<float> yearsWeight = study->parameters.getYearsWeight();
            assert((uint)y < yearsWeight.size());

            std::ostringstream stream;
            stream << std::setprecision(3);
            stream << yearsWeight[y];

            return stream.str();
        }
        }
    }
    return wxEmptyString;
}

IRenderer::CellStyle MCPlaylist::cellStyle(int, int y) const
{
    if (!(!study) && (uint)y < study->parameters.nbYears)
    {
        return !study->parameters.yearsFilter[y] ? IRenderer::cellStyleConstraintNoWeight
                                                 : IRenderer::cellStyleConstraintWeight;
    }
    return IRenderer::cellStyleConstraintNoWeight;
}

} // namespace Antares::Component::Datagrid::Renderer
