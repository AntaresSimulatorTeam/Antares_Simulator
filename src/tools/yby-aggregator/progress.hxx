// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __STUDY_JOB_AGGREGATOR_PROGRESS_HXX__
#define __STUDY_JOB_AGGREGATOR_PROGRESS_HXX__

inline Progress::~Progress()
{
}

template<class StringT>
void Progress::message(const StringT& msg)
{
    using namespace Antares;
    logs.info() << msg;
    ThreadingPolicy::MutexLocker locker(*this);
    pMessage = msg;
}

#endif // __STUDY_JOB_AGGREGATOR_PROGRESS_HXX__
