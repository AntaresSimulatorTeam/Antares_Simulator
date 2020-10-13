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
#ifndef __EXPORT_STRUCTURE__
#define __EXPORT_STRUCTURE__

#include <vector>
#include <string>

#include <stdint.h>
#include <yuni/yuni.h>

#include <antares/Enum.hpp>

namespace Antares {
	namespace Data {
		namespace Enum {

			/*! Enum class to define export structure dictionnary */
			enum class ExportStructDict : unsigned char {
				ValeurDeNTCOrigineVersExtremite,
				PalierThermique,
				ProdHyd, 
				DefaillancePositive,
				DefaillanceNegative,
				BilansPays,
				CoutOrigineVersExtremiteDeLInterconnexion,
				CoutExtremiteVersOrigineDeLInterconnexion,
				CorrespondanceVarNativesVarOptim
			};
		}
	}
}

struct PROBLEME_HEBDO;

void OPT_Export_add_variable(std::vector<std::string>& varname, int Var, Antares::Data::Enum::ExportStructDict structDict, int firstVal, int secondVal, int ts);
void OPT_Export_add_variable(std::vector<std::string>& varname, int Var, Antares::Data::Enum::ExportStructDict structDict, int firstVal, int ts);

void OPT_ExportInterco(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace);
void OPT_ExportAreaName(PROBLEME_HEBDO* ProblemeHebdo, uint numSpace);
void OPT_ExportVariables(const std::vector<std::string>& varname, const std::string& fileName, const std::string& fileExtension, uint numSpace);



#endif