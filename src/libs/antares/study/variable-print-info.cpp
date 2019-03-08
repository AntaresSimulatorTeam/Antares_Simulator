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

#include "variable-print-info.h"
#include<algorithm>
#include<assert.h>
#include <antares/study.h>
#include "../solver/variable/categories.h"


using namespace Antares::Solver::Variable;


namespace Antares
{
namespace Data
{	
	// ============================================================
	// One variable print information
	// ============================================================
	VariablePrintInfo::VariablePrintInfo(AnyString vname, uint maxNbCols, uint dataLvl, uint fileLvl)
		: varname(""), to_be_printed(true), maxNumberColumns(maxNbCols),
		  dataLevel(dataLvl), fileLevel(fileLvl)
	{ varname = vname; }

	string VariablePrintInfo::name() { return varname.to<string>(); }
	void VariablePrintInfo::enablePrint(bool b) { to_be_printed = b; }
	bool VariablePrintInfo::isPrinted() { return to_be_printed; }
	uint VariablePrintInfo::getColumnsCount() { return maxNumberColumns; }

	// ============================================================
	// Variables print information collector
	// ============================================================
	variablePrintInfoCollector::variablePrintInfoCollector(AllVariablesPrintInfo * allvarsprintinfo)
		: allvarsinfo(allvarsprintinfo)
	{}

	void variablePrintInfoCollector::add(const AnyString& name, uint nbGlobalResults, uint dataLevel, uint fileLevel/*, bool possibly_non_applicable*/)
	{
		allvarsinfo->add(new VariablePrintInfo(name, nbGlobalResults, dataLevel, fileLevel));
	}

	// ============================================================
	// All variables print information
	// ============================================================
	AllVariablesPrintInfo::AllVariablesPrintInfo()
		: columnsCount(0)
	{	
		// Re-initializing the iterator
		it_info = allVarsPrintInfo.begin();
	}

	AllVariablesPrintInfo::~AllVariablesPrintInfo()
	{
		clear();
	}

	void AllVariablesPrintInfo::add(VariablePrintInfo * v) { allVarsPrintInfo.push_back(v); }

	void AllVariablesPrintInfo::clear()
	{		
		// Destroying objects in lists
		// ---------------------------
		// Deleting variable' print info objects pointed in the list
		for (it_info = allVarsPrintInfo.begin(); it_info != allVarsPrintInfo.end(); ++it_info)
			delete *it_info;
		
		// After destroying objects in list, clearing lists
		// ------------------------------------------------
		// Clearing variables' print info list
		allVarsPrintInfo.clear();
		
		resetInfoIterator();
	}


	VariablePrintInfo* AllVariablesPrintInfo::operator [] (uint i) const { return allVarsPrintInfo[i]; }

	size_t AllVariablesPrintInfo::size() const { return allVarsPrintInfo.size(); }

	bool AllVariablesPrintInfo::isEmpty() const { return size() == 0; }

	// Resetting iterator at the beginning of the list of all variables' print info
	void AllVariablesPrintInfo::resetInfoIterator() { it_info = allVarsPrintInfo.begin(); }

	bool AllVariablesPrintInfo::setPrintStatus(string varname, bool printStatus)
	{
		std::transform(varname.begin(), varname.end(), varname.begin(), ::toupper);

		for (; it_info != allVarsPrintInfo.end(); it_info++)
		{
			string current_var_name = (*it_info)->name();
			std::transform(current_var_name.begin(), current_var_name.end(), current_var_name.begin(), ::toupper);
			if (varname == current_var_name)
			{
				(*it_info)->enablePrint(printStatus);
				return true;
			}
		}
		return false;
	}

	void AllVariablesPrintInfo::prepareForSimulation(bool userSelection)
	{
		assert(!isEmpty() && "The variable print info list must not be empty at this point");
		if (!userSelection)
		{
			for (uint i = 0; i < size(); ++i)
				allVarsPrintInfo[i]->enablePrint(true);
		}

		
		uint CFileLevel = 1;
		uint CDataLevel = 1;

		while (CDataLevel <= Category::maxDataLevel && CFileLevel <= Category::maxFileLevel)
		{
			uint currentColumnsCount = 0;

			// Computing the max number of columns for an output file 
			it_info = allVarsPrintInfo.begin();
			for (; it_info != allVarsPrintInfo.end(); it_info++)
			{
				if ( (*it_info)->isPrinted() && (*it_info)->getFileLevel() & CFileLevel && (*it_info)->getDataLevel() & CDataLevel )
					currentColumnsCount += (*it_info)->getColumnsCount();
			}

			if (currentColumnsCount > columnsCount)
				columnsCount = currentColumnsCount;

			CFileLevel = (CFileLevel * 2 > (int)Category::maxFileLevel) ? 1 : CFileLevel * 2;
			CDataLevel = (CFileLevel * 2 > (int)Category::maxFileLevel) ? CDataLevel * 2 : CDataLevel;
		}

		// Reset iterator on variables print info list after reading ini file
		resetInfoIterator();
	}

	void AllVariablesPrintInfo::find(string var_name)
	{
		for (; it_info != allVarsPrintInfo.end(); it_info++)
		{
			if ((*it_info)->name() == var_name)
			{
				// Now the iterator is pointing the searched variable
				break;
			}
		}
	}

} // namespace Data
} // namespace Antares