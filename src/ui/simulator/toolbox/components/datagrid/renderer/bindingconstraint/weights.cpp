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

#include <yuni/yuni.h>
#include <antares/study.h>
#include <antares/study/memory-usage.h>
#include "weights.h"
#include <antares/wx-wrapper.h>
#include <yuni/core/math.h>
#include <wx/window.h>


using namespace Yuni;


namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
namespace BindingConstraint
{


	LinkWeights::LinkWeights(wxWindow* control) :
		pControl(control),
		pZero(wxT("0 "))
	{
	}


	LinkWeights::~LinkWeights()
	{
		destroyBoundEvents();
	}


	wxString LinkWeights::rowCaption(int rowIndx) const
	{
		switch (rowIndx)
		{
			case 0: return wxT("Bounds");
			case 1: return wxT("Type");
			case 2: return wxT("Links");
			case 3: return wxT("On / Off");
			case 4: return wxEmptyString;
			default:
				if (!(!study))
				{
					if ((uint)(rowIndx - 5) < study->uiinfo->linkCount())
					{
						auto* lnk = study->uiinfo->link((uint)(rowIndx - 5));
						assert(lnk);
						assert(lnk->from);
						assert(lnk->with);
						wxString s;
						s += wxStringFromUTF8(lnk->from->id);
						s += wxT(" / ");
						s += wxStringFromUTF8(lnk->with->id);
						return s;
					}
				}
		}
		return wxEmptyString;
	}


	wxString LinkWeights::cellValue(int x, int y) const
	{
		if (!study)
			return wxEmptyString;

		if ((uint)x >= study->uiinfo->constraintCount())
			return wxEmptyString;
		if (y < 5)
		{
			switch (y)
			{
				case 0:
					return wxStringFromUTF8(
						Data::BindingConstraint::MathOperatorToCString((study->uiinfo->constraint(x))->operatorType()))
						<< wxT(' ');
				case 1:
					return wxStringFromUTF8(
						Data::BindingConstraint::TypeToCString((study->uiinfo->constraint(x))->type())) << wxT(' ');
				case 2:
					return wxString() << (study->uiinfo->constraint(x))->linkCount() << wxT(" links   ");
				case 3:
					{
						if ((study->uiinfo->constraint(x))->enabled())
						{
							if (((study->uiinfo->constraint(x))->linkCount() > 0 || (study->uiinfo->constraint(x))->enabledClusterCount() > 0))
								return wxT("   Yes   ");
							return wxT("   Skipped   ");
						}
						return wxT("   Disabled   ");
					}
			}
			return wxEmptyString;
		}

		const double d = cellNumericValue(x, y);
		if (!Math::Zero(d))
			return DoubleToWxString(d);
		return pZero;
	}


	wxString LinkWeights::columnCaption(int x) const
	{
		if (not Data::Study::Current::Valid())
			return wxEmptyString;

		if ((uint) x < study->uiinfo->constraintCount())
			return wxStringFromUTF8((study->uiinfo->constraint((uint) x))->name());
		return wxEmptyString;
	}


	IRenderer::CellStyle LinkWeights::cellStyle(int x, int y) const
	{
		if (not Data::Study::Current::Valid())
			return IRenderer::cellStyleConstraintDisabled;

		if ((uint) x >= study->uiinfo->constraintCount())
			return IRenderer::cellStyleConstraintDisabled;
		switch (y)
		{
			case 0:
				return IRenderer::cellStyleConstraintOperator;
			case 1:
				return IRenderer::cellStyleConstraintType;
			case 2:
				return IRenderer::cellStyleConstraintWeightCount;
			case 3:
				return ((study->uiinfo->constraint(x))->enabled() &&
					((study->uiinfo->constraint(x))->linkCount() > 0 || (study->uiinfo->constraint(x))->enabledClusterCount() > 0))
					? IRenderer::cellStyleConstraintEnabled
					: IRenderer::cellStyleConstraintDisabled;
			case 4:
				return IRenderer::cellStyleDefault;

			default:
				{
					return IRenderer::cellStyleCustom;
					return (Math::Zero(
						(study->uiinfo->constraint(x))->weight(study->uiinfo->link(y - 5))))
						? IRenderer::cellStyleConstraintNoWeight
						: IRenderer::cellStyleConstraintWeight;
				}
		}
	}


	bool LinkWeights::cellValue(int x, int y, const String& value)
	{
		if (!study || !study->uiinfo || (uint) x >= study->uiinfo->constraintCount())
			return false;

		auto& uiinfo = *(study->uiinfo);
		auto* constraint = ((uint) x < uiinfo.orderedConstraint.size())
			? (uiinfo.constraint(x)) : nullptr;
		if (!constraint)
			return false;

		switch (y)
		{
			case 0: // operator
				{
					const Data::BindingConstraint::Operator op = Data::BindingConstraint::StringToOperator(value);
					if (op != Data::BindingConstraint::opUnknown)
					{
						constraint->operatorType(op);
						uiinfo.reloadBindingConstraints();
						OnStudyConstraintModified(constraint);
						return true;
					}
					return false;
				}
			case 1: // type
				{
					if (constraint->type() != Data::BindingConstraint::typeHourly)
					{
						const Data::BindingConstraint::Type type = Data::BindingConstraint::StringToType(value);
						if (type == Data::BindingConstraint::typeDaily || type == Data::BindingConstraint::typeWeekly)
						{
							constraint->mutateTypeWithoutCheck(type);
							OnStudyConstraintModified(constraint);
							return true;
						}
					}
					return false;
				}
			case 2:
				return false;
			case 3:
				{
					const bool v = value.to<bool>();
					constraint->enabled(v);
					OnStudyConstraintModified(constraint);
					return true;
				}
			case 4:
				return false;

			default:
				{
					uint linkIndex = y - 5;
					assert(linkIndex < uiinfo.linkCount());

					double d;
					if (value.to(d))
					{
						if (linkIndex < uiinfo.linkCount())
						{
							constraint->weight(uiinfo.link(linkIndex), d);
							if (pControl)
								pControl->Refresh();
							OnStudyConstraintModified(constraint);
							return true;
						}
					}
				}
		}
		return false;
	}


	bool LinkWeights::valid() const
	{
		return !study ? false : (study->bindingConstraints.size() > 0);
	}


	double LinkWeights::cellNumericValue(int x, int y) const
	{
		if (!study)
			return 0;

		return (y > 4)
			? (study->uiinfo->constraint(x))->weight(study->uiinfo->link(y-5))
			: ((y == 3) ? (study->uiinfo->constraint(x))->linkCount() : 0.);
	}


	int LinkWeights::width() const
	{
		return !study ? 0 : study->uiinfo->constraintCount();
	}


	int LinkWeights::height() const
	{
		return !study ? 0 : study->uiinfo->linkCount() + 5;
	}


	wxColour LinkWeights::cellBackgroundColor(int x, int y) const
	{
		if (!study)
			return wxColour(229, 206, 206);

		double value = cellNumericValue(x, y);
		if (Math::NaN(value) || Math::Zero(value))
			return wxColour(250, 250, 250);

		return wxColour(240, 240, 250);
	}


	wxColour LinkWeights::cellTextColor(int x, int y) const
	{
		if (!study)
			return wxColour(0, 0, 0);

		double value = cellNumericValue(x, y);
		if (Math::Zero(value))
			return wxColour(230, 230, 230);
		return wxColour(255, 100, 43);
	}

	void LinkWeights::applyLayerFiltering(size_t layerID, VGridHelper* gridHelper)
	{
		int last = 0; // The count of valid cols 
		for (int x = 0; x < gridHelper->virtualSize.x; ++x)
		{
			// The current constraint
			Data::BindingConstraint* constraint = study->uiinfo->constraint(gridHelper->indicesCols[x]);

			if (constraint->hasAllWeightedLinksOnLayer(layerID) && constraint->hasAllWeightedClustersOnLayer(layerID))
			{
				gridHelper->indicesCols[last] = gridHelper->indicesCols[x];
				++last;
			}

		}
		//gridHelper->indicesCols.resize(last);
		gridHelper->virtualSize.x = last;

		last = 5;// The count of valid rows
		for (int r = 5; r != gridHelper->virtualSize.y; ++r)
		{
			auto* lnk = study->uiinfo->link((uint)(r - 5));
			assert(lnk);
			assert(lnk->from);
			assert(lnk->with);
			if (lnk->isVisibleOnLayer(layerID))
			{
				gridHelper->indicesRows[last] = r;
				++last;
			}
		}
		gridHelper->virtualSize.y = last;
	}

	ClusterWeights::ClusterWeights(wxWindow* control) :
		pControl(control),
		pZero(wxT("0 "))
	{
	}


	ClusterWeights::~ClusterWeights()
	{
		destroyBoundEvents();
	}


	wxString ClusterWeights::rowCaption(int rowIndx) const
	{
		switch (rowIndx)
		{
		case 0: return wxT("Bounds");
		case 1: return wxT("Type");
		case 2: return wxT("Clusters");
		case 3: return wxT("On / Off");
		case 4: return wxEmptyString;
		default:
			if (!(!study))
			{
				if ((uint)(rowIndx - 5) < study->uiinfo->clusterCount())
				{
					auto* clstr = study->uiinfo->cluster((uint)(rowIndx - 5));
					assert(clstr);
					assert(clstr->parentArea);
					wxString s;
					s += wxStringFromUTF8(clstr->getFullName());
					return s;
				}
			}
		}
		return wxEmptyString;
	}


	wxString ClusterWeights::cellValue(int x, int y) const
	{
		if (!study)
			return wxEmptyString;

		if ((uint)x >= study->uiinfo->constraintCount())
			return wxEmptyString;
		if (y < 5)
		{
			switch (y)
			{
			case 0:
				return wxStringFromUTF8(
					Data::BindingConstraint::MathOperatorToCString((study->uiinfo->constraint(x))->operatorType()))
					<< wxT(' ');
			case 1:
				return wxStringFromUTF8(
					Data::BindingConstraint::TypeToCString((study->uiinfo->constraint(x))->type())) << wxT(' ');
			case 2:
				return wxString() << (study->uiinfo->constraint(x))->clusterCount() << wxT(" clusters   ");
			case 3:
			{
				if ((study->uiinfo->constraint(x))->enabled())
				{
					if (((study->uiinfo->constraint(x))->linkCount() > 0 || (study->uiinfo->constraint(x))->enabledClusterCount() > 0))
						return wxT("   Yes   ");
					return wxT("   Skipped   ");
				}
				return wxT("   Disabled   ");
			}
			}
			return wxEmptyString;
		}

		// Cluster is must-run : this cluster state is printed in the grid
		if (study->uiinfo->cluster(y - 5)->mustrun)
			return wxT("   must-run   ");
		
		// Cluster is disabled : this cluster state is printed in the grid
		if(not study->uiinfo->cluster(y - 5)->enabled)
			return wxT("   disabled   ");

		const double d = cellNumericValue(x, y);
		if (!Math::Zero(d))
			return DoubleToWxString(d);
		return pZero;
	}


	wxString ClusterWeights::columnCaption(int x) const
	{
		if (not Data::Study::Current::Valid())
			return wxEmptyString;

		if ((uint)x < study->uiinfo->constraintCount())
			return wxStringFromUTF8((study->uiinfo->constraint((uint)x))->name());
		return wxEmptyString;
	}


	IRenderer::CellStyle ClusterWeights::cellStyle(int x, int y) const
	{
		if (not Data::Study::Current::Valid())
			return IRenderer::cellStyleConstraintDisabled;

		if ((uint)x >= study->uiinfo->constraintCount())
			return IRenderer::cellStyleConstraintDisabled;
		switch (y)
		{
		case 0:
			return IRenderer::cellStyleConstraintOperator;
		case 1:
			return IRenderer::cellStyleConstraintType;
		case 2:
			return IRenderer::cellStyleConstraintWeightCount;
		case 3:
			return ((study->uiinfo->constraint(x))->enabled() &&
				((study->uiinfo->constraint(x))->linkCount() > 0 || (study->uiinfo->constraint(x))->enabledClusterCount() > 0))
				? IRenderer::cellStyleConstraintEnabled
				: IRenderer::cellStyleConstraintDisabled;
		case 4:
			return IRenderer::cellStyleDefault;

		default:
		{
			if (study->uiinfo->cluster(y - 5)->enabled && !study->uiinfo->cluster(y - 5)->mustrun)
				return IRenderer::cellStyleCustom;
			else
				return IRenderer::cellStyleDisabled;
		}
		}
	}


	bool ClusterWeights::cellValue(int x, int y, const String& value)
	{
		if (!study || !study->uiinfo || (uint)x >= study->uiinfo->constraintCount())
			return false;

		auto& uiinfo = *(study->uiinfo);
		auto* constraint = ((uint)x < uiinfo.orderedConstraint.size())
			? (uiinfo.constraint(x)) : nullptr;
		if (!constraint)
			return false;

		switch (y)
		{
		case 0: // operator
		{
			const Data::BindingConstraint::Operator op = Data::BindingConstraint::StringToOperator(value);
			if (op != Data::BindingConstraint::opUnknown)
			{
				constraint->operatorType(op);
				uiinfo.reloadBindingConstraints();
				OnStudyConstraintModified(constraint);
				return true;
			}
			return false;
		}
		case 1: // type
		{
			if (constraint->type() != Data::BindingConstraint::typeHourly)
			{
				const Data::BindingConstraint::Type type = Data::BindingConstraint::StringToType(value);
				if (type == Data::BindingConstraint::typeDaily || type == Data::BindingConstraint::typeWeekly)
				{
					constraint->mutateTypeWithoutCheck(type);
					OnStudyConstraintModified(constraint);
					return true;
				}
			}
			return false;
		}
		case 2:
			return false;
		case 3:
		{
			const bool v = value.to<bool>();
			constraint->enabled(v);
			OnStudyConstraintModified(constraint);
			return true;
		}
		case 4:
			return false;

		default:
		{
			uint clusterIndex = y - 5;
			assert(clusterIndex < uiinfo.clusterCount());
			bool clusterEnabled = uiinfo.cluster(clusterIndex)->enabled;
			bool clusterMustRun = uiinfo.cluster(clusterIndex)->mustrun;
			if (not clusterEnabled || clusterMustRun)
				return true;

			double d;
			if (value.to(d))
			{
				if (clusterIndex < uiinfo.clusterCount())
				{
					constraint->weight(uiinfo.cluster(clusterIndex), d);
					if (pControl)
						pControl->Refresh();
					OnStudyConstraintModified(constraint);
					return true;
				}
			}
		}
		}
		return false;
	}


	bool ClusterWeights::valid() const
	{
		return !study ? false : (study->bindingConstraints.size() > 0);
	}


	double ClusterWeights::cellNumericValue(int x, int y) const
	{
		if (!study)
			return 0;

		return (y > 4)
			? (study->uiinfo->constraint(x))->weight(study->uiinfo->cluster(y - 5))
			: ((y == 3) ? (study->uiinfo->constraint(x))->clusterCount() : 0.);
	}


	int ClusterWeights::width() const
	{
		return !study ? 0 : study->uiinfo->constraintCount();
	}


	int ClusterWeights::height() const
	{
		return !study ? 0 : study->uiinfo->clusterCount() + 5;
	}


	wxColour ClusterWeights::cellBackgroundColor(int x, int y) const
	{
		if (!study)
			return wxColour(229, 206, 206);

		double value = cellNumericValue(x, y);
		if (Math::NaN(value) || Math::Zero(value))
			return wxColour(250, 250, 250);

		return wxColour(240, 240, 250);
	}


	wxColour ClusterWeights::cellTextColor(int x, int y) const
	{
		if (!study)
			return wxColour(0, 0, 0);

		double value = cellNumericValue(x, y);
		if (Math::Zero(value))
			return wxColour(230, 230, 230);
		return wxColour(255, 100, 43);
	}

	void ClusterWeights::applyLayerFiltering(size_t layerID, VGridHelper* gridHelper)
	{
		int last = 0; // The count of valid cols 
		for (int x = 0; x < gridHelper->virtualSize.x; ++x)
		{
			// The current constraint
			Data::BindingConstraint* constraint = study->uiinfo->constraint(gridHelper->indicesCols[x]);

			if (constraint->hasAllWeightedLinksOnLayer(layerID) && constraint->hasAllWeightedClustersOnLayer(layerID))
			{
				gridHelper->indicesCols[last] = gridHelper->indicesCols[x];
				++last;
			}

		}
		//gridHelper->indicesCols.resize(last);
		gridHelper->virtualSize.x = last;

		last = 5;// The count of valid rows
		for (int r = 5; r != gridHelper->virtualSize.y; ++r)
		{
		auto* clstr = study->uiinfo->cluster((uint)(r - 5));
		assert(clstr);
		if (clstr->isVisibleOnLayer(layerID))
		{
		gridHelper->indicesRows[last] = r;
		++last;
		}
		}
		gridHelper->virtualSize.y = last;
	}



} // namespace BindingConstraint
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

