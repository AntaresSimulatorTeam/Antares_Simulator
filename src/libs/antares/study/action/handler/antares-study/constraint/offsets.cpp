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

#include "offsets.h"

using namespace Yuni;

namespace Antares
{
	namespace Action
	{
		namespace AntaresStudy
		{
			namespace Constraint
			{

				Offsets::Offsets(const AnyString& name, Antares::Data::ConstraintName targetName) :
					pOriginalConstraintName(name),
					targetName(targetName),
					pCurrentContext(NULL)
				{
					pInfos.caption << "Offsets";
				}


				Offsets::~Offsets()
				{}



				bool Offsets::prepareWL(Context&)
				{
					pInfos.message.clear();
					pInfos.state = stReady;
					switch (pInfos.behavior)
					{
					case bhOverwrite:
						pInfos.message << "The Offsets will be copied";
						break;
					default:
						pInfos.state = stNothingToDo;
						break;
					}
					return true;
				}


				void Offsets::translate(Antares::Data::AreaName& out, const Antares::Data::AreaName& original)
				{
					assert(pCurrentContext);
					assert(not original.empty());

					auto i = pCurrentContext->areaLowerNameMapping.find(original);
					if (i != pCurrentContext->areaLowerNameMapping.end())
					{
						out = i->second;
						out.toLower();
					}
					else
						out = original;
					assert(not out.empty());
					logs.debug() << "  copy/paste: binding constraint offset : " << original << " -> " << out;
				}

				void Offsets::toLower(Antares::Data::AreaName& out, const Antares::Data::AreaName& original)
				{

					out = original;
					out.toLower();

					assert(not out.empty());
					logs.debug() << "  copy/paste: binding constraint weight : " << original << " -> " << out;
				}

				bool Offsets::performWL(Context& ctx)
				{
					if (ctx.constraint && ctx.extStudy)
					{
						assert(not pOriginalConstraintName.empty());

						Antares::Data::ConstraintName id;
						TransformNameIntoID(pOriginalConstraintName, id);

						Data::BindingConstraint* source = ctx.extStudy->bindingConstraints.find(id);

						if (source && source != ctx.constraint)
						{
							pCurrentContext = &ctx;
							Bind<void(Data::AreaName&, const Data::AreaName&)> tr;
							if (!targetName.empty())
							{
								tr.bind(this, &Offsets::toLower);
							}
							else
							{
								tr.bind(this, &Offsets::translate);
							}
							ctx.constraint->copyOffsets(ctx.study, *source, true, tr);
							pCurrentContext = nullptr;
							return true;
						}
					}
					return false;
				}





			} // namespace Constraint
		} // namespace AntaresStudy
	} // namespace Action
} // namespace Antares

