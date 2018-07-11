/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "../static/types.h"



namespace Yuni
{
namespace Private
{

/*!
** \brief Any implementation details
**
** This namespace stores a big part of the actual Any
** implementation, in particular the Static Function Pointer
** Table Polymorphism Pattern (SFPTPP) implementation.
*/
namespace Any
{


	/*!
	** \brief Static function pointer table
	**
	** All the operations done on Any storage objects are done via
	** the functions currently defined in pTable. Depending on the size of the object,
	** this member may use different approachs to store the data.
	** This function table defines the function pointers to the different operations.
	*/
	struct TypeManipulationTable
	{
		const std::type_info& (*type)();
		void (*staticDelete)(void**);
		void (*clone)(void* const*, void**);
		void (*move)(void* const*, void**);
	};



	/*!
	** \brief Static functions for small objects (smaller than a void *)
	**
	** The goal of this struct is to use pObject as a sort of union
	** to store object smaller than pointer size. Of course, this means that
	** bigger objects can be stored faster if you use 64-bit addresses.
	*/
	template <bool isSmall>
	struct Functions final
	{
		template <typename T>
		struct TypeManipulator
		{
			static const std::type_info& Type()
			{
				return typeid(T);
			}

			static void Delete(void** object)
			{
				reinterpret_cast<T*>(object)->~T();
			}

			static void Clone(void* const* source, void** dest)
			{
				new (dest) T(*reinterpret_cast<T const*>(source));
			}

			static void Move(void* const* source, void** dest)
			{
				reinterpret_cast<T*>(dest)->~T();
				*reinterpret_cast<T*>(dest) = *reinterpret_cast<T const*>(source);
			}
		};
	};


	/*!
	** \brief Static functions implementations for big objects (larger than a void *)
	**
	** This implementation stores big objects as normal objects (ie. a pointer to an alloc'ed
	** object in pObject.)
	*/
	template <>
	struct Functions<false> final
	{
		template <typename T>
		struct TypeManipulator
		{
			static const std::type_info& Type()
			{
				return typeid(T);
			}

			static void Delete(void** object)
			{
				delete(*reinterpret_cast<T**>(object));
			}

			static void Clone(void* const* source, void** dest)
			{
				*dest = new T(**reinterpret_cast<T* const*>(source));
			}

			static void Move(void* const* source, void** dest)
			{
				(*reinterpret_cast<T**>(dest))->~T();
				**reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(source);
			}
		};
	};


	/*!
	** \brief Structure providing function table for a type.
	**
	** This structure provides a function to get the appropriate function table for
	** a given type in an allegedly sexy way. (like Table<T>::get())
	*/
	template<typename T>
	struct Table final
	{
		/*!
		** \brief Returns a pointer on a static function table
		**
		** The static function table depends on the type and its size.
		** One different function table will be instanciated by type.
		*/
		static TypeManipulationTable * Get()
		{
			static TypeManipulationTable staticTable =
			{
				Functions<Static::Type::IsSmall<T>::Yes>::template TypeManipulator<T>::Type,
				Functions<Static::Type::IsSmall<T>::Yes>::template TypeManipulator<T>::Delete,
				Functions<Static::Type::IsSmall<T>::Yes>::template TypeManipulator<T>::Clone,
				Functions<Static::Type::IsSmall<T>::Yes>::template TypeManipulator<T>::Move
			};

			return &staticTable;
		}
	};



	/*!
	** \brief Any type for an empty Any.
	*/
	struct Empty {};



} // namespace Any
} // namespace Private
} // namespace Yuni
