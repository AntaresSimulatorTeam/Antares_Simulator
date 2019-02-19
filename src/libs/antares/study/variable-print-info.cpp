
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
		is_non_applicable(false), dataLevel(dataLvl), fileLevel(fileLvl)
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

	void variablePrintInfoCollector::add(const AnyString& name, uint nbGlobalResults, uint dataLevel, uint fileLevel, bool possibly_non_applicable)
	{
		allvarsinfo->add(new VariablePrintInfo(name, nbGlobalResults, dataLevel, fileLevel));

		// Storing addresses of variables (~ variables print info objects) that can be non applicable 
		if (possibly_non_applicable)
			allvarsinfo->addPossiblyNonApplicable(allvarsinfo->getLast());
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
		varsPossiblyNonApplicable.clear();
		
		// Destroying objects in lists
		// ---------------------------
		// Deleting variable' print info objects pointed in the list
		for (it_info = allVarsPrintInfo.begin(); it_info != allVarsPrintInfo.end(); ++it_info)
			delete *it_info;

		// Deleting each (possibly non applicable) variables' print info objects pointed in the list
		for (it_info = varsPossiblyNonApplicable.begin(); it_info != varsPossiblyNonApplicable.end(); ++it_info)
			delete *it_info;
		
		// After destroying objects in list, clearing lists
		// ------------------------------------------------
		// Clearing variables' print info list
		allVarsPrintInfo.clear();
		//  Clearing possibly non applicable variables' print info list
		varsPossiblyNonApplicable.clear();

		
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

	void AllVariablesPrintInfo::addPossiblyNonApplicable(VariablePrintInfo* v)
	{
		varsPossiblyNonApplicable.push_back(v);
	}

	void AllVariablesPrintInfo::setToNonApplicable(bool na)
	{
		// Setting all variables that can be non applicable to status : N/A
		vector<VariablePrintInfo*>::iterator it = varsPossiblyNonApplicable.begin();
		for (; it != varsPossiblyNonApplicable.end(); it++)
			(*it)->setNonApplicableStatus(na);
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

	void AllVariablesPrintInfo::setNonApplicableStatusForEachVariable()
	{
		// If at least one area is such as reservoir management is turned off, then
		// some hydro variables must be set to status : non applicable
		auto& study = *Study::Current::Get();	// Get the study object
		bool all_vars_applicable = true;
		for (uint i = 0; i < study.areas.size(); ++i)
		{
			auto& area = *study.areas.byIndex[i];
			if (!area.hydro.reservoirManagement)
			{
				all_vars_applicable = false;
				break;
			}
		}

		setToNonApplicable(!all_vars_applicable);
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