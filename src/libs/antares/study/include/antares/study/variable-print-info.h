/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#ifndef __SOLVER_VARIABLE_PRINT_POLICY_H__
#define __SOLVER_VARIABLE_PRINT_POLICY_H__

#include <string>
#include <vector>
#include <map>
#include <yuni/yuni.h>
#include <yuni/core/fwd.h>
#include <yuni/core/string.h>

namespace Antares
{
namespace Data
{
// Represents an output variable (wears the same name) and mainly answers the question :
// Is the real variable printed in all output reports ? Or is it not printed in any report ?
class VariablePrintInfo
{
public:
    VariablePrintInfo(uint dataLvl, uint fileLvl);
    ~VariablePrintInfo() = default;

    // Do we enable or disable variable's print in output reports ?
    void enablePrint(bool b);
    bool isPrinted() const;
    void reverse();

    uint getMaxColumnsCount();
    void setMaxColumns(uint maxColumnsNumber);

    bool isPrintedOnDataLevel(uint dataLevel) const
    {
        return dataLevel_ & dataLevel;
    }

    bool isPrintedOnFileLevel(uint fileLevel) const
    {
        return fileLevel_ & fileLevel;
    }

private:
    // Is the variable printed ?
    bool to_be_printed_ = true;

    // The number of columns the output variable takes in a SYNTHESIS report.
    // Recall that synthesis reports always contain more columns than
    // any other reports (for instance year-by-year reports)
    uint maxNumberColumns_ = 0;

    // Which reports the output variable has columns in ?
    // Example : areas/values-<time-interval>.txt
    // dataLevel can be : areas, links, bindingConstraint
    // fileLevel can be : values-<time-interval>.txt, details-<time-interval>.txt,
    // id-<time-interval>.txt, ...
    uint dataLevel_ = 0;
    uint fileLevel_ = 0;
};

class AllVariablesPrintInfo;

class variablePrintInfoCollector
{
public:
    variablePrintInfoCollector(AllVariablesPrintInfo* allvarsprintinfo);
    void add(const AnyString& name, uint dataLevel, uint fileLevel);

private:
    AllVariablesPrintInfo* allvarsinfo;
};

// Variables print info collection. Mainly a vector of pointers to print info.
// This collection is filled with as many print info as we can find output variables in the output
// variables Antares's static list.
class AllVariablesPrintInfo
{
public:
    // Public methods
    AllVariablesPrintInfo() = default;
    ~AllVariablesPrintInfo() = default;

    void add(std::string name, VariablePrintInfo v);
    void clear();
    VariablePrintInfo& operator[](uint i);
    size_t size() const;
    bool exists(std::string name);

    void setPrintStatus(std::string varname, bool printStatus);
    void setPrintStatus(unsigned int index, bool printStatus);

    void setMaxColumns(std::string varname, uint maxColumnsNumber);
    std::string name_of(unsigned int index) const;

    void prepareForSimulation(bool isThematicTrimmingEnabled,
                              const std::vector<std::string>& excluded_vars = {});

    // Classic search, then get the print status
    bool isPrinted(std::string var_name) const;

    uint getTotalMaxColumnsCount() const
    {
        return totalMaxColumnsCount_;
    }

    uint getNbSelectedZonalVars() const
    {
        return numberSelectedAreaVariables;
    }

    uint getNbSelectedLinkVars() const
    {
        return numberSelectedLinkVariables;
    }

    void computeMaxColumnsCountInReports();
    void setAllPrintStatusesTo(bool b);
    void reverseAll();

    unsigned int numberOfEnabledVariables();
    std::vector<std::string> namesOfEnabledVariables();
    std::vector<std::string> namesOfDisabledVariables();

private:
    std::vector<std::string> namesOfVariablesWithPrintStatus(bool printStatus);
    void countSelectedAreaVars();
    void countSelectedLinkVars();

private:
    // Contains print info for all variables
    std::map<std::string, VariablePrintInfo> allVarsPrintInfo;
    std::map<unsigned int, std::string> index_to_name;

    // Max columns count a report of any kind can contain, depending on the number of selected
    // variables. The less variables are selected, the smallest this count is.
    uint totalMaxColumnsCount_ = 0;

    // Number of selected zonal variables
    uint numberSelectedAreaVariables = 0;
    // Number of selected link variables
    uint numberSelectedLinkVariables = 0;
};

} // namespace Data
} // namespace Antares

#endif // __SOLVER_VARIABLE_PRINT_POLICY_H__
