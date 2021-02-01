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
#ifndef __STUDY_JOB_AGGREGATOR_JOB_H__
#define __STUDY_JOB_AGGREGATOR_JOB_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/job/job.h>
#include "datafile.h"
#include "output.h"
#include "studydata.h"
#include <yuni/job/queue/service.h>
#include <yuni/io/file.h>

class JobFileReader final : public Yuni::Job::IJob
{
public:
    //! The most suitable smart pointer
    typedef Yuni::SmartPtr<JobFileReader> Ptr;
    enum
    {
        maxRows = 8800,
    };

    //! Get if some jobs remain
    static bool RemainJobsToExecute();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    JobFileReader();
    //! Destructor
    virtual ~JobFileReader();
    //@}

public:
    //! The current year (zero-based)
    uint year;
    //! Data file
    DataFile::Ptr datafile;
    //! Output
    Output::Ptr output;
    //! Study data
    StudyData::Ptr studydata;
    //! Path
    Yuni::String path;

protected:
    /*!
    ** \brief Execute the job
    **
    ** The job consists in reading a single CSV file from one of the
    ** numerous 'mc-i<year>' and to keep the results on its reading
    ** into the variable 'results' available in the output structure.
    */
    virtual void onExecute() override;

private:
    /*!
    ** \brief Try to open the CSV file
    */
    bool openCSVFile();
    /*!
    ** \brief Prepare the `jump table`
    */
    bool prepareJumpTable();

    /*!
    ** \brief Read the raw data from the CSV file
    */
    bool readRawData();

    void readLine(const AnyString& line, uint y);

    bool storeResults();

    //! Reset the jump table
    void resizeJumpTable(uint newsize);

private:
    //! Type for a temporary column
    typedef CellData* TemporaryColumnData;
    //! Buffer type
    typedef Yuni::CString<65536> BufferType;
    //! Jump table
    typedef std::vector<uint> JumpTable;

private:
    //! Buffer for reading data
    BufferType buffer;
    //! Stream
    Yuni::IO::File::Stream pFile;
    //! CSV filename
    Yuni::String pFilename;
    //! Jump table
    JumpTable pJumpTable;
    //! Flag to know if a variable has been found
    bool* pVariablesOn;
    //! Offset of the first data
    uint pDataOffset;
    //! Temporary results
    TemporaryColumnData* pTmpResults;
    //! The total number of lines found
    uint pLineCount;

}; // class JobFileReader

//! The queue service
extern Yuni::Job::QueueService queueService;

#include "job.hxx"

#endif // __STUDY_JOB_AGGREGATOR_JOB_H__
