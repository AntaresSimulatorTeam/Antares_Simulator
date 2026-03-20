// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __STUDY_JOB_AGGREGATOR_JOB_HXX__
#define __STUDY_JOB_AGGREGATOR_JOB_HXX__

inline void JobFileReader::resizeJumpTable(uint newsize)
{
    pJumpTable.clear();
    pJumpTable.resize(newsize);
    for (uint i = 0; i != newsize; ++i)
    {
        pJumpTable[i] = (uint)-1;
    }
}

#endif // __STUDY_JOB_AGGREGATOR_JOB_HXX__
