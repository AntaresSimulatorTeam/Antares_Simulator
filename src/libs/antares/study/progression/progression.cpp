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

#include "antares/study/progression/progression.h"

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Solver
{
Progression::Task::Task(const Antares::Data::Study& study, Section section):
    pProgression(study.progression),
    pPart(study.progression.begin((uint)-1, section))
{
    assert(&pProgression);
}

Progression::Task::Task(const Antares::Data::Study& study, uint year, Section section):
    pProgression(study.progression),
    pPart(study.progression.begin(year, section))
{
    assert(&pProgression);
}

void Progression::add(uint year, Section section, unsigned nbTicks)
{
    // This section is not thread-safe because always called before really launching
    // the simulation

    Part& part = pProgressMeter.parts[year][section];
    // reset
    part.maxTickCount = nbTicks;
    part.lastTickCount = 0;

    // Caption
    part.caption.clear() << "task 0 " << SectionToCStr(section) << ", ";
    if (year != (uint)-1)
    {
        part.caption << "year: " << year << ", ";
    }
    else
    {
        part.caption << "post, ";
    }
}

void Progression::end(Part& part)
{
    // Removing the task from the active task list
    pProgressMeter.mutex.lock();

#ifndef NDEBUG
    {
        bool gotcha = false;
        const auto end = pProgressMeter.inUse.end();
        for (auto i = pProgressMeter.inUse.begin(); i != end; ++i)
        {
            if (*i == &part)
            {
                gotcha = true;
                break;
            }
        }
        if (not gotcha)
        {
            assert(false && "Trying to remove an unknown progression part");
            pProgressMeter.mutex.unlock();
            return;
        }
    }
#endif

    pProgressMeter.inUse.remove(&part);

    // 100% - We have to display the '100%' whatever the case to make sure that
    // the UI is aware that the sub-task is complete
    if (part.maxTickCount != part.lastTickCount)
    {
        pProgressMeter.mutex.unlock();

        // Only if started. This variable has no need to be modified because
        // never modified after the start of the simulation
        if (pStarted)
        {
            logs.progress() << part.caption << "100";
        }
        return;
    }
    else
    {
        pProgressMeter.mutex.unlock();
    }
}

bool Progression::Meter::onInterval(uint)
{
    uint count = 0;

    mutex.lock();
    if (inUse.empty())
    {
        mutex.unlock();
        return true;
    }

    {
        const auto end = inUse.end();
        for (auto i = inUse.begin(); i != end; ++i)
        {
            Part& part = *(*i);
            if (part.tickCount != part.lastTickCount)
            {
                part.lastTickCount = part.tickCount;
                logsContainer[count].clear()
                  << part.caption << ((part.lastTickCount * 100.f) / part.maxTickCount);
                ++count;
            }
        }
    }
    mutex.unlock();

    // Print all logs
    for (uint i = 0; i != count; ++i)
    {
        logs.progress() << logsContainer[i];
    }

    // True to continue the execution of the timer
    return true;
}

Progression::Progression():
    pStarted(false)
{
}

Progression::~Progression()
{
    pProgressMeter.stop();
}

bool Progression::saveToFile(const Yuni::String& filename, IResultWriter& writer)
{
    Yuni::Clob buffer;
    std::lock_guard locker(pProgressMeter.mutex);
    {
        uint year;
        const Part::Map::const_iterator end = pProgressMeter.parts.end();
        for (Part::Map::const_iterator i = pProgressMeter.parts.begin(); i != end; ++i)
        {
            year = i->first;
            const Part::MapPerSection::const_iterator jend = i->second.end();
            for (Part::MapPerSection::const_iterator j = i->second.begin(); j != jend; ++j)
            {
                if (year != (uint)-1)
                {
                    buffer << year;
                }
                else
                {
                    buffer << "post";
                }
                buffer << ' ' << SectionToCStr(j->first) << '\n';
            }
        }
    }
    writer.addEntryFromBuffer(filename.c_str(), buffer);
    logs.info() << LOG_UI_PROGRESSION_MAP << filename;
    return true;
}

void Progression::start()
{
    pStarted = true;
    pProgressMeter.interval(1200 /*ms*/);
    pProgressMeter.start();
}

void Progression::stop()
{
    pProgressMeter.stop();
    pStarted = false;
}

} // namespace Antares::Solver
