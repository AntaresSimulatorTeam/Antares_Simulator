// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_JOB_H__
#define __ANTARES_WINDOWS_OUTPUT_JOB_H__

#include <yuni/yuni.h>
#include <yuni/job/job.h>
#include <antares/study/fwd.h>
#include "output.h"
#include "content.h"

namespace Antares::Private::OutputViewerData
{
class Job final: public Yuni::Job::IJob
{
public:
    /*!
    ** \brief Constructor
    */
    Job(Antares::Window::OutputViewer::Component& component, const AnyString& path);
    //! Destructor
    virtual ~Job();

protected:
    virtual void onExecute() override;

    //! Gather informations about year-by-year data
    void gatherInfosAboutYearByYearData(const AnyString& path);

    //! Gather informations about thermal clusters
    void gatherInfosAboutThermalClusters(const AnyString& path);

private:
    Antares::Window::OutputViewer::Component& pComponent;
    //! The path to analyze
    const YString pPath;
    //!
    Content* pContent;

}; // class Job

} // namespace Antares::Private::OutputViewerData

#endif // __ANTARES_WINDOWS_OUTPUT_JOB_H__
