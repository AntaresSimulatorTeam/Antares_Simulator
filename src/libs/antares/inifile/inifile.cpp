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

#include "antares/inifile/inifile.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

#include <boost/algorithm/string.hpp>

#include <antares/io/statistics.h>
#include <antares/logs/logs.h>

using namespace Yuni;

namespace fs = std::filesystem;

namespace Antares
{

IniFile::Property::Property(const AnyString& key):
    key(key)
{
    this->key.trim();
    this->key.toLower();
}

inline void IniFile::Property::saveToStream(std::ostream& stream_out, uint64_t& written) const
{
    written += key.size() + value.size() + 4;
    stream_out << key << " = " << value << '\n';
}

void IniFile::Section::add(const Property& property)
{
    add(property.key, property.value);
}

void IniFile::Section::saveToStream(std::ostream& stream_out, uint64_t& written) const
{
    if (!firstProperty)
    {
        return;
    }
    stream_out << '[' << name << "]\n";
    written += 4 /* []\n\n */ + name.size();

    each([&stream_out, &written](const IniFile::Property& p)
         { p.saveToStream(stream_out, written); });

    stream_out << '\n';
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

IniFile::IniFile(const fs::path& filename)
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
    {
        return 0;
    }
    uint count = 0;
    auto* p = firstProperty;
    do
    {
        ++count;
        p = p->next;
    } while (p);
    return count;
}

static bool isStartingSection(std::string line)
{
    boost::trim(line);
    return line.starts_with("[") && line.ends_with("]");
}

static std::string getSectionName(const std::string& line)
{
    std::vector<std::string> splitLine;
    boost::split(splitLine, line, boost::is_any_of("[]"));
    return splitLine[1];
}

static bool isProperty(std::string_view line)
{
    return std::ranges::count(line, '=') == 1;
}

static IniFile::Property getProperty(std::string line)
{
    boost::trim(line);
    std::vector<std::string> splitLine;
    boost::split(splitLine, line, boost::is_any_of("="));

    return IniFile::Property(splitLine[0], splitLine[1]);
}

static bool isComment(std::string line)
{
    boost::trim_left(line);
    return line.starts_with("#") || line.starts_with(";");
}

bool isEmpty(std::string line)
{
    boost::trim(line);
    return line.empty();
}

bool IniFile::readStream(std::istream& in_stream)
{
    std::string line;
    IniFile::Section* currentSection(nullptr);
    uint64_t read = 0;
    while (std::getline(in_stream, line))
    {
        read += line.size();
        if (isStartingSection(line))
        {
            currentSection = addSection(getSectionName(line));
            continue;
        }

        if (isProperty(line))
        {
            // Note : if a property not in a section, it's simply skipped
            if (currentSection)
            {
                currentSection->add(getProperty(line));
            }
            continue;
        }

        if (isComment(line) || isEmpty(line))
        {
            continue;
        }

        logs.error() << "INI content : unknown format for line '" << line << "'";
        return false;
    }
    if (read)
    {
        Statistics::HasReadFromDisk(read);
    }

    return true;
}

bool IniFile::open(const std::string& filename, bool warnings)
{
    fs::path filepath = filename;
    return open(filepath, warnings);
}

bool IniFile::open(const fs::path& filename, bool warnings)
{
    clear();
    filename_ = filename.string(); // storing filename for further use

    if (std::ifstream file(filename); file.is_open())
    {
        if (!readStream(file))
        {
            logs.error() << "Invalid INI file : " << filename;
            return false;
        }
        return true;
    }

    if (warnings)
    {
        logs.error() << "I/O error: " << filename << ": Impossible to read the file";
    }
    return false;
}

void IniFile::saveToStream(std::ostream& stream_out, uint64_t& written) const
{
    each([&stream_out, &written](const IniFile::Section& s)
         { s.saveToStream(stream_out, written); });

    if (written != 0)
    {
        Statistics::HasWrittenToDisk(written);
    }
}

std::string IniFile::toString() const
{
    uint64_t written = 0;
    std::ostringstream ostream;

    this->saveToStream(ostream, written);

    return ostream.str();
}

bool IniFile::save(const AnyString& filename) const
{
    logs.debug() << "  :: writing `" << filename << '`';

    std::ofstream of(filename.to<std::string>());
    if (of.good())
    {
        uint64_t written = 0;
        this->saveToStream(of, written);
        return true;
    }

    logs.error() << "I/O error: " << filename << ": Impossible to write the file";
    return false;
}

IniFile::Property* IniFile::Section::find(const AnyString& key)
{
    for (auto* property = firstProperty; property; property = property->next)
    {
        if (property->key == key)
        {
            return property;
        }
    }
    return nullptr;
}

const IniFile::Property* IniFile::Section::find(const AnyString& key) const
{
    for (auto* property = firstProperty; property; property = property->next)
    {
        if (property->key == key)
        {
            return property;
        }
    }
    return nullptr;
}

IniFile::Section* IniFile::find(const AnyString& name)
{
    for (auto* section = firstSection; section; section = section->next)
    {
        if (section->name == name)
        {
            return section;
        }
    }
    return nullptr;
}

const IniFile::Section* IniFile::find(const AnyString& name) const
{
    for (auto* section = firstSection; section; section = section->next)
    {
        if (section->name == name)
        {
            return section;
        }
    }
    return nullptr;
}

} // namespace Antares
