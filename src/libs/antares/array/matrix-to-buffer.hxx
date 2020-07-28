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

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
# define __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__

# ifdef YUNI_OS_MSVC
#	define ANTARES_MATRIX_SNPRINTF  sprintf_s
# else
#	define ANTARES_MATRIX_SNPRINTF  snprintf
# endif


namespace Antares
{
	namespace // anonymous
	{

		template<class T>
		struct MatrixScalar
		{
			static inline void Append(Yuni::Clob& file, T v)
			{
				if (Yuni::Math::Zero(v))
					file.append('0');
				else
					file.append(v);
			}

			static inline void Append(Yuni::Clob& file, T v, const char* const)
			{
				Append(file, v);
			}
		};


		template<>
		struct MatrixScalar<double>
		{
			static void Append(Yuni::Clob& file, double v)
			{
				if (Yuni::Math::Zero(v))
				{
					file += '0';
				}
				else
				{
					char ConversionBuffer[128];
					const int sizePrintf = ANTARES_MATRIX_SNPRINTF(ConversionBuffer, sizeof(ConversionBuffer), "%.0f", v);

					if (sizePrintf >= 0 and sizePrintf < (int)(sizeof(ConversionBuffer)))
						file.write((const char*)ConversionBuffer, sizePrintf);
					else
						file += "ERR";
				}
			}

			static void Append(Yuni::Clob& file, double v, const char* const format)
			{
				if (Yuni::Math::Zero(v))
				{
					file += '0';
				}
				else
				{
					char ConversionBuffer[128];
					const int sizePrintf =
						(Yuni::Math::Zero(v - floor(v)))
						? ANTARES_MATRIX_SNPRINTF(ConversionBuffer, sizeof(ConversionBuffer), "%.0f", v)
						: ANTARES_MATRIX_SNPRINTF(ConversionBuffer, sizeof(ConversionBuffer), format, v);

					if (sizePrintf >= 0 and sizePrintf < (int)(sizeof(ConversionBuffer)))
						file.write((const char*)ConversionBuffer, sizePrintf);
					else
						file += "ERR";
				}
			}
		};


		template<>
		struct MatrixScalar<float>
		{
			static void Append(Yuni::Clob& file, float v)
			{
				if (Yuni::Math::Zero(v))
				{
					file += '0';
				}
				else
				{
					char ConversionBuffer[128];
					const int sizePrintf = ANTARES_MATRIX_SNPRINTF(ConversionBuffer, sizeof(ConversionBuffer), "%.0f", (double)v);

					if (sizePrintf >= 0 and sizePrintf < (int)(sizeof(ConversionBuffer)))
						file.write((const char*)ConversionBuffer, sizePrintf);
					else
						file += "ERR";
				}
			}

			static void Append(Yuni::Clob& file, float v, const char* const format)
			{
				if (Yuni::Math::Zero(v))
				{
					file += '0';
				}
				else
				{
					char ConversionBuffer[128];
					const int sizePrintf =
						(Yuni::Math::Zero(v - floor(v)))
						? ANTARES_MATRIX_SNPRINTF(ConversionBuffer, sizeof(ConversionBuffer), "%.0f", (double)v)
						: ANTARES_MATRIX_SNPRINTF(ConversionBuffer, sizeof(ConversionBuffer), format, (double)v);

					if (sizePrintf >= 0 and sizePrintf < (int)(sizeof(ConversionBuffer)))
						file.write((const char*)ConversionBuffer, sizePrintf);
					else
						file += "ERR";
				}
			}
		};

	} // anonymous namespace



	template<class T, class ReadWriteT, class PredicateT>
	I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>* 
		matrix_to_buffer_dumper_factory::
			get_dumper(	const Matrix<T, ReadWriteT>* mtx, 
						Yuni::Clob& data, 
						PredicateT& predicate)
	{
		if(mtx->width == 1)
		{
			if (not any_decimal_)
				return new one_column__no_decimal__dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate);
			else
				return new one_column__decimals__dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate);
		}
		else
		{
			if (not any_decimal_)
				return new multiple_columns__no_decimal__dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate);
			else
				return new multiple_columns__decimals__dumper<T, ReadWriteT, PredicateT>(mtx, data, predicate);
		}

		// return nullptr;
	}


	template<class T, class ReadWriteT, class PredicateT>
	void I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>::set_print_format(bool isDecimal, uint precision)
	{
		// Determining the string format to use according the given precision
		if (isDecimal and precision)
		{
			const char* const sfmt[] =
			{
				"%.0f",  "%.1f",  "%.2f", "%.3f",  "%.4f",  "%.5f",  "%.6f",
				"%.7f",	 "%.8f",  "%.9f", "%.10f", "%.11f", "%.12f", "%.13f",
				"%.14f", "%.15f", "%.16f",
			};
			assert(precision <= 16);
			format_ = sfmt[precision];
		}
	}


	template<class T, class ReadWriteT, class PredicateT>
	void one_column__no_decimal__dumper<T, ReadWriteT, PredicateT>::run()
	{
		for (uint y = 0; y != (this->mtx_)->height; ++y)
		{
			MatrixScalar<ReadWriteT>::Append(this->buffer_, (ReadWriteT)this->predicate_((this->mtx_)->entry[0][y]));
			this->buffer_ += '\n';
		}
	}

	template<class T, class ReadWriteT, class PredicateT>
	void one_column__decimals__dumper<T, ReadWriteT, PredicateT>::run()
	{
		for (uint y = 0; y != (this->mtx_)->height; ++y)
		{
			MatrixScalar<ReadWriteT>::Append(this->buffer_, (ReadWriteT)this->predicate_((this->mtx_)->entry[0][y]), this->format_);
			this->buffer_ += '\n';
		}
	}

	template<class T, class ReadWriteT, class PredicateT>
	void multiple_columns__no_decimal__dumper<T, ReadWriteT, PredicateT>::run()
	{
		for (uint y = 0; y < (this->mtx_)->height; ++y)
		{
			MatrixScalar<ReadWriteT>::Append(this->buffer_, (ReadWriteT)this->predicate_((this->mtx_)->entry[0][y]));
			for (uint x = 1; x < (this->mtx_)->width; ++x)
			{
				this->buffer_ += '\t';
				MatrixScalar<ReadWriteT>::Append(this->buffer_, (ReadWriteT)this->predicate_((this->mtx_)->entry[x][y]));
			}
			this->buffer_ += '\n';
		}
	}

	template<class T, class ReadWriteT, class PredicateT>
	void multiple_columns__decimals__dumper<T, ReadWriteT, PredicateT>::run()
	{
		for (uint y = 0; y < (this->mtx_)->height; ++y)
		{
			MatrixScalar<ReadWriteT>::Append(this->buffer_, (ReadWriteT)this->predicate_((this->mtx_)->entry[0][y]), this->format_);
			for (uint x = 1; x < (this->mtx_)->width; ++x)
			{
				this->buffer_ += '\t';
				MatrixScalar<ReadWriteT>::Append(this->buffer_, (ReadWriteT)this->predicate_((this->mtx_)->entry[x][y]), this->format_);
			}
			this->buffer_ += '\n';
		}
	}

}	// namespace Antares

# undef ANTARES_MATRIX_SNPRINTF

#endif	// __ANTARES_LIBS_ARRAY_MATRIX_TO_BUFFER_SENDER_HXX__
