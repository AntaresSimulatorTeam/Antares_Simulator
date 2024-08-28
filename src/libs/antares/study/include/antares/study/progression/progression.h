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
#ifndef __ANTARES_LIBS_SOLVER_SIMULATION_PROGRESSION_H__
#define __ANTARES_LIBS_SOLVER_SIMULATION_PROGRESSION_H__

#include <atomic>
#include <list>
#include <map>
#include <mutex>
#include <vector>

#include <yuni/core/singleton.h>
#include <yuni/io/file.h>
#include <yuni/thread/timer.h>

#include <antares/writer/i_writer.h>

#include "../fwd.h"

namespace Antares
{
namespace Solver
{
/*!
** \brief Progress meter about any operation performed on the attached study
*/
class Progression final
{
public:
    enum Section
    {
        sectYear = 0,
        sectOutput,
        sectTSGLoad,
        sectTSGSolar,
        sectTSGWind,
        sectTSGHydro,
        sectTSGThermal,
        sectImportTS,
        sectMax
    };

    enum
    {
        npos = (uint)-1,
    };

    const char* SectionToCStr(Section section);

private:
    class Part final
    {
    public:
        using MapPerSection = std::map<Section, Part>;
        using Map = std::map<uint, MapPerSection>;
        // using ListRef = Yuni::LinkedList<Part*>;
        using ListRef = std::list<Part*>;

    public:
        //! The total number of ticks to achieve
        unsigned maxTickCount;
        //! The current number of ticks
        std::atomic<unsigned> tickCount;
        //! The last number of ticks, to reduce the log verbosity
        unsigned lastTickCount;
        // Caption to use when displaying logs
        // Example: 'year: 10000, task: thermal'
        Yuni::CString<40, false> caption;
    };

public:
    class Task final
    {
    public:
        Task(const Antares::Data::Study& study, Section section);
        Task(const Antares::Data::Study& study, uint year, Section section);

        ~Task()
        {
            pProgression.end(pPart);
        }

        Task& operator++()
        {
            ++pPart.tickCount;
            return *this;
        }

        Task& operator+=(unsigned value)
        {
            pPart.tickCount += value;
            return *this;
        }

    private:
        Progression& pProgression;
        Part& pPart;
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \name Default constructor
    */
    Progression();
    /*!
    ** \brief Destructor
    */
    ~Progression();
    //@}

    /*!
    ** \brief Declare a new part of the progression
    **
    ** \param year The associated year
    ** \param section The section in this year
    ** \param nbTicks The total number of ticks to achieve
    **
    ** \internal The number of ticks should remain an `int` because
    **   we can not use unsigned atomic integer
    */
    void add(uint year, Section section, unsigned nbTicks);

    void add(Section section, int nbTicks);

    bool saveToFile(const Yuni::String& filename, IResultWriter& writer);

    void setNumberOfParallelYears(uint nb);

    //! \name Thread management
    //@{
    /*!
    ** \brief Start the thread dedicated to the progress notification
    */
    void start();

    /*!
    ** \brief Stop the thread dedicated to the progress notification
    */
    void stop();
    //@}

protected:
    Part& begin(uint year, Section section);
    void end(Part& part);

private:
    class Meter final: public Yuni::Thread::Timer
    {
    public:
        Meter();

        virtual ~Meter() = default;

        /*!
        ** \brief Prepare enough space to allow @n simultaneous tasks
        */
        void taskCount(uint n);

    protected:
        virtual bool onInterval(uint) override;

    public:
        Progression::Part::Map parts;
        Part::ListRef inUse;
        std::mutex mutex;
        uint nbParallelYears;

        std::vector<Yuni::CString<256, false>> logsContainer;

    }; // class Meter

private:
    //! A dedicated thread for displaying the progression
    Meter pProgressMeter;
    //! Flag to indicate if the progress meter has been started
    // This variable has no need to be thread-safe because initialized at the very beginning
    bool pStarted;
    // Friend
    friend class Task;

}; // class Progression

} // namespace Solver
} // namespace Antares

#include "progression.hxx"

#endif // __ANTARES_LIBS_SOLVER_SIMULATION_PROGRESSION_H__
