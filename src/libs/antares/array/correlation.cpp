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

#include <yuni/yuni.h>
#include "../study/study.h"
#include "../inifile/inifile.h"
#include "../logs.h"
#include "correlation.h"
#include <math.h>
#include "../study/memory-usage.h"

using namespace Yuni;
using namespace Antares;

namespace Antares
{
namespace Data
{
static inline void InterAreaCorrelationSetValue(Matrix<>& m,
                                                const char filename[],
                                                Area* from,
                                                Area* to,
                                                IniFile::Section* s,
                                                IniFile::Property* p)
{
    double v;

    // Same area ? This value can not be modified
    if (to == from)
    {
        logs.warning()
          << filename << ": '" << s->name << "'/'" << p->key << "'='" << p->value
          << "': This value of correlation is locked (equal to 1.0). The property is ignored";
        return;
    }
    // Multiple correlation valueis
    if (m[from->index][to->index] != 0.)
    {
        logs.warning()
          << filename << ": '" << s->name << "'/'" << p->key << "'='" << p->value
          << "': This correlation value is already set and will override the previous value";
    }
    // Try to convert the value
    if (!p->value.to<double>(v))
    {
        logs.warning() << filename << ": '" << s->name << "'/'" << p->key << "'='" << p->value
                       << "': A floatting point value is required. The property is ignored";
        return;
    }
    // Out of bounds
    if (v < -1. or v > 1.)
    {
        logs.error() << filename << ": '" << s->name << "'/'" << p->key << "'='" << p->value
                     << "': A correlation coefficient must be between -1.0 and +1.0";
        return;
    }
    // The same for the opposite
    m[from->index][to->index] = v;
    m[to->index][from->index] = v;
}

static inline bool ReadCorrelationModeFromINI(const IniFile& ini,
                                              Correlation::Mode& mode,
                                              bool warnings)
{
    auto* section = ini.find("general");
    if (!section)
    {
        if (warnings)
            logs.error() << ini.filename() << ": the section 'general' is missing";
        return false;
    }
    const IniFile::Property* p = section->find("mode");
    if (!p)
    {
        if (warnings)
            logs.error() << ini.filename() << ": the property 'mode' is missing";
        return false;
    }
    mode = Correlation::CStringToMode(p->value.c_str());
    if (mode == Correlation::modeNone)
    {
        if (warnings)
            logs.error() << ini.filename() << ": invalid correlation mode";
        return false;
    }
    return true;
}

static inline void ReadCorrelationCoefficients(Correlation& correlation,
                                               Study& study,
                                               Matrix<>& m,
                                               const IniFile& ini,
                                               const IniFile::Section& section,
                                               bool warnings)
{
    double v;
    AreaName nameFrom;
    AreaName nameTo;

    for (auto* p = section.firstProperty; p; p = p->next)
    {
        if (p->key.empty())
            continue;

        const AreaName::Size offset = p->key.find('%');
        if (offset == AreaName::npos or !offset or offset == p->key.size() - 1)
        {
            logs.error() << ini.filename() << ": '" << p->key << "': invalid token";
            continue;
        }
        nameFrom.assign(p->key.c_str(), offset);
        nameTo.assign(p->key.c_str() + offset + 1, p->key.size() - (offset + 1));

        auto* from = study.areas.find(nameFrom);
        auto* to = study.areas.find(nameTo);

        if (!from)
        {
            if (warnings)
                logs.error() << ini.filename() << ": '" << nameFrom
                             << "': impossible to find the area";
            continue;
        }
        if (!to)
        {
            if (warnings)
                logs.error() << ini.filename() << ": '" << nameTo
                             << "': impossible to find the area";
            continue;
        }

        if (p->value.to<double>(v))
        {
            correlation.set(m, *from, *to, v);
        }
        else
        {
            if (warnings)
            {
                logs.warning() << ini.filename() << ": section '" << section.name
                               << "': invalid coefficient for the link '" << from->id << "' -> '"
                               << to->id << "'";
            }
        }
    }
}

static inline void ExportCorrelationCoefficients(Study& study,
                                                 const Matrix<>& m,
                                                 IO::File::Stream& file,
                                                 const char* name)
{
    if (m.empty() or m.width != m.height)
        return;

    file << '[' << name << "]\n";

    // For each column
    for (uint x = 0; x != m.width; ++x)
    {
        const AreaName& from = study.areas.byIndex[x]->id;

        auto& col = m.entry[x];
        for (uint y = 0; y < x; ++y)
        {
            if (!Math::Zero(col[y]))
            {
                file << from << '%' << study.areas.byIndex[y]->id << " = " << col[y] << '\n';
            }
        }
    }

    file << '\n';
}

int InterAreaCorrelationLoadFromIniFile(Matrix<>* m, AreaList* l, IniFile* ini, int warnings)
{
    // Asserts
    assert(m);
    assert(l);

    if (ini)
    {
        IniFile::Section* s;
        for (s = ini->firstSection; s != NULL; s = s->next) /* Each section */
        {
            Area* from = AreaListLFind(l, s->name.c_str());
            if (from)
            {
                IniFile::Property* p;
                for (p = s->firstProperty; p != NULL; p = p->next) /* Each property*/
                {
                    Area* to = AreaListLFind(l, p->key.c_str());
                    if (to and to != from)
                    {
                        // Set the value
                        InterAreaCorrelationSetValue(*m, "<memory>", from, to, s, p);
                    }
                    else /* nullptr != to */
                    {
                        if (warnings)
                            logs.warning()
                              << "Correlation: " << s->name << ": " << p->key << ": Area not found";
                    }
                }
            }
            else /* nullptr != from */
            {
                if (warnings)
                    logs.warning() << "Correlation: " << s->name << ": Area not found";
            }
        }
        return 1;
    }
    return 0;
}

int InterAreaCorrelationLoadFromFile(Matrix<>* m, AreaList* l, const char filename[])
{
    /* Asserts */
    assert(m);
    assert(l);
    assert(filename);

    InterAreaCorrelationResetMatrix(m, l);
    IniFile ini;
    if (ini.open(filename))
    {
        return InterAreaCorrelationLoadFromIniFile(m, l, &ini, 1);
    }
    logs.error() << filename << ": Impossible to load the file";
    return 0;
}

IniFile* InterAreaCorrelationSaveToIniFile(const Matrix<>* m, const AreaList* l)
{
    /* Asserts */
    assert(m);
    assert(l);

    /* The Ini File structure */
    IniFile* ini = new IniFile();

    if (m->width != l->size() or m->height != l->size())
    {
        logs.error() << "Correlation: The matrix does not have the good sie (" << l->size() << 'x'
                     << l->size() << " expected, got " << m->width << 'x' << m->height << ')';
    }
    else
    {
        uint x;
        uint y;
        IniFile::Section* s;
        for (x = 1; x < l->size(); ++x)
        {
            s = ini->addSection(l->byIndex[x]->id);
            auto& col = m->entry[x];
            for (y = 0; y < x; ++y)
            {
                if (fabs(col[y]) > 0.00000001)
                    s->add(l->byIndex[y]->id, col[y]);
            }
        }
    }
    return ini;
}

int InterAreaCorrelationSaveToFile(const Matrix<>* m, const AreaList* l, const char filename[])
{
    /* Asserts */
    assert(m);
    assert(l);

    if (!filename or '\0' == *filename)
        return 0;
    if (m->width != l->size() or m->height != l->size())
    {
        logs.error() << filename << ": The matrix does not have the good sie (" << l->size() << 'x'
                     << l->size() << " expected, got " << m->width << 'x' << m->height << ')';
    }
    else
    {
        IniFile* ini = InterAreaCorrelationSaveToIniFile(m, l);
        ini->save(filename);
        delete ini;
        return 1;
    }
    return 0;
}

Correlation::Correlation() : annual(nullptr), monthly(nullptr), pMode(modeNone)
{
}

Correlation::~Correlation()
{
    delete annual;
    delete[] monthly;
}

bool Correlation::loadFromFile(Study& study, const AnyString& filename, bool warnings, int version)
{
#ifndef NDEBUG
    Antares::logs.debug() << "  " << correlationName << ": loading " << filename;
#endif
    IniFile ini;
    return (ini.open(filename)) ? internalLoadFromINI(study, ini, warnings, version) : false;
}

bool Correlation::saveToFile(Study& study, const AnyString& filename) const
{
    using namespace Yuni;
    IO::File::Stream file;
    if (file.openRW(filename))
    {
        internalSaveToINI(study, file);
        return true;
    }
    return false;
}

const char* Correlation::ModeToCString(Mode mode)
{
    switch (mode)
    {
    case modeAnnual:
        return "annual";
    case modeMonthly:
        return "monthly";
    case modeNone:
        return "unknown";
    }
    return "unknown";
}

Correlation::Mode Correlation::CStringToMode(const AnyString& str)
{
    ShortString64 s(str);
    s.trim(" \t\r\n");
    s.toLower();
    if (s == "annual")
        return modeAnnual;
    if (s == "monthly")
        return modeMonthly;
    return modeNone;
}

void Correlation::internalSaveToINI(Study& study, IO::File::Stream& file) const
{
    // General settings
    // (the only mandatory section)
    // mode
    file << "[general]\nmode = " << ModeToCString(pMode) << "\n\n";

    if (annual)
    {
        ExportCorrelationCoefficients(study, *annual, file, "annual");
    }
    else
        logs.error() << correlationName << ": the annual correlation coefficients are missing";

    if (monthly)
    {
        ExportCorrelationCoefficients(study, monthly[0], file, "0");
        ExportCorrelationCoefficients(study, monthly[1], file, "1");
        ExportCorrelationCoefficients(study, monthly[2], file, "2");
        ExportCorrelationCoefficients(study, monthly[3], file, "3");
        ExportCorrelationCoefficients(study, monthly[4], file, "4");
        ExportCorrelationCoefficients(study, monthly[5], file, "5");
        ExportCorrelationCoefficients(study, monthly[6], file, "6");
        ExportCorrelationCoefficients(study, monthly[7], file, "7");
        ExportCorrelationCoefficients(study, monthly[8], file, "8");
        ExportCorrelationCoefficients(study, monthly[9], file, "9");
        ExportCorrelationCoefficients(study, monthly[10], file, "10");
        ExportCorrelationCoefficients(study, monthly[11], file, "11");
    }
    else
        logs.error() << correlationName << ": the montlhy correlation coefficients are missing";
}

bool Correlation::internalLoadFromINIPost32(Study& study, const IniFile& ini, bool warnings)
{
    if (!ReadCorrelationModeFromINI(ini, pMode, warnings))
        return false;

    if (JIT::usedFromGUI or pMode == modeAnnual)
    {
        annual = new Matrix<>();
        annual->resize(study.areas.size(), study.areas.size());
        annual->fillUnit();

        auto* section = ini.find("annual");
        if (section) // the section might be missing
            ReadCorrelationCoefficients(*this, study, *annual, ini, *section, warnings);
    }

    if (JIT::usedFromGUI or pMode == modeMonthly)
    {
        monthly = new Matrix<>[12];
        for (uint i = 0; i < 12; ++i)
        {
            monthly[i].resize(study.areas.size(), study.areas.size());
            monthly[i].fillUnit();
        }

        int index;
        for (auto* section = ini.firstSection; section; section = section->next)
        {
            if (section->name.to<int>(index))
            {
                if (index >= 0 and index < 12)
                {
                    ReadCorrelationCoefficients(
                      *this, study, monthly[index], ini, *section, warnings);
                }
                else
                    logs.error() << "Invalid month index: " << index;
            }
        }
    }

    return true;
}

void Correlation::reset(Study& study)
{
    // Clean
    if (annual)
    {
        delete annual;
        annual = nullptr;
    }
    if (monthly)
    {
        delete[] monthly;
        monthly = nullptr;
    }

    pMode = modeAnnual;
    if (JIT::usedFromGUI)
    {
        // Reset
        annual = new Matrix<>();
        annual->resize(study.areas.size(), study.areas.size());
        annual->fillUnit();

        // Preparing the monthly correlation matrices
        monthly = new Matrix<>[12];
        for (int i = 0; i < 12; ++i)
        {
            monthly[i].resize(study.areas.size(), study.areas.size());
            monthly[i].fillUnit();
        }
    }
    else
    {
        annual = new Matrix<>();
        annual->resize(study.areas.size(), study.areas.size());
        annual->fillUnit();
    }
}

void Correlation::clear()
{
    // Clean
    if (annual)
    {
        delete annual;
        annual = nullptr;
    }
    if (monthly)
    {
        delete[] monthly;
        monthly = nullptr;
    }
}

bool Correlation::internalLoadFromINI(Study& study, const IniFile& ini, bool warnings, int version)
{
    // Clean
    if (annual)
    {
        delete annual;
        annual = nullptr;
    }
    if (monthly)
    {
        delete[] monthly;
        monthly = nullptr;
    }

    if (version == static_cast<int>(versionUnknown))
        version = static_cast<int>(study.header.version);
    // Compatibility <= 3.2
    if (version <= static_cast<int>(version320))
        return internalLoadFromINIv32(study, ini, warnings);

    // Post 3.2
    if (!internalLoadFromINIPost32(study, ini, warnings))
    {
        // The loading has failed - fallback
        pMode = modeAnnual;
        if (JIT::usedFromGUI)
        {
            // Reset
            annual = new Matrix<>();
            annual->resize(study.areas.size(), study.areas.size());
            annual->fillUnit();

            // Preparing the monthly correlation matrices
            monthly = new Matrix<>[12];
            for (int i = 0; i < 12; ++i)
            {
                monthly[i].resize(study.areas.size(), study.areas.size());
                monthly[i].fillUnit();
            }
        }
        else
        {
            annual = new Matrix<>();
            annual->resize(study.areas.size(), study.areas.size());
            annual->fillUnit();
        }

        return false;
    }
    return true;
}

bool Correlation::internalLoadFromINIv32(Study& study, const IniFile& ini, bool warnings)
{
    // Force the mode of the study
    pMode = modeAnnual;

    annual = new Matrix<>();
    annual->resize(study.areas.size(), study.areas.size());
    annual->fillUnit();

    if (JIT::usedFromGUI)
    {
        // Preparing the monthly correlation matrices
        monthly = new Matrix<>[12];
        for (int i = 0; i < 12; ++i)
        {
            monthly[i].resize(study.areas.size(), study.areas.size());
            monthly[i].fillUnit();
        }
    }

    AreaName name;

    // Browsing each section
    for (auto* s = ini.firstSection; s; s = s->next)
    {
        name = s->name;
        name.toLower();
        auto* from = study.areas.find(name);
        if (!from)
        {
            if (warnings)
                logs.warning() << correlationName << "*: " << s->name << ": Area not found";
            continue;
        }

        for (const IniFile::Property* p = s->firstProperty; p; p = p->next)
        {
            name = p->key;
            name.toLower();
            const Area* to = study.areas.find(name);
            if (to)
            {
                set(*annual, *from, *to, p->value);
            }
            else
            {
                if (warnings)
                    logs.warning() << correlationName << "*: " << s->name << ": " << p->key
                                   << ": Area not found";
            }
        }
    }
    return true;
}

void Correlation::set(Matrix<>& m, const Area& from, const Area& to, double v)
{
    if (from.index == to.index)
    {
        logs.error() << correlationName
                     << ": Impossible to overwrite the correlation coefficient for `" << from.name
                     << "` / `" << to.name << '`';
        return;
    }

    if (v < -1. or v > +1.)
    {
        double copy = v;
        if (v < -1.)
            v = -1.;
        if (v > +1.)
            v = +1.;
        logs.warning() << correlationName << ": `" << from.name << "` / `" << to.name
                       << "`: Expected a value in [-1..+1], got " << copy << ", rounded to " << v;
    }

    m[from.index][to.index] = v;
    m[to.index][from.index] = v;
}

void Correlation::retrieveMontlyMatrixArray(const Matrix<>* array[12]) const
{
    switch (pMode)
    {
    case modeAnnual:
    {
        for (uint i = 0; i != 12; ++i)
            array[i] = annual;
        break;
    }
    case modeMonthly:
    {
        for (uint i = 0; i != 12; ++i)
            array[i] = &(monthly[i]);
        break;
    }
    default:
    {
        for (uint i = 0; i != 12; ++i)
            array[i] = nullptr;
        return;
    }
    }
}

Yuni::uint64 Correlation::memoryUsage() const
{
    Yuni::uint64 r = sizeof(Correlation);
    if (annual)
        r += annual->memoryUsage();
    if (monthly)
    {
        for (uint i = 0; i != 12; ++i)
            r += monthly[i].memoryUsage();
    }
    return r;
}

void Correlation::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    if (pMode == modeAnnual)
    {
        Matrix<>::EstimateMemoryUsage(u, u.study.areas.size(), u.study.areas.size());
    }
    else
    {
        for (uint i = 0; i != 12; ++i)
            Matrix<>::EstimateMemoryUsage(u, u.study.areas.size(), u.study.areas.size());
    }
}

bool Correlation::invalidate(bool reload) const
{
    bool ret = true;
    if (annual)
        ret = annual->invalidate(reload) and ret;
    for (uint i = 0; i != 12; ++i)
        ret = monthly[i].invalidate(reload) and ret;

    return ret;
}

void Correlation::markAsModified() const
{
    if (annual)
        annual->markAsModified();
    for (uint i = 0; i != 12; ++i)
        monthly[i].markAsModified();
}

static inline uint FindMappedAreaName(const AreaName& name,
                                      const Study& study,
                                      const Area::NameMapping& mapping)
{
    auto i = mapping.find(name);
    if (i != mapping.end())
    {
        auto* area = study.areas.findFromName(i->second);
        return (!area) ? ((uint)-1) : area->index;
    }
    auto* area = study.areas.findFromName(name);
    return (!area) ? ((uint)-1) : area->index;
}

static void CopyFromSingleMatrix(const Matrix<>& mxsrc,
                                 Matrix<>& mxout,
                                 const Study& studySource,
                                 uint areaSource,
                                 uint areaTarget,
                                 const Area::NameMapping& mapping,
                                 const Study& study)
{
    // for (uint x = 0; x <= areaSource; ++x)
    // We MUST take the original area list, and not the current one
    auto end = studySource.areas.nameidSet.end();
    for (auto i = studySource.areas.nameidSet.begin(); i != end; ++i)
    {
        uint x;
        {
            auto* a = studySource.areas.find(*i);
            if (!a)
                continue;
            x = a->index;
        }

        auto& sourceAreaName = studySource.areas[x]->name;
        uint areaIndex = FindMappedAreaName(sourceAreaName, study, mapping);

        if (areaIndex != (uint)-1) // the area has been found
        {
            double c = mxsrc[areaSource][x];
            mxout[areaTarget][areaIndex] = c;
            mxout[areaIndex][areaTarget] = c;
        }
    }
}

void Correlation::copyFrom(const Correlation& source,
                           const Study& studySource,
                           const AreaName& areaSource,
                           const Area::NameMapping& mapping,
                           const Study& study)
{
    if (study.areas.size() <= 1)
    {
        // No correlation
        return;
    }
    enum
    {
        invalid = (uint)-1,
    };

    uint areaTargetIndex = (uint)-1;
    uint areaSourceIndex = (uint)-1;

    {
        const Area* a = studySource.areas.findFromName(areaSource);
        if (!a)
        {
            logs.error() << "[correlation][copy] Impossible to find '" << areaSource << "'";
            return;
        }
        areaSourceIndex = a->index;
    }
    areaTargetIndex = FindMappedAreaName(areaSource, study, mapping);
    if (invalid == areaSourceIndex)
    {
        logs.error() << "[correlation][copy] Impossible to find '" << areaSource << "'";
        return;
    }

    // copying the annual correlation matrix
    std::cout << "ANNUAL\n";
    CopyFromSingleMatrix(
      *source.annual, *annual, studySource, areaSourceIndex, areaTargetIndex, mapping, study);

    std::cout << "MONTHLY\n";
    // copying monthly correlation matrix
    for (uint i = 0; i != 12; ++i)
    {
        CopyFromSingleMatrix(source.monthly[i],
                             monthly[i],
                             studySource,
                             areaSourceIndex,
                             areaTargetIndex,
                             mapping,
                             study);
    }
}

} // namespace Data
} // namespace Antares
