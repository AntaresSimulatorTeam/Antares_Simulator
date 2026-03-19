// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __STUDY_JOB_AGGREGATOR_JOB_H__
#define __STUDY_JOB_AGGREGATOR_JOB_H__

#include <memory>

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/io/file.h>
#include <yuni/job/job.h>
#include <yuni/job/queue/service.h>

#include "antares/solver/ts-generator/xcast/studydata.h"

#include "datafile.h"
#include "output.h"

class JobFileReader final: public Yuni::Job::IJob
{
public:
    //! The most suitable smart pointer
    using Ptr = std::shared_ptr<JobFileReader>;

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
    using TemporaryColumnData = CellData*;
    //! Buffer type
    using BufferType = Yuni::CString<65536>;
    //! Jump table
    using JumpTable = std::vector<uint>;

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
