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
#ifndef __ANTARES_WINDOWS_OUTPUT_FWD_H__
#define __ANTARES_WINDOWS_OUTPUT_FWD_H__

#include <yuni/yuni.h>
#include <yuni/job/job.h>
#include <vector>
#include <map>
#include <set>

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
class Variables;
class Outputs;
class Comparison;

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
class Layer;
class Component;
class SpotlightProvider;
class SpotlightProviderGlobalSelection;

/*!
** \brief Level of Details
*/
enum LevelOfDetails
{
    lodAllMCYears,
    lodDetailledResults,
    lodDetailedResultsWithConcatenation,
};

/*!
** \brief Type of the selection
*/
enum SelectionType
{
    //! No selection
    stNone = 0,
    //! An area has been selected
    stArea,
    //! A link has been selected
    stLink,
    //! The summary has been selected
    stSummary,
    //! The comments have been selected
    stComments,
    //! The logs have been selected
    stLogs
};

/*!
** \brief All types of layers
*/
enum LayerType
{
    //! A real data file, taken from an output
    ltOutput = 0,
    //! Virtual layer, sum of all non-virtual layers
    ltSum,
    //! Virtual layer, diff of all non-virtual layers
    ltDiff,
    //! Virtual layer, average of all non-virtual layers
    ltAverage,
    //! Virtual layer, min of all non-virtual layers
    ltMin,
    //! Virtual layer, max of all non-virtual layers
    ltMax,
};

} // namespace OutputViewer
} // namespace Window
} // namespace Antares

namespace Antares
{
namespace Private
{
namespace OutputViewerData
{
//! Job for analyzing an output
class Job;

//! Content of a single output
class Content;

//! Content for a given output
using ContentMap = std::map<Yuni::String, Content*>;

//! Smart pointer on a job
using JobPtr = Yuni::Job::IJob::Ptr::Promote<Job>::Ptr;

//! Array of jobs
using JobVector = std::vector<JobPtr>;

//! Output panel
class Panel;

using ThermalNameSet = std::set<Data::ClusterName>;
//! List of thermal clusters, for all areas
using ThermalNameSetPerArea = std::map<Data::AreaName, std::set<Data::ClusterName>>;

} // namespace OutputViewerData
} // namespace Private
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_FWD_H__
