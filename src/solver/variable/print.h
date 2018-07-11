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
#ifndef __SOLVER_VARIABLE_PRINT_H__
# define __SOLVER_VARIABLE_PRINT_H__

# include <yuni/yuni.h>


namespace Antares
{
namespace Solver
{
namespace Variable
{


	class PrintInfosStdCout final
	{
	public:
		PrintInfosStdCout()
			:pIndent(0)
		{
		}

		template<class VCardT> void beginNode()
		{
			printVCard<VCardT, true>();
			++pIndent;
		}

		template<class VCardT> void addVCard()
		{
			printVCard<VCardT, false>();
		}

		void endNode()
		{
			--pIndent;
		}

	private:
		template<class VCardT, bool IsNodeT> void printVCard()
		{
			pBuffer.clear();
			pBuffer.resize(1 + pBuffer.size() + pIndent * 4, " ");
			pBuffer += (IsNodeT ? "+ " : "  ");
			pBuffer += VCardT::Caption();
			pBuffer.resize(29, " ");
			pBuffer += VCardT::Unit();
			pBuffer.resize(37, " ");
			pBuffer += VCardT::Description();
			Antares::logs.info() << pBuffer;
		}

		void printIndent()
		{
			if (pIndent)
			{
				for (uint i = 0; i != pIndent; ++i)
					pBuffer += "    ";
			}
		}

	private:
		uint pIndent;
		Yuni::String pBuffer;
	};


} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_PRINT_H__
