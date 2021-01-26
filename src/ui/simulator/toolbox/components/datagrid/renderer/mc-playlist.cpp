/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <sstream>
#include <iomanip>
#include <string>
#include "mc-playlist.h"
#include <yuni/core/math.h>


using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	MCPlaylist::MCPlaylist() :
		pControl(nullptr)
	{
	}


	MCPlaylist::~MCPlaylist()
	{
	}


	bool MCPlaylist::valid() const
	{
		if (!study)
			std::cout << "not valid ! \n";
		return !(!study);
	}


	int MCPlaylist::height() const
	{
		return (not study) ? 0 : (int) study->parameters.nbYears;
	}

	wxString MCPlaylist::columnCaption(int i) const
	{
        switch (i)
        {
            case MCPlaylistCol::STATUS			:return wxT("   Status   ");
            case MCPlaylistCol::WEIGHT			:return wxT("   Weight   ");
        }
        return wxString();

	}


	wxString MCPlaylist::rowCaption(int rowIndx) const
	{
		return wxString(wxT("MC year  ")) << (rowIndx + 1) << wxT("  ");
	}


	bool MCPlaylist::cellValue(int x, int y, const Yuni::String& value)
	{
		if (!(!study) && (uint) y < study->parameters.nbYears)
		{
            String s = value;
            s.trim();
            s.toLower();

            switch (x)
            {
                case MCPlaylistCol::STATUS			:
                {
                    bool v = s.to<bool>() || s == "active" || s == "enabled";
                    assert(study->parameters.yearsFilter);
                    study->parameters.yearsFilter[y] = v;
                    break;
                }
                case MCPlaylistCol::WEIGHT			:
                {
                    float weight;
                    if (value.to<float>(weight))
                    {
                        if (weight >= 0.f) {
                            study->parameters.setYearWeight(y, weight);
                        }
                        else {
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
		if (!(!study) && (uint) y < study->parameters.nbYears)
		{
            switch (x)
            {
                case MCPlaylistCol::STATUS:
                {
                    assert(study->parameters.yearsFilter);
                    return study->parameters.yearsFilter[y];
                }
                case MCPlaylistCol::WEIGHT:
                {
                    std::vector<float> yearsWeight = study->parameters.getYearsWeight();
                    assert(y < yearsWeight.size());
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
                    assert(study->parameters.yearsFilter);
                    return study->parameters.yearsFilter[y] ? wxT("Active") : wxT("skip");
                }
                case MCPlaylistCol::WEIGHT:
                {
                    std::vector<float> yearsWeight = study->parameters.getYearsWeight();
                    assert(y < yearsWeight.size());

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
		if (!(!study) && (uint) y < study->parameters.nbYears)
		{
			assert(study->parameters.yearsFilter);
			return !study->parameters.yearsFilter[y]
				? IRenderer::cellStyleConstraintNoWeight : IRenderer::cellStyleConstraintWeight;
		}
		return IRenderer::cellStyleConstraintNoWeight;
	}






} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

