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

#include "study.h"
#include "../benchmarking.h"
#include "../array/correlation.h"
#include "scenario-builder/sets.h"
#include <yuni/core/string/wstring.h>
#include <fstream>

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
bool Study::resetFolderIcon() const
{
    // Create the Desktop.ini file to show a different icon for Antares study folder
    {
        buffer.clear() << folder << SEP << "Desktop.ini";
        IO::File::Stream file;
        if (not file.openRW(buffer))
            return false;
        buffer.clear() << "[.shellclassinfo]\r\n"
                       << "iconfile = settings/resources/study.ico\r\n"
                       << "iconindex = 0\r\n"
                       << "infotip = Antares Study" << VersionToCStr((Version)header.version)
                       << ": " << header.caption << "\r\n";
        file << buffer;
    }

#ifdef YUNI_OS_WINDOWS
    {
        // The file should be closed at this point
        Yuni::WString wbuffer(folder);
        if (not wbuffer.empty())
            SetFileAttributesW(wbuffer.c_str(), FILE_ATTRIBUTE_SYSTEM);
    }
#endif

    return true;
}

bool Study::saveToFolder(const AnyString& newfolder)
{
    if (newfolder.empty())
        return false;

    logs.notice() << "Exporting the study...";
    const String location = newfolder;
    // trying to access to the new location first, before any useless
    // loading attemp
    // This should be enough for the next creation (unless there is no
    // disk space left)
    if (not IO::Directory::Create(location))
    {
        logs.error() << "I/O error: impossible to create the folder '" << location << "'";
        return false;
    }

    Statistics::LogsDumper statisticsDumper;

    logs.info() << "Preloading data...";
    bool ret = true;
    {
        uint invalidateCount = 0;
        ret = areas.preloadAndMarkAsModifiedAllInvalidatedAreas(&invalidateCount);
        if (invalidateCount > 0)
        {
            // if at least one area has been invalidated, all constraints
            // must be invalidated as well
            bindingConstraints.invalidate(true);
            bindingConstraints.markAsModified();
        }
    }

    // Trying to create a few important folders
    logs.info() << "Preparing folders...";
    {
        const String backupFolder = this->folder;
        // Changing the paths
        relocate(location);

        // Output
        if (not IO::Directory::Create(folderInput))
        {
            logs.error() << "I/O error: impossible to create the folder '" << folderInput << "'";
            relocate(backupFolder);
            return false;
        }
        if (not IO::Directory::Create(folderSettings))
        {
            logs.error() << "I/O error: impossible to create the folder '" << folderSettings << "'";
            relocate(backupFolder);
            return false;
        }

        if (not IO::Directory::Create(folderOutput))
        {
            logs.error() << "I/O error: impossible to create the folder '" << folderOutput << "'";
            relocate(backupFolder);
            return false;
        }
    }

    // Checking if the version has changed (aka upgrade)
    //
    // In this case, we have to invalidate all matrices to make sure that all
    // data will be rewritten to avoid data loss
    // (if filenames have changed for examples)
    bool versionUpgrade = ((uint)header.version != (uint)versionLatest);
    if (versionUpgrade)
    {
        logs.info() << "  performing a format upgrade";
        // Invalidate all matrices
        invalidate(true);
        markAsModified();
        // Invalidate the scenario builder data
        if (not scenarioRules)
            scenarioRulesCreate();
    }

    buffer.clear() << folder << Yuni::IO::Separator << "ConstraintBuilder" << Yuni::IO::Separator
                   << "ConstraintBuilder.ini"; // moving the constraint builder file introduced in
                                               // version 6.4 if it exists
    if (IO::File::Exists(buffer))
    {
        YString dest;
        dest << folder << Yuni::IO::Separator << "settings" << Yuni::IO::Separator
             << "constraintbuilder.ini";

        IO::File::Copy(buffer, dest);

        buffer.clear() << folder << Yuni::IO::Separator << "ConstraintBuilder";
        IO::Directory::Remove(buffer);
    }

    buffer.clear() << folder << SEP << "study.antares";
    header.saveToFile(buffer);

    // Icon File - For Windows only
    resetFolderIcon();

    // Simulations
    buffer.clear() << folder << SEP << "settings" << SEP << "resources";
    ret = IO::Directory::Create(buffer) and ret;

    if (not StudyIconFile.empty())
    {
        buffer.clear() << folder << SEP << "settings" << SEP << "resources" << SEP << "study.ico";

        std::ifstream src(StudyIconFile.c_str(), std::ios::binary);
        std::ofstream dst(buffer.c_str(), std::ios::binary);

        dst << src.rdbuf();

        dst.close();
        src.close();
    }

    // User
    buffer.clear() << folder << SEP << "user";
    ret = IO::Directory::Create(buffer) and ret;

    // Logs
    buffer.clear() << folder << SEP << "logs";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "reserves";
    ret = IO::Directory::Create(buffer) and ret;

    // Misc-gen
    buffer.clear() << folder << SEP << "input" << SEP << "misc-gen";
    ret = IO::Directory::Create(buffer) and ret;

    // Hydro
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "common" << SEP
                   << "capacity";
    ret = IO::Directory::Create(buffer) and ret;

    // Settings
    buffer.clear() << folder << SEP << "settings" << SEP << "simulations";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "settings";
    ret = simulationMetadata.saveToFolder(buffer) and ret;
    buffer.clear() << folder << SEP << "settings" << SEP << "generaldata.ini";
    ret = parameters.saveToFile(buffer) and ret;

    // All areas
    ret = areas.saveToFolder(folder) and ret;

    // Layers
    buffer.clear() << folder << SEP << "layers";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "layers" << SEP << "layers.ini";
    ret = saveLayers(buffer) and ret;

    // Binding constraints
    buffer.clear() << folder << SEP << "input" << SEP << "bindingconstraints";
    ret = bindingConstraints.saveToFolder(buffer) and ret;

    // Correlation matrices
    logs.info() << "Exporting the correlation matrices";

    logs.info() << "Correlation: Load";
    buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "prepro" << SEP
                   << "correlation.ini";
    ret = preproLoadCorrelation.saveToFile(*this, buffer) and ret;

    logs.info() << "Correlation: Solar";
    buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "prepro" << SEP
                   << "correlation.ini";
    ret = preproSolarCorrelation.saveToFile(*this, buffer) and ret;

    logs.info() << "Correlation: Wind";
    buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "prepro" << SEP
                   << "correlation.ini";
    ret = preproWindCorrelation.saveToFile(*this, buffer) and ret;

    logs.info() << "Correlation: Hydro";
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "prepro" << SEP
                   << "correlation.ini";
    ret = preproHydroCorrelation.saveToFile(*this, buffer) and ret;

    // Sets
    buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << "sets.ini";
    ret = setsOfAreas.saveToFile(buffer) and ret;

    if (scenarioRules)
    {
        buffer.clear() << folder << SEP << "settings" << SEP << "scenariobuilder.dat";
        ret = scenarioRules->saveToINIFile(buffer) and ret;
    }

    // Maps
    buffer.clear() << folder << SEP << "output" << SEP << "maps";
    ret = IO::Directory::Create(buffer) and ret;

    return ret;
}

bool Study::saveLayers(const AnyString& filename)
{
    IO::File::Stream file;
    if (file.openRW(filename))
    {
        CString<256, true> data;
        data << "[layers]\n";
        for (std::map<size_t, std::string>::iterator iterator = layers.begin();
             iterator != layers.end();
             iterator++)
        {
            data << iterator->first << " = " << iterator->second;
            data << '\n';
        }

        data << "[activeLayer]\n";
        data << "activeLayerID"
             << " = " << activeLayerID;
        data << '\n';
        data << "showAllLayer"
             << " = " << showAllLayer;

        file << data;

        return true;
    }

    logs.error() << "I/O error: impossible to write " << filename;
    return false;
}

} // namespace Data
} // namespace Antares
