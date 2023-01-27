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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jit.h"

bool JIT::enabled = false;

bool JIT::usedFromGUI = false;

JIT::Informations::Informations() :
 alreadyLoaded(false),
 modified(false),
 loadDataIfNotAlreadyDone(false),
 options(0),
 minWidth(0),
 maxHeight(0),
 lastModification(0)
{
    estimatedSize[0] = 0;
    estimatedSize[1] = 0;
}

JIT::Informations::Informations(const Informations& rhs) :
 alreadyLoaded(true),
 modified(true),
 loadDataIfNotAlreadyDone(true),
 options(rhs.options),
 minWidth(rhs.minWidth),
 maxHeight(rhs.maxHeight),
 lastModification(0)
{
    estimatedSize[0] = 0;
    estimatedSize[1] = 0;
}

JIT::Informations::~Informations()
{
}

JIT::Informations* JIT::Reset(JIT::Informations* jit)
{
    assert(JIT::enabled and "JIT must be enabled");

    // if changes, see JIT::Reset(jit, filename)
    // Allocate the structure if needed
    if (not jit)
        return new JIT::Informations();

    // Reset the `sourceFilename` if already set
    jit->sourceFilename.clear();
    // Resetting the estimated values in cache
    jit->estimatedSize[0] = jit->estimatedSize[1] = 0;
    return jit;
}

JIT::Informations* JIT::Reset(JIT::Informations* jit, const AnyString& filename)
{
    assert(JIT::enabled and "JIT must be enabled");

    if (not jit)
    {
        jit = new JIT::Informations();
    }
    else
    {
        // Resetting the estimated values in cache
        jit->estimatedSize[0] = 0;
        jit->estimatedSize[1] = 0;
    }

    // Reset the `sourceFilename` if already set
    jit->sourceFilename = filename;
    assert(not jit->sourceFilename.empty() and "jit: invalid filename registration");
    return jit;
}

void JIT::MarkAsNotLoaded(JIT::Informations* jit)
{
    if (jit)
    {
        jit->modified = false;
        jit->alreadyLoaded = false;
    }
}

void JIT::Invalidate(JIT::Informations* jit)
{
    if (jit)
    {
        jit->loadDataIfNotAlreadyDone = true;
        // jit->modified = true;
        // Resetting the estimated values in cache
        jit->estimatedSize[0] = 0;
        jit->estimatedSize[1] = 0;
    }
}

void JIT::just_in_time_manager::record_current_jit_state(uint width, uint height)
{
    jit_record_ = new JIT::Informations();

    // These values are _mandatory_ especially for empty matrices
    jit_record_->options = 0; // = optNone
    jit_record_->minWidth = (width != 0) ? width : 1;
    jit_record_->maxHeight = height;

    if (jit_)
    {
        jit_record_->options = jit_->options;
        jit_record_->minWidth = jit_->minWidth;
        jit_record_->maxHeight = jit_->maxHeight;
    }
}

bool JIT::just_in_time_manager::do_we_force_matrix_load_from_disk()
{
    // Force the load of the current matrix from disk, for example when we are making a "save as" of
    // a study into a new one. The study "save" operation (after a change in GUI) does not need such
    // an enforcement, as the matrix is lying in memory and not on disk.
    return jit_->loadDataIfNotAlreadyDone;
}