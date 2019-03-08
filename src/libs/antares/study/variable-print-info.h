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

#ifndef __SOLVER_VARIABLE_PRINT_POLICY_H__
# define __SOLVER_VARIABLE_PRINT_POLICY_H__


# include<string>
# include<vector>
# include<map>
# include <yuni/yuni.h>
# include <yuni/core/fwd.h>
# include <yuni/core/string.h>

using namespace std;
// using namespace Yuni;

namespace Antares
{
namespace Data
{

	class VariablePrintInfo
	{
	public:
		VariablePrintInfo(AnyString vname, uint maxNbCols, uint dataLvl, uint fileLvl);
		~VariablePrintInfo() {};

		string name();

		void enablePrint(bool b);
		bool isPrinted();

		uint getColumnsCount();
		uint getDataLevel() { return dataLevel; }
		uint getFileLevel() { return fileLevel; }

	private:
		// Current variable's name
		AnyString varname;
		// Is the variable printed ?
		bool to_be_printed;

		// All this useful to compute the max number of colums any report can contain
		// ... maximum number of columns taken by variable in the output files
		uint maxNumberColumns;
		// ... data and file levels
		uint dataLevel;
		uint fileLevel;
	};

	class AllVariablesPrintInfo;

	class variablePrintInfoCollector
	{
	public:
		variablePrintInfoCollector(AllVariablesPrintInfo * allvarsprintinfo);
		void add(const AnyString& name, uint nbGlobalResults, uint dataLevel, uint fileLevel/*, bool possibly_non_applicable*/);
	private:
		AllVariablesPrintInfo * allvarsinfo;
	};

	// gp : var select :
	// gp : - mot clé const ?
	// gp : - mot clé inline ?
	// gp : - autres ?
	class AllVariablesPrintInfo
	{
	public:
		// Public methods
		AllVariablesPrintInfo();
		~AllVariablesPrintInfo();

		void add(VariablePrintInfo * v);
		void clear();
		VariablePrintInfo* operator [] (uint i) const;
		size_t size() const;
		bool isEmpty() const;

		void resetInfoIterator();
		
		bool setPrintStatus(string varname, bool printStatus);

		inline VariablePrintInfo* getLast() { return allVarsPrintInfo.back(); }
		inline VariablePrintInfo* getCurrentPrintInfo() { return *it_info; }

		void prepareForSimulation(bool userSelection);

		void find(string var_name);

		// Get a flag inside the variable object pointed by the iterator
		inline bool isPrinted() { return (*it_info)->isPrinted(); };

		uint getColumnsCount() { return columnsCount; }

	private:
		// Contains print info for all variables
		vector<VariablePrintInfo*> allVarsPrintInfo;
		
		// Iterator on variable print info list
		vector<VariablePrintInfo*>::iterator it_info;

		uint columnsCount;
	};

} // namespace Data
} // namespace Antares


#endif // __SOLVER_VARIABLE_PRINT_POLICY_H__