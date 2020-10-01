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
#ifndef __ANTARES_LIBS_JUST_IN_TIME_INFORMATIONS_HXX__
# define __ANTARES_LIBS_JUST_IN_TIME_INFORMATIONS_HXX__

#include "../logs.h"

using namespace Antares;

inline bool JIT::IsReady(JIT::Informations* j)
{
	return (!j or j->alreadyLoaded);
}


inline Yuni::uint64 JIT::Informations::memoryUsage() const
{
	return sizeof(JIT::Informations) + sourceFilename.capacity();
}


inline void JIT::Informations::markAsModified()
{
	modified = true;
	alreadyLoaded = true;
}

using namespace Antares;

template<class T, class ReadWriteT>
void JIT::just_in_time_manager::clear_matrix(const Matrix<T, ReadWriteT>* mtx)
{
	JIT::MarkAsNotLoaded(jit_);

	// Ugly const_cast but it is to preserve a good public Matrix class API :
	auto * mtx_not_const = const_cast<Matrix<T, ReadWriteT>*>(mtx);
	mtx_not_const->clear();
}

template<class T, class ReadWriteT>
void JIT::just_in_time_manager::unload_matrix_properly_from_memory(const Matrix<T, ReadWriteT>* mtx)
{
	using namespace Antares;

	auto* mtx_not_const = const_cast<Matrix<T, ReadWriteT>*>(mtx);

	// - jit activated :
	//		If JIT (Just-In-Time) is activated, we have to unload data to keep the memory for the solver.
	//		The old width must only be kept when the matrix has fixed dimension.
	//
	// - JIT::enabled and jit not activated :
	//		If the matrix is not ready for just-in-time (not activated), but JIT is enabled, like it would be the case 
	//		from the GUI, we have to activate jit.
	//		2 reasons for this :
	//		- To reserve the memory for the study
	//		- To reload data in case of preprocessor data have been written to
	//		  the input folder

	if (jit_ or JIT::enabled)
	{
		Yuni::String buffer = file_name_;
		jit_ = JIT::Reset(jit_, buffer);
		mtx->jit = jit_;
		JIT::MarkAsNotLoaded(jit_);
		jit_->minWidth = (0 != (jit_recorded_state()->options & Matrix<T, ReadWriteT>::optFixedSize)) ? 
								jit_recorded_state()->minWidth : 1;
		jit_->maxHeight = jit_recorded_state()->maxHeight;
		jit_->options = jit_recorded_state()->options;
		mtx_not_const->clear();
	}
}


template<class T, class ReadWriteT>
void JIT::just_in_time_manager::load_matrix(const Matrix<T, ReadWriteT>* mtx)
{
	if (not jit_->alreadyLoaded)
	{
		auto* mtx_not_const = const_cast<Matrix<T, ReadWriteT>*>(mtx);

		logs.debug() << " Force loading of " << jit_->sourceFilename;
		const bool modi = jit_->modified;

		mtx_not_const->loadFromCSVFile(
				jit_->sourceFilename, 
				jit_->minWidth,
				jit_->maxHeight,
				jit_->options | Matrix<T, ReadWriteT>::optImmediate);

		jit_->modified = modi;
	}
	jit_->loadDataIfNotAlreadyDone = false;
}

#endif // __ANTARES_LIBS_JUST_IN_TIME_INFORMATIONS_HXX__
