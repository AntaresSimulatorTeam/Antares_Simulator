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

#include "nodal-optimization.h"
#include "../../../../../application/study.h"
#include "../../../../../windows/inspector.h"
#include "../../component.h"
#include <antares/study/xcast/xcast.h>
#include <antares/study/area/constants.h>


using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{


	NodalOptimization::NodalOptimization(wxWindow* parent) :
		pControl(parent)
	{
		OnStudyAreaRename.connect(this, &NodalOptimization::onAreaRenamed);
		OnStudyNodalOptimizationChanged.connect(this, &NodalOptimization::onNodalOptimizationExternalChanged);
	}


	NodalOptimization::~NodalOptimization()
	{
		destroyBoundEvents();
	}


	int NodalOptimization::width() const
	{
		return 7;
	}


	int NodalOptimization::height() const
	{
		return !study ? 0 : study->areas.size();
	}


	bool NodalOptimization::valid() const
	{
		return !(!study) && (study->areas.size() != 0);
	}


	wxString NodalOptimization::columnCaption(int colIndx) const
	{
		switch (colIndx)
		{
			case 0: return wxT("   Unsupplied   \n Energy Cost\n(average)");
			case 1: return wxT("   Unsupplied   \n Energy Cost\n(spread)");
			case 2: return wxT("  Spilled  \n   Energy Cost   \n(average)");
			case 3: return wxT("  Spilled  \n   Energy Cost   \n(spread)");
			case 4: return wxT("  Non dispatch. power  \n  Shedding Status");
			case 5: return wxT("  Dispatch. hydropower  \n  Shedding Status");
			case 6: return wxT("  Other Dispatch. power  \n  Shedding Status");
		}
		return wxEmptyString;
	}


	wxString NodalOptimization::rowCaption(int rowIndx) const
	{
		return (!(!study) && (uint)rowIndx < study->areas.size())
			? wxStringFromUTF8(study->areas.byIndex[rowIndx]->name)
			: wxString();
	}


	void NodalOptimization::applyLayerFiltering(size_t layerID, VGridHelper * gridHelper)
	{
		int last = 0; // The count of valid cols 
		for (int y = 0; y < gridHelper->virtualSize.y; ++y)
		{
			// The current area
			auto area = study->areas[gridHelper->indicesRows[y]];

			if (area->isVisibleOnLayer(layerID))
			{
				gridHelper->indicesRows[last] = gridHelper->indicesRows[y];
				++last;
			}

		}

		gridHelper->virtualSize.y = last;


	}

	IRenderer::CellStyle NodalOptimization::cellStyle(int x, int y) const
	{
		if (!(!study) && (uint) y < study->areas.size())
		{
			auto& area = *(study->areas.byIndex[y]);
			switch (x)
			{
				case 0:
					return Math::Zero(area.thermal.unsuppliedEnergyCost)
						? IRenderer::cellStyleDefaultDisabled
						: (area.thermal.unsuppliedEnergyCost<0 ? IRenderer::cellStyleWarning : IRenderer::cellStyleDefault);
				case 1:
					return Math::Zero(area.spreadUnsuppliedEnergyCost)
						? IRenderer::cellStyleDefaultDisabled
						: IRenderer::cellStyleDefault;
				case 2:
					return Math::Zero(area.thermal.spilledEnergyCost)
						? IRenderer::cellStyleDefaultDisabled
						: (area.thermal.spilledEnergyCost<0 ? IRenderer::cellStyleWarning : IRenderer::cellStyleDefault);
				case 3:
					return Math::Zero(area.spreadSpilledEnergyCost)
						? IRenderer::cellStyleDefaultDisabled
						: IRenderer::cellStyleDefault;
				case 4:
					return (area.nodalOptimization & Data::anoNonDispatchPower)
						? IRenderer::cellStyleDefault
						: IRenderer::cellStyleDefaultDisabled;
				case 5:
					return (area.nodalOptimization & Data::anoDispatchHydroPower)
						? IRenderer::cellStyleDefault
						: IRenderer::cellStyleDefaultDisabled;
				case 6:
					return (area.nodalOptimization & Data::anoOtherDispatchPower)
						? IRenderer::cellStyleDefault
						: IRenderer::cellStyleDefaultDisabled;
			}
		}
		return IRenderer::cellStyleDefault;
	}



	wxString NodalOptimization::cellValue(int x, int y) const
	{
		if (!(!study) && (uint) y < study->areas.size())
		{
			const auto& area = *(study->areas.byIndex[y]);
			switch (x)
			{
				case 0:
					return DoubleToWxString(area.thermal.unsuppliedEnergyCost);
				case 1:
					return DoubleToWxString(area.spreadUnsuppliedEnergyCost);
				case 2:
					return DoubleToWxString(area.thermal.spilledEnergyCost);
				case 3:
					return DoubleToWxString(area.spreadSpilledEnergyCost);
				case 4:
					return (area.nodalOptimization & Data::anoNonDispatchPower)   ? wxT("Yes") : wxT("No");
				case 5:
					return (area.nodalOptimization & Data::anoDispatchHydroPower) ? wxT("Yes") : wxT("No");
				case 6:
					return (area.nodalOptimization & Data::anoOtherDispatchPower) ? wxT("Yes") : wxT("No");
			}
		}
		return wxEmptyString;
	}


	double NodalOptimization::cellNumericValue(int x,int y) const
	{
		if (!(!study) && (uint) y < study->areas.size())
		{
			const auto& area = *(study->areas.byIndex[y]);
			switch (x)
			{
				case 0:
					return area.thermal.unsuppliedEnergyCost;
				case 1:
					return area.spreadUnsuppliedEnergyCost;
				case 2:
					return area.thermal.spilledEnergyCost;
				case 3:
					return area.spreadSpilledEnergyCost;
				case 4:
					return (area.nodalOptimization & Data::anoNonDispatchPower)   ? 1. : 0.;
				case 5:
					return (area.nodalOptimization & Data::anoDispatchHydroPower) ? 1. : 0.;
				case 6:
					return (area.nodalOptimization & Data::anoOtherDispatchPower) ? 1. : 0.;
			}
		}
		return 0.;
	}


	bool NodalOptimization::cellValue(int x, int y, const String& value)
	{
		if (!study || (uint) y >= study->areas.size())
			return false;

		auto& area = *(study->areas.byIndex[y]);

		// MBO 15/04/2014
		// limit unsupplied energy cost (average and spread)
		// limit spilled (average and spread)

		switch (x)
		{
			case 0:
				{
					double d;
					if (value.to<double>(d) && !Math::Equals(d, area.thermal.unsuppliedEnergyCost))
					{
						// New scheme
						if (Math::Abs(d) < 5.e-3)
							d = 0.;
						else
						{
							if (d > 5.e4)
								d = 5.e4;
							else
							{
								if (d < -5.e4)
									d = -5.e4;
								else
									d = Math::Round(d, 3);
							}
						}
						area.thermal.unsuppliedEnergyCost = d;
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
			case 1:
				{
					double d;
					if (value.to<double>(d) && !Math::Equals(d, area.spreadUnsuppliedEnergyCost))
					{
						if ( d<5.e-3 )
						{
							d=0.;
						}
						else
						{
							if (d > 5.e4)
								d = 5.e4;
						}
						area.spreadUnsuppliedEnergyCost = Math::Round(d, 3);
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
			case 2:
				{
					double d;
					if (value.to<double>(d) && !Math::Equals(d, area.thermal.spilledEnergyCost))
					{
						// New scheme
						if (Math::Abs(d) < 5.e-3)
							d = 0.;
						else
						{
							if (d > 5.e4)
								d = 5.e4;
							else
							{
								if (d < -5.e4)
									d = -5.e4;
								else
									d =Math::Round(d, 3);
							}
						}

						area.thermal.spilledEnergyCost = d;
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
			case 3:
				{
					double d;
					if (value.to<double>(d) && !Math::Equals(d, area.spreadSpilledEnergyCost))
					{
						if ( d<5.e-3 )
						{
							d=0.;
						}
						else
						{
							if (d > 5.e4)
								d = 5.e4;
						}
						area.spreadSpilledEnergyCost = Math::Round(d, 3);
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
			case 4:
				{
					const bool b = value.to<bool>();
					if (b != (area.nodalOptimization & Data::anoNonDispatchPower))
					{
						if (b)
							area.nodalOptimization |= Data::anoNonDispatchPower;
						else
							area.nodalOptimization &= ~Data::anoNonDispatchPower;
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
			case 5:
				{
					bool b = value.to<bool>();
					if (b != (0 != (area.nodalOptimization & Data::anoDispatchHydroPower)))
					{
						if (b)
							area.nodalOptimization |= Data::anoDispatchHydroPower;
						else
							area.nodalOptimization &= ~Data::anoDispatchHydroPower;
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
			case 6:
				{
					bool b = value.to<bool>();
					if (b != (0 != (area.nodalOptimization & Data::anoOtherDispatchPower)))
					{
						if (b)
							area.nodalOptimization |= Data::anoOtherDispatchPower;
						else
							area.nodalOptimization &= ~Data::anoOtherDispatchPower;
						Window::Inspector::Refresh();
						OnInspectorRefresh(nullptr);
						return true;
					}
					break;
				}
		}

		return false;
	}


	void NodalOptimization::onAreaRenamed(Antares::Data::Area*)
	{
		if (pControl)
			pControl->Refresh();
	}


	void NodalOptimization::onNodalOptimizationExternalChanged()
	{
		if (pControl)
			pControl->Refresh();
	}





} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

