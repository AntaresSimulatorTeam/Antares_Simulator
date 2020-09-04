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

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__
# define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__

# include <yuni/core/string.h>

namespace Antares
{
	template<class T, class ReadWriteT>
	class Matrix;
}

namespace Antares
{
	// Forward declarations
	const char* get_format(bool isDecimal, uint precision);
	template<class T, class ReadWriteT, class PredicateT>
	class I_mtx_to_buffer_dumper;
	


	class matrix_to_buffer_dumper_factory
	{
	public:
		matrix_to_buffer_dumper_factory(bool isDecimal, uint precision) :
			any_decimal_(isDecimal and precision)
		{}

		~matrix_to_buffer_dumper_factory() {}

		template<class T, class ReadWriteT, class PredicateT>
		I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>* get_dumper(const Matrix<T, ReadWriteT>* mtx, Yuni::Clob& data, PredicateT& predicate);

	private:
		bool any_decimal_;
	};



	template<class T, class ReadWriteT, class PredicateT>
	class I_mtx_to_buffer_dumper
	{
	public:
		I_mtx_to_buffer_dumper(const Matrix<T, ReadWriteT>* mtx, Yuni::Clob& data, PredicateT& predicate) :
			mtx_(mtx),
			buffer_(data),
			predicate_(predicate),
			format_(nullptr)
		{}

		void set_print_format(bool isDecimal, uint precision);
		virtual void run() = 0;
		~I_mtx_to_buffer_dumper() { delete format_; }

	protected:
		const Matrix<T, ReadWriteT>* mtx_;
		Yuni::Clob& buffer_;
		PredicateT& predicate_;
		const char* format_;
	};

	template<class T, class ReadWriteT, class PredicateT>
	class one_column__dumper : public I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>
	{
	public:
		one_column__dumper(const Matrix<T, ReadWriteT>* mtx, Yuni::Clob& data, PredicateT& predicate) :
			I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate)
		{}
		void run() override;
	};

	template<class T, class ReadWriteT, class PredicateT>
	class multiple_columns__dumper : public I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>
	{
	public:
		multiple_columns__dumper(const Matrix<T, ReadWriteT>* mtx, Yuni::Clob& data, PredicateT& predicate) :
			I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate)
		{}
		void run() override;
	};

}	// namespace Antares

#include "matrix-to-buffer.hxx"

#endif	// __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_H__