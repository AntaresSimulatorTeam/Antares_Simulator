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
#ifndef __ANTARES_LIBS_STUDY_CORRELATION_UPDATER_HXX__
#define __ANTARES_LIBS_STUDY_CORRELATION_UPDATER_HXX__

#include <yuni/yuni.h>

#define SEP IO::Separator

namespace Antares
{
namespace // anonymous
{
class CorrelationUpdater final
{
public:
    CorrelationUpdater(Data::Study& study) : pStudy(study)
    {
        using namespace Yuni;
        // We can store the INI files in disk because it may not fit in memory
        pTempFile << memory.cacheFolder() << SEP << "antares-corr-save-" << memory.processID()
                  << '-' << (void*)(this) << '-';

        pS.clear() << pTempFile << "load.tmp";
        study.preproLoadCorrelation.saveToFile(study, pS);
        pS.clear() << pTempFile << "solar.tmp";
        study.preproSolarCorrelation.saveToFile(study, pS);
        pS.clear() << pTempFile << "wind.tmp";
        study.preproWindCorrelation.saveToFile(study, pS);
        pS.clear() << pTempFile << "hydro.tmp";
        study.preproHydroCorrelation.saveToFile(study, pS);
    }

    ~CorrelationUpdater()
    {
        using namespace Yuni;
        pS.clear() << pTempFile << "load.tmp";
        pStudy.preproLoadCorrelation.loadFromFile(pStudy, pS, false, Data::versionLatest);
        IO::File::Delete(pS);

        pS.clear() << pTempFile << "solar.tmp";
        pStudy.preproSolarCorrelation.loadFromFile(pStudy, pS, false, Data::versionLatest);
        IO::File::Delete(pS);

        pS.clear() << pTempFile << "wind.tmp";
        pStudy.preproWindCorrelation.loadFromFile(pStudy, pS, false, Data::versionLatest);
        IO::File::Delete(pS);

        pS.clear() << pTempFile << "hydro.tmp";
        pStudy.preproHydroCorrelation.loadFromFile(pStudy, pS, false, Data::versionLatest);
        IO::File::Delete(pS);
    }

private:
    Data::Study& pStudy;
    Yuni::Clob pTempFile;
    Yuni::Clob pS;

}; // class CorrelationUpdater

} // anonymous namespace

} // namespace Antares

#endif //  __ANTARES_LIBS_STUDY_CORRELATION_UPDATER_HXX__
