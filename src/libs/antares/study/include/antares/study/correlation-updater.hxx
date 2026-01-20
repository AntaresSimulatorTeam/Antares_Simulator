// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    CorrelationUpdater(Data::Study& study):
        pStudy(study)
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
        pStudy.preproLoadCorrelation.loadFromFile(pStudy, pS, false);
        IO::File::Delete(pS);

        pS.clear() << pTempFile << "solar.tmp";
        pStudy.preproSolarCorrelation.loadFromFile(pStudy, pS, false);
        IO::File::Delete(pS);

        pS.clear() << pTempFile << "wind.tmp";
        pStudy.preproWindCorrelation.loadFromFile(pStudy, pS, false);
        IO::File::Delete(pS);

        pS.clear() << pTempFile << "hydro.tmp";
        pStudy.preproHydroCorrelation.loadFromFile(pStudy, pS, false);
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
