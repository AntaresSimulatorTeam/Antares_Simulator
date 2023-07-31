/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_LIBS_JIT_JIT_H__
#define __ANTARES_LIBS_JIT_JIT_H__

#include <yuni/core/fwd.h>
#include <yuni/core/string/string.h>

namespace Antares
{
template<class T, class ReadWriteT>
class Matrix;
}

class JIT final
{
public:
    // Forward declaration
    class Informations;

    /*!
    ** \brief Reset the source filename
    */
    static Informations* Reset(Informations* jit, const AnyString& filename);

    /*!
    ** \brief Reset the source filename
    */
    static Informations* Reset(Informations* jit);

    /*!
    ** \brief Mark the attached object as not loaded
    */
    static void MarkAsNotLoaded(Informations* j);

    /*!
    ** \brief Mark the attached object as modified
    */
    static void Invalidate(Informations* j);

    /*!
    ** \brief Get if the data has been loaded
    */
    static bool IsReady(Informations* j);

public:
    class just_in_time_manager
    {
    public:
        just_in_time_manager(JIT::Informations* jit, const AnyString filename) :
         jit_(jit), file_name_(filename)
        {
        }
        void record_current_jit_state(unsigned width, unsigned height);
        inline JIT::Informations* jit_recorded_state()
        {
            return jit_record_;
        }
        bool matrix_content_in_memory_is_same_as_on_disk()
        {
            return not jit_->modified;
        }
        bool jit_activated()
        {
            return jit_;
        }
        bool do_we_force_matrix_load_from_disk();

        template<class T, class ReadWriteT>
        void load_matrix(const Antares::Matrix<T, ReadWriteT>* mtx);

        template<class T, class ReadWriteT>
        void clear_matrix(const Antares::Matrix<T, ReadWriteT>* mtx);

        template<class T, class ReadWriteT>
        void unload_matrix_properly_from_memory(const Antares::Matrix<T, ReadWriteT>* mtx);
        ~just_in_time_manager()
        {
            delete jit_record_;
        }

    private:
        JIT::Informations* jit_;
        JIT::Informations* jit_record_;
        AnyString file_name_;
    };

public:
    /*!
    ** \brief
    */
    class Informations final
    {
    public:
        //! \name Constructor & Destructor
        //@{
        /*!
        ** \brief Default constructor
        */
        Informations();
        //! Copy constructor
        Informations(const Informations& rhs);
        //! Destructor
        ~Informations();
        //@}

        /*!
        ** \brief Mark the associated data as modified
        */
        void markAsModified();

        //! Get the amount of memory currently used
        yuint64 memoryUsage() const;

    public:
        //! Filename/folder to consider if some data should be loaded
        YString sourceFilename;

        //! Flag to determine wheter if the associated data have already been loaded
        bool alreadyLoaded;
        //! Flag to determine wheter if the associated data have already been modified
        bool modified;
        //! Flag to determine wheter if the associated data have already been reload
        // The item will be considered as modified
        bool loadDataIfNotAlreadyDone;

        //! The option to the matrix
        unsigned options;
        /*!
        ** \brief Minimum width expected (for Matrices)
        **
        ** It is the 'width' if fixedSize != 0
        */
        unsigned minWidth;
        /*!
        ** \brief Minimum height expected (for Matrices)
        **
        ** It is the 'height' if fixedSize != 0
        */
        unsigned maxHeight;

        //! The estimated number of columns and rows
        unsigned estimatedSize[2];

        //! Timestamp of the last modification
        Yuni::sint64 lastModification;

    }; // class Informations

public:
    /*!
    ** \brief Flag to enable/disable JIT informations
    **
    ** This variable must be set before the loading of any study.
    ** Disabled by default.
    */
    static bool enabled;

    /*!
    ** \brief Flag to know if the library is called from the User interface
    */
    static bool usedFromGUI;

}; // class JIT

#include "jit.hxx"

#endif // __ANTARES_LIBS_JIT_JIT_H__
