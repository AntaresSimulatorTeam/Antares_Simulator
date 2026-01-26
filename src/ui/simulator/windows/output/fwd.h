// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_FWD_H__
#define __ANTARES_WINDOWS_OUTPUT_FWD_H__

#include <yuni/yuni.h>
#include <yuni/job/job.h>
#include <vector>
#include <map>
#include <set>

namespace Antares::Window::OutputViewer::Provider
{
class Variables;
class Outputs;
class Comparison;

} // namespace Antares::Window::OutputViewer::Provider

namespace Antares::Window::OutputViewer
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

} // namespace Antares::Window::OutputViewer

namespace Antares::Private::OutputViewerData
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

using ThermalNameSet = std::set<YString>;
//! List of thermal clusters, for all areas
using ThermalNameSetPerArea = std::map<Data::AreaName, std::set<YString>>;

} // namespace Antares::Private::OutputViewerData

#endif // __ANTARES_WINDOWS_OUTPUT_FWD_H__
