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

#include "inifile.h"
#include "../io/statistics.h"
#include "../logs.h"

using namespace Yuni;

namespace Antares
{
static inline IniFile::Section* AnalyzeIniLine(const String& pFilename,
                                               IniFile* d,
                                               IniFile::Section* section,
                                               char* line,
                                               uint64& read)
{
    enum Type
    {
        typeUnknown = 0,
        typeSection = 1,
        typeProperty = 2,
    };

    uint bytesRead = 0;
    Type type = typeUnknown;
    char* p = line;
    const char* key = line;
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

    CString<255, false> k = key;
    k.trim(" \r\n\t");
    if (not k.empty() and k[0] != ';' and k[0] != '#')
    {
        logs.error() << pFilename << ": invalid INI format. Got a key without any value '" << k
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
inline void IniFile::Property::saveToStream(StreamT& file, uint64& written) const
{
    written += key.size() + value.size() + 4;
    file << key << " = " << value << '\n';
}

template<class StreamT>
void IniFile::Section::saveToStream(StreamT& file, uint64& written) const
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

bool IniFile::open(const AnyString& filename, bool warnings)
{
    // clear
    clear();
    pFilename = filename;

    // Load the file
    IO::File::Stream file;
    if (file.open(filename))
    {
        enum
        {
            lineHardLimit = 2048
        };
        auto* line = new char[lineHardLimit];

        IniFile::Section* lastSection = nullptr;
        uint64 read = 0;

        // analyzing each line
        while (file.readline(line, lineHardLimit))
            lastSection = AnalyzeIniLine(pFilename, this, lastSection, line, read);

        delete[] line;

        if (read)
            Statistics::HasReadFromDisk(read);
        return true;
    }

    // Error
    if (warnings)
        logs.error() << "I/O error: " << filename << ": Impossible to read the file";
    pFilename.clear();
    return false;
}

bool IniFile::save(const AnyString& filename) const
{
    logs.debug() << "  :: writing `" << filename << '`';
    IO::File::Stream f;
    if (f.openRW(filename))
    {
        pFilename = filename;
        uint64 written = 0;

        // save all sections
        for (auto* section = firstSection; section; section = section->next)
            section->saveToStream(f, written);

        if (written != 0)
            Statistics::HasWrittenToDisk(written);
        return true;
    }
    else
    {
        pFilename.clear();
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
