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
#ifndef __YUNI_CORE_UNIT_DATA_DATA_H__
# define __YUNI_CORE_UNIT_DATA_DATA_H__

# include "unit.h"
# include "define.h"


namespace Yuni
{
namespace Unit
{

/*!
** \defgroup UnitData Digital information storage
** \ingroup Units
*/


/*!
** \brief Digital information storage
** \ingroup UnitData
**
**
** \defgroup UnitDataOctet Measured in octets
** \ingroup UnitData
**
** \defgroup UnitDataBit   Measured in bits
** \ingroup UnitData
*/
namespace Data
{


/*!
** \name Digital information storage (measured in bits)
** \ingroup UnitDataBit
*/
namespace Bit
{
	//! \ingroup UnitDataBit
	struct Quantity;



	//! SI (byte)
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(SIBaseUnit, "bit", "B", int, 1);

	//! Zettabit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Zettabit, "zettabit", "zB", uint64_t, 1e21);
	//! Exabit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Exabit, "exabit", "eB", uint64_t, 1e18);
	//! Petabit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Petabit, "petabit", "pB", uint64_t, 1e15);
	//! Terabit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Terabit, "terabit", "tB", uint64_t, 1e12);
	//! Gigabit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Gigabit, "gigabit", "gB", int, 1e9);
	//! Megabit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Megabit, "megabit", "mB", int, 1e6);
	//! Kilobit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Kilobit, "kilobit", "kB", int, 1e3);
	//! Hectobit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Hectobit, "hectobit", "hB", int, 1e2);


	//! Bit
	//! \ingroup UnitDataBit
	YUNI_UNIT_IMPL(Bit, "bit", "B", int, 1);



} // namespace Bit





/*!
** \name Digital information storage (measured in octets)
** \ingroup UnitDataOctet
*/
namespace Octet
{
	//! \ingroup UnitDataOctet
	struct Quantity;


	//! SI (byte)
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(SIBaseUnit, "octet", "o", int, 1);

	//! Zettaoctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Zettaoctet, "zettaoctet", "Zo", int64_t, 1e21);
	//! Exaoctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Exaoctet, "exaoctet", "Eo", int64_t, 1e18);
	//! Petaoctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Petaoctet, "petaoctet", "Po", int64_t, 1e15);
	//! Teraoctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Teraoctet, "teraoctet", "To", int64_t, 1e12);
	//! Gigaoctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Gigaoctet, "gigaoctet", "Go", int, 1e9);
	//! Megaoctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Megaoctet, "megaoctet", "Mo", int, 1e6);
	//! Kilooctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Kilooctet, "kilooctet", "Ko", int, 1e3);


	//! Yobioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Yobioctet, "yobioctet", "Yio", long double, 1208925819614629174706176.);
	//! Zebioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Zebioctet, "zebioctet", "Zio", long double, 1180591620717411303424.);
	//! Exbictet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Exbioctet, "exbioctet", "Eio", uint64_t, 1152921504606846976LL);
	//! Pebioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Pebioctet, "pebioctet", "Pio", uint64_t, 1125899906842624LL);
	//! Tebioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Tebioctet, "tebioctet", "Tio", uint64_t, 1099511627776LL);
	//! Gibioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Gibioctet, "gibioctet", "Gio", uint, 1073741824);
	//! Mebioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Mebioctet, "mebioctet", "Mio", uint, 1048576);
	//! Kibioctet
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Kibioctet, "kibioctet", "Kio", uint, 1024);

	//! Bit
	//! \ingroup UnitDataOctet
	YUNI_UNIT_IMPL(Octet, "octet", "o", int, 1);



} // namespace Octet


} // namespace Data




	/*!
	** \brief Conversion from Octet to Bit
	*/
	template<typename FromType, typename ToType>
	struct QuantityConversion<Data::Octet::Quantity, FromType, Data::Bit::Quantity, ToType>
	{
		static ToType Value(const FromType u) {return ToType(u * 8);}
	};

	/*!
	** \brief Conversion from Bit to Octet
	*/
	template<typename FromType, typename ToType>
	struct QuantityConversion<Data::Bit::Quantity, FromType, Data::Octet::Quantity, ToType>
	{
		static ToType Value(const FromType u) {return ToType(u / 8);}
	};



} // namespace Unit
} // namespace Yuni

# include "undef.h"

#endif // __YUNI_CORE_UNIT_DATA_DATA_H__
