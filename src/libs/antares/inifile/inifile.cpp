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

#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "antares/inifile/inifile.h"
#include <antares/logs/logs.h>
#include <antares/io/statistics.h>

using namespace Yuni;

namespace Antares
{
static inline IniFile::Section* AnalyzeIniLine(const std::string filename,
                                               IniFile* d,
                                               IniFile::Section* section,
                                               std::string line,
                                               uint64_t& read)
{
    enum Type
    {
        typeUnknown = 0,
        typeSection = 1,
        typeProperty = 2,
    };

    uint bytesRead = 0;
    Type type = typeUnknown;
    char* p = &line[0];
    const char* key = &line[0];
    const char* value = nullptr;

    while ('\0' != *p)
    {
        ++bytesRead;
        if (typeUnknown == type)
        {
            if ('[' == *p)
            {
                type = typeSection;
                ++p;
                value = p;
                continue;
            }
            else if ('=' == *p)
            {
                type = typeProperty;
                *p = '\0';
                ++p;
                value = p;
                continue;
            }
        }
        else
        {
            if (typeSection == type)
            {
                if (']' == *p)
                {
                    *p = '\0';
                    break;
                }
            }
            else
            {
                if (('\n' == *p)
                    or ('\r' == *p)) // or (*p == '/' and ('/' == *(p + 1) or '*' == *(p + 1))))
                {
                    *p = '\0';
                    break;
                }
            }
        }
        ++p;
    }

    read += bytesRead;

    if (typeProperty == type)
    {
        if (section and *value != '\0' and value)
            section->add(key, value);
        return section;
    }
    if (typeSection == type and value and '\0' != *value)
        return d->addSection(value);

    std::string k{ key };
    boost::trim(k);
    if (not k.empty() and k[0] != ';' and k[0] != '#')
    {
        logs.error() << filename << ": invalid INI format. Got a key without any value '" << k
                     << "'";
    }
    return section;
}

IniFile::Property::Property(const AnyString& key) : key(key), next(nullptr)
{
    this->key.trim();
    this->key.toLower();
}

IniFile::Property::Property() : next(nullptr)
{
}

IniFile::Property::~Property()
{
}

template<class StreamT>
inline void IniFile::Property::saveToStream(StreamT& file, uint64_t& written) const
{
    written += key.size() + value.size() + 4;
    file << key << " = " << value << '\n';
}

template<class StreamT>
void IniFile::Section::saveToStream(StreamT& file, uint64_t& written) const
{
    if (!firstProperty)
        return;
    file << '[' << name << "]\n";
    written += 4 /* []\n\n */ + name.size();

    for (auto* property = firstProperty; property; property = property->next)
        property->saveToStream(file, written);

    file << '\n';
}

IniFile::Section::~Section()
{
    if (firstProperty)
    {
        IniFile::Property* p = firstProperty;
        IniFile::Property* n;
        do
        {
            n = p->next;
            delete p;
            p = n;
        } while (p);
    }
}

IniFile::IniFile() : firstSection(nullptr), lastSection(nullptr)
{
}

IniFile::IniFile(const AnyString& filename) : firstSection(nullptr), lastSection(nullptr)
{
    open(filename);
}

IniFile::~IniFile()
{
    if (firstSection)
    {
        IniFile::Section* s = firstSection;
        IniFile::Section* n;
        do
        {
            n = s->next;
            delete s;
            s = n;
        } while (s);
    }
}

IniFile::Section* IniFile::add(IniFile::Section* s)
{
    assert(s and "The section can not be null");

    if (!lastSection)
    {
        firstSection = s;
        lastSection = s;
    }
    else
    {
        lastSection->next = s;
        lastSection = s;
    }
    return s;
}

void IniFile::clear()
{
    if (firstSection)
    {
        IniFile::Section* s = firstSection;
        IniFile::Section* n;
        while (s)
        {
            n = s->next;
            delete s;
            s = n;
        }
        firstSection = nullptr;
        lastSection = nullptr;
    }
}

uint IniFile::Section::size() const
{
    if (!firstProperty)
        return 0;
    uint count = 0;
    auto* p = firstProperty;
    do
    {
        ++count;
        p = p->next;
    } while (p);
    return count;
}

void IniFile::readStream(std::istream& in_stream, std::string filePath, bool warnings)
{
    std::string line;
    IniFile::Section* lastSection = nullptr;
    uint64_t read = 0;

    while (std::getline(in_stream, line))
        lastSection = AnalyzeIniLine(filePath, this, lastSection, line, read);

    if (read)
        Statistics::HasReadFromDisk(read);
}

bool IniFile::open(const AnyString& filename, bool warnings)
{
    clear();
    pFilename = filename; // storing filename for further use
    std::string filePath = filename.to<std::string>();

    if (std::ifstream file(filePath); file.is_open())
    {
        readStream(file, filePath, warnings);
        return true;
    }

    if (warnings)
        logs.error() << "I/O error: " << filename << ": Impossible to read the file";
    pFilename.clear();
    return false;
}

void IniFile::saveToString(std::string& str) const
{
    uint64_t written = 0;
    std::ostringstream ostream;
    // save all sections
    for (auto* section = firstSection; section; section = section->next)
        section->saveToStream(ostream, written);

    if (written != 0)
        Statistics::HasWrittenToDisk(written);

    str = ostream.str();
}

bool IniFile::save(const AnyString& filename) const
{
    logs.debug() << "  :: writing `" << filename << '`';
    
    std::ofstream f(filename.to<std::string>());
    if (f.good())
    {
        uint64_t written = 0;
        for (auto* section = firstSection; section; section = section->next)
            section->saveToStream(f, written);
        if (written != 0)
            Statistics::HasWrittenToDisk(written);
        return true;
    }
    else
    {
        logs.error() << "I/O error: " << filename << ": Impossible to write the file";
        return false;
    }

    

}

IniFile::Property* IniFile::Section::find(const AnyString& key)
{
    for (auto* property = firstProperty; property; property = property->next)
    {
        if (property->key == key)
            return property;
    }
    return nullptr;
}

const IniFile::Property* IniFile::Section::find(const AnyString& key) const
{
    for (auto* property = firstProperty; property; property = property->next)
    {
        if (property->key == key)
            return property;
    }
    return nullptr;
}

IniFile::Section* IniFile::find(const AnyString& name)
{
    for (auto* section = firstSection; section; section = section->next)
    {
        if (section->name == name)
            return section;
    }
    return nullptr;
}

const IniFile::Section* IniFile::find(const AnyString& name) const
{
    for (auto* section = firstSection; section; section = section->next)
    {
        if (section->name == name)
            return section;
    }
    return nullptr;
}

} // namespace Antares
