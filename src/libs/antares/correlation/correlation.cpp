// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/correlation/correlation.h"

#include <antares/array/matrix.h>
#include <antares/utils/utils.h>
#include "antares/study/area/area.h"
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

namespace Antares::Data
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
        {
            logs.error() << ini.filename() << ": the section 'general' is missing";
        }
        return false;
    }
    const IniFile::Property* p = section->find("mode");
    if (!p)
    {
        if (warnings)
        {
            logs.error() << ini.filename() << ": the property 'mode' is missing";
        }
        return false;
    }
    mode = Correlation::CStringToMode(p->value.c_str());
    if (mode == Correlation::modeNone)
    {
        if (warnings)
        {
            logs.error() << ini.filename() << ": invalid correlation mode";
        }
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
        {
            continue;
        }

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
            {
                logs.error() << ini.filename() << ": '" << nameFrom
                             << "': impossible to find the area";
            }
            continue;
        }
        if (!to)
        {
            if (warnings)
            {
                logs.error() << ini.filename() << ": '" << nameTo
                             << "': impossible to find the area";
            }
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
                                                 const std::string& name)
{
    if (m.empty() or m.width != m.height)
    {
        return;
    }

    file << '[' << name << "]\n";

    // For each column
    for (uint x = 0; x != m.width; ++x)
    {
        const AreaName& from = study.areas.byIndex[x]->id;

        auto& col = m.entry[x];
        for (uint y = 0; y < x; ++y)
        {
            if (!Utils::isZero(col[y]))
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
        for (s = ini->firstSection; s; s = s->next) /* Each section */
        {
            Area* from = AreaListLFind(l, s->name.c_str());
            if (from)
            {
                IniFile::Property* p;
                for (p = s->firstProperty; p; p = p->next) /* Each property*/
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
                        {
                            logs.warning()
                              << "Correlation: " << s->name << ": " << p->key << ": Area not found";
                        }
                    }
                }
            }
            else /* nullptr != from */
            {
                if (warnings)
                {
                    logs.warning() << "Correlation: " << s->name << ": Area not found";
                }
            }
        }
        return 1;
    }
    return 0;
}

int InterAreaCorrelationLoadFromFile(Matrix<>* m, AreaList* l, const std::string& filename)
{
    /* Asserts */
    assert(m);
    assert(l);
    assert(!filename.empty());

    InterAreaCorrelationResetMatrix(m, l);
    IniFile ini;
    if (ini.open(filename))
    {
        return InterAreaCorrelationLoadFromIniFile(m, l, &ini, 1);
    }
    logs.error() << filename << ": Impossible to load the file";
    return 0;
}

Correlation::Correlation():
    pMode(modeNone)
{
}

bool Correlation::loadFromFile(Study& study, const AnyString& filename, bool warnings)
{
    Antares::logs.debug() << "  " << correlationName << ": loading " << filename;
    IniFile ini;
    return (ini.open(filename)) ? internalLoadFromINI(study, ini, warnings) : false;
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
    {
        return modeAnnual;
    }
    if (s == "monthly")
    {
        return modeMonthly;
    }
    return modeNone;
}

bool Correlation::internalLoadFromINITry(Study& study, const IniFile& ini, bool warnings)
{
    if (!ReadCorrelationModeFromINI(ini, pMode, warnings))
    {
        return false;
    }

    if (pMode == modeAnnual)
    {
        annual.clear();
        annual.resize(study.areas.size(), study.areas.size());
        annual.fillUnit();

        auto* section = ini.find("annual");
        if (section) // the section might be missing
        {
            ReadCorrelationCoefficients(*this, study, annual, ini, *section, warnings);
        }
    }

    if (pMode == modeMonthly)
    {
        monthly.resize(12);
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
                    ReadCorrelationCoefficients(*this,
                                                study,
                                                monthly[index],
                                                ini,
                                                *section,
                                                warnings);
                }
                else
                {
                    logs.error() << "Invalid month index: " << index;
                }
            }
        }
    }

    return true;
}

void Correlation::reset(Study& study)
{
    clear();

    pMode = modeAnnual;
    annual.clear();
    annual.resize(study.areas.size(), study.areas.size());
    annual.fillUnit();
}

void Correlation::clear()
{
    annual.reset();
    monthly.clear();
}

bool Correlation::internalLoadFromINI(Study& study, const IniFile& ini, bool warnings)
{
    clear();

    if (!internalLoadFromINITry(study, ini, warnings))
    {
        // The loading has failed - fallback
        pMode = modeAnnual;
        annual.clear();
        annual.resize(study.areas.size(), study.areas.size());
        annual.fillUnit();

        return false;
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
        {
            v = -1.;
        }
        if (v > +1.)
        {
            v = +1.;
        }
        logs.warning() << correlationName << ": `" << from.name << "` / `" << to.name
                       << "`: Expected a value in [-1..+1], got " << copy << ", rounded to " << v;
    }

    m[from.index][to.index] = v;
    m[to.index][from.index] = v;
}

static inline uint FindMappedAreaName(const AreaName& name,
                                      const Study& study,
                                      const AreaNameMapping& mapping)
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
                                 const AreaNameMapping& mapping,
                                 const Study& study)
{
    // for (uint x = 0; x <= areaSource; ++x)
    // We MUST take the original area list, and not the current one
    for (const auto& i: studySource.areas.nameidSet)
    {
        uint x;
        {
            auto* a = studySource.areas.find(i);
            if (!a)
            {
                continue;
            }
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
                           const AreaNameMapping& mapping,
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
    CopyFromSingleMatrix(source.annual,
                         annual,
                         studySource,
                         areaSourceIndex,
                         areaTargetIndex,
                         mapping,
                         study);

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

} // namespace Antares::Data
