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
#include <ostream>
#include "../../yuni.h"



namespace Yuni
{

/*!
** \brief Units of measurement
** \ingroup Units
*/
namespace Unit
{


	/*!
	** \brief Any value of a physical quantity, in a given unit of measurement (Abstract)
	**
	** This class represents any value of a physical quantity and behaves like a
	** fundamental type. Its allows to express a physical value in terms of
	** another unit, via implicit conversions.
	**
	** Example :
	** \code
	** Unit::Length::Kilometer<> k(10.2); // 10.2 Km
	** // Displays the distance in meters : `10200m`
	** std::cout << Unit::Length::Meter<>(k) << Unit::Length::Meter::Symbol() << std::endl;
	** // Displays the distance in inch
	** std::cout << Unit::Length::Inch<>(k) << Unit::Length::Inch::Symbol() << std::endl;
	** \endcode
	**
	**
	** It is impossible by design to make conversions from a quantity to another
	** one (for example a length and a temperature). If it is really what you
	** want to do, you have to explicitly use the method `value()` to avoid any
	** implicit conversion.
	** \code
	** Unit::Temperature::Kelvin k(42);
	** Unit::Length::Meter<> m = k;          // Won't compile
	** Unit::Length::Meter<> m = k.value();  // Ok, no conversion is done here
	** \endcode
	**
	**
	** The rounding of results / calculation can be controlled with the second
	** template parameter. By default the type used to perform computations is
	** the same than the stored value.
	** \code
	** Unit::Length::Metre<int> m(600); // 600 m
	**
	** Unit::Length::Kilometre<int> k1(m);
	** std::cout << "In km (int) : " k1 << std::endl; // 0
	**
	** Unit::Length::Kilometre<double> k2(m);
	** std::cout << "In km (double)" << k2 << std::endl; // 0.6
	**
	** Unit::Length::Metre<int> m1(k); // should be 600 m
	** std::cout << "In meters (int for conversions)    : " << m1 << std::endl; // 599m
	** Unit::Length::Metre<int, double> m2(k); // should be 600 m
	** std::cout << "In meters (double for conversions) : " << m2 << std::endl; // 600m
	** \endcode
	**
	**
	** This class should not be directly used, even for template methods.
	** Always prefer the SI bse unit.
	** \code
	** #include <yuni/unit/length.h>
	** using namespace Yuni;
	**
	** void foo(const Unit::Length::SIBaseUnit<double>& distance)
	** {
	**  	std::cout << distance << " " << distance.unitName() << std::endl;
	** }
	** \endcode
	**
	** A better example would be :
	** \code
	** #include <yuni/unit/length.h>
	** using namespace Yuni;
	**
	** template<typename T, typename ResultsRounding = T>
	** void foo(const Unit::Length::SIBaseUnit<T, ResultsRounding>& distance)
	** {
	**  	std::cout << distance << " " << distance.unitName() << std::endl;
	** }
	** \endcode
	**
	**
	** \note This class is not thread-safe.
	**
	** \todo Find a way to implement units conversion by factor-label
	**       to compute Dimensional analysis
	**
	** \internal Here is an exemple to implement generic methods for a given quantity (an angle here):
	** \code
	** #include <yuni/unit/angle.h>
	** using namespace Yuni;
	**
	** template<class C, typename T, typename P, typename N>
	** void foo(const Unit::Abstract<Unit::Angle::Quantity, C,T,P,N>& value)
	** {
	** 		// The value may be a radian, a degree...
	** 		...
	** }
	** \endcode
	**
	**
	** \ingroup Units
	**
	** \tparam Q The quantity class which the unit belongs to
	** \tparam Child The real type of the Unit (static polymorphism)
	** \tparam T The type of the stored value
	** \tparam P The type used for computations (to deal with rounding of results)
	** \tparam N Fundamental Type of the relation to SI
	*/
	template<class Q, class Child, typename T, typename P, typename N>
	class Abstract
	{
	public:
		//! The SI category
		typedef Q  QuantityType;
		//! Type
		typedef Abstract<Q,Child,T,P,N>  AbstractType;
		//! Real type
		typedef Child  Type;
		//! The stored type
		typedef T InnerType;

		//! Symbol used for the Unit of measurement (may be empty)
		static const char* Symbol() {return Child::Symbol();}
		//! Name of the Unit of measurement
		static const char* UnitName() {return Child::UnitName();}

		//! Relation to SI
		static N RelationToSI() {return Child::RelationToSI();}

	public:
		//! \name Constructors
		//@{
		//! Default constructor
		Abstract();

		//! Constructor from a raw value
		Abstract(sint16 u);
		//! Constructor from a raw value
		Abstract(sint32 u);
		//! Constructor from a raw value
		Abstract(sint64 u);
		//! Constructor from a raw value
		Abstract(uint16 u);
		//! Constructor from a raw value
		Abstract(uint32 u);
		//! Constructor from a raw value
		Abstract(uint64 u);
		//! Constructor from a raw value
		Abstract(float u);
		//! Constructor from a raw value
		Abstract(double u);

		/*!
		** \brief Copy Constructor (from the same unit of measurement)
		**
		** \param rhs The value to copy
		** \tparam P1 Any precision, which will change nothing in this case
		*/
		template<typename P1>
		Abstract(const Abstract<Q,Child, T,P1,N>& rhs);

		/*!
		** \brief Constructor from another quantity
		**
		** \param rhs The value to convert
		** \tparam Q1 Another quantity
		** \tparam T1 Any type
		** \tparam P1 Any precision associated to the ratio N1/M1
		** \tparam N1 The nominator of the ration N1/M1
		** \tparam M1 The denominator of the ration N1/M1
		*/
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		Abstract(const Abstract<Q1,C1, T1,P1,N1>& rhs);
		//@}


		//! \name Value
		//@{
		/*!
		** \brief The value, without any conversion
		*/
		T value() const {return pValue;}
		//@}


		//! \name Informations about the Unit
		//@{
		//! Symbol used for the Unit of measurement (may be empty)
		const char* symbol() const {return Child::Symbol();}
		//! Name of the Unit of measurement
		const char* unitName() const {return Child::UnitName();}
		//! Relation to SI
		N relationToSI() const {return Child::RelationToSI();}
		//@}

		//! \name Conversion to SI
		//@{
		template<typename U>
		U toSIBaseUnit() const {return (U)((P)pValue * (P)Child::RelationToSI());}
		//@}

		//! \name Opertor `=`
		//@{
		// Builtin types
		AbstractType& operator = (sint16 u);
		AbstractType& operator = (sint32 u);
		AbstractType& operator = (sint64 u);
		AbstractType& operator = (uint16 u);
		AbstractType& operator = (uint32 u);
		AbstractType& operator = (uint64 u);
		AbstractType& operator = (float u);
		AbstractType& operator = (double u);
		AbstractType& operator = (long double u);
		// Itself (same quantity)
		template<typename P1>
		AbstractType& operator = (const Abstract<Q,Child, T,P1,N>& rhs);
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		AbstractType& operator = (const Abstract<Q1,C1,T1,P1,N1>& rhs);
		//@}


		//! \name Operators `+=`
		//@{
		// Builtin types
		AbstractType& operator += (sint16 u);
		AbstractType& operator += (sint32 u);
		AbstractType& operator += (sint64 u);
		AbstractType& operator += (uint16 u);
		AbstractType& operator += (uint32 u);
		AbstractType& operator += (uint64 u);
		AbstractType& operator += (float u);
		AbstractType& operator += (double u);
		AbstractType& operator += (long double u);
		// Itself (same quantity)
		template<typename P1>
		AbstractType& operator += (const Abstract<Q,Child, T,P1,N>& rhs);
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		AbstractType& operator += (const Abstract<Q1,C1,T1,P1,N1>& rhs);
		//@}

		//! \name Operators `-=`
		//@{
		// Builtin types
		AbstractType& operator -= (sint16 u);
		AbstractType& operator -= (sint32 u);
		AbstractType& operator -= (sint64 u);
		AbstractType& operator -= (uint16 u);
		AbstractType& operator -= (uint32 u);
		AbstractType& operator -= (uint64 u);
		AbstractType& operator -= (float u);
		AbstractType& operator -= (double u);
		AbstractType& operator -= (long double u);
		// Itself (same quantity)
		template<typename P1>
		AbstractType& operator -= (const Abstract<Q,Child, T,P1,N>& rhs);
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		AbstractType& operator -= (const Abstract<Q1,C1,T1,P1,N1>& rhs);
		//@}

		//! \name Operators `*=`
		//@{
		// Builtin types
		AbstractType& operator *= (sint16 u);
		AbstractType& operator *= (sint32 u);
		AbstractType& operator *= (sint64 u);
		AbstractType& operator *= (uint16 u);
		AbstractType& operator *= (uint32 u);
		AbstractType& operator *= (uint64 u);
		AbstractType& operator *= (float u);
		AbstractType& operator *= (double u);
		AbstractType& operator *= (long double u);
		// Itself (same quantity)
		template<typename P1>
		AbstractType& operator *= (const Abstract<Q,Child, T,P1,N>& rhs);
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		AbstractType& operator *= (const Abstract<Q1,C1,T1,P1,N1>& rhs);
		//@}

		//! \name Operators `/=`
		//@{
		// Builtin types
		AbstractType& operator /= (sint16 u);
		AbstractType& operator /= (sint32 u);
		AbstractType& operator /= (sint64 u);
		AbstractType& operator /= (uint16 u);
		AbstractType& operator /= (uint32 u);
		AbstractType& operator /= (uint64 u);
		AbstractType& operator /= (float u);
		AbstractType& operator /= (double u);
		AbstractType& operator /= (long double u);
		// Itself (same quantity)
		template<typename P1>
		AbstractType& operator /= (const Abstract<Q,Child, T,P1,N>& rhs);
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		AbstractType& operator /= (const Abstract<Q1,C1,T1,P1,N1>& rhs);
		//@}

		//! \name Operators `<=`
		//@{
		// Builtin types
		bool operator <= (int u) const;
		bool operator <= (uint u) const;
		bool operator <= (float u) const;
		bool operator <= (double u) const;
		bool operator <= (long double u) const;
		// Itself (same quantity)
		template<typename P1>
		bool operator <= (const Abstract<Q,Child, T,P1,N>& rhs) const;
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		bool operator <= (const Abstract<Q1,C1,T1,P1,N1>& rhs) const;
		//@}

		//! \name Operators `>=`
		//@{
		// Builtin types
		bool operator >= (int u) const;
		bool operator >= (uint u) const;
		bool operator >= (float u) const;
		bool operator >= (double u) const;
		// Itself (same quantity)
		template<typename P1>
		bool operator >= (const Abstract<Q,Child, T,P1,N>& rhs) const;
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		bool operator >= (const Abstract<Q1,C1,T1,P1,N1>& rhs) const;
		//@}

		//! \name Operators `<`
		//@{
		// Builtin types
		bool operator < (int u) const;
		bool operator < (uint u) const;
		bool operator < (float u) const;
		bool operator < (double u) const;
		// Itself (same quantity)
		template<typename P1>
		bool operator < (const Abstract<Q,Child, T,P1,N>& rhs) const;
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		bool operator < (const Abstract<Q1,C1,T1,P1,N1>& rhs) const;
		//@}

		//! \name Operators `>`
		//@{
		// Builtin types
		bool operator > (int u) const;
		bool operator > (uint u) const;
		bool operator > (float u) const;
		bool operator > (double u) const;
		// Itself (same quantity)
		template<typename P1>
		bool operator > (const Abstract<Q,Child, T,P1,N>& rhs) const;
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		bool operator > (const Abstract<Q1,C1,T1,P1,N1>& rhs) const;
		//@}

		//! \name Operators `==`
		//@{
		// Builtin types
		bool operator == (int u) const;
		bool operator == (uint u) const;
		bool operator == (float u) const;
		bool operator == (double u) const;
		// Itself (same quantity)
		template<typename P1>
		bool operator == (const Abstract<Q,Child, T,P1,N>& rhs) const;
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		bool operator == (const Abstract<Q1,C1,T1,P1,N1>& rhs) const;
		//@}

		//! \name Operators `!=`
		//@{
		// Builtin types
		bool operator != (int u) const;
		bool operator != (uint u) const;
		bool operator != (float u) const;
		bool operator != (double u) const;
		// Itself (same quantity)
		template<typename P1>
		bool operator != (const Abstract<Q,Child, T,P1,N>& rhs) const;
		// Another quantity
		template<class Q1, typename T1, class C1, typename P1, typename N1>
		bool operator != (const Abstract<Q1,C1,T1,P1,N1>& rhs) const;
		//@}

		//! \name Operator `++`
		//@{
		AbstractType& operator ++ ();
		AbstractType  operator ++ (int);
		//@}

		//! \name Operator `--`
		//@{
		AbstractType& operator -- ();
		AbstractType  operator -- (int);
		//@}

		//! \name Operator `!`
		//@{
		bool operator ! () const {return !pValue;}
		//@}


	protected:
		//! The value
		T pValue;

	}; // class Abstract







	template<class FromQ, typename FromType, class ToQ, typename ToType>
	struct QuantityConversion final
	{
		static ToType Value(const FromType u);
	};

	template<class Q, typename FromType, typename ToType>
	struct QuantityConversion<Q, FromType, Q, ToType> final
	{
		static ToType Value(const FromType u) {return u;}
	};




} // namespace Unit
} // namespace Yuni


#include "unit.hxx"





//! \name Operator overload for stream printing
//@{
template<class Q, class Child, typename T, typename P, typename N>
inline std::ostream& operator << (std::ostream& out, const Yuni::Unit::Abstract<Q,Child, T,P,N>& m)
{ out << m.value(); return out; }
//@}


template<class Q, class Child, typename T, typename P, typename N,
	class Child1, typename T1, typename P1, typename N1>
inline const Yuni::Unit::Abstract<Q,Child, T,P,N> operator + (const Yuni::Unit::Abstract<Q,Child, T,P,N>& lhs,
	const Yuni::Unit::Abstract<Q,Child1, T1,P1,N1>& rhs)
{ return Yuni::Unit::Abstract<Q,Child, T,P,N>(lhs) += rhs; }


template<class Q, class Child, typename T, typename P, typename N,
	class Child1, typename T1, typename P1, typename N1>
inline const Yuni::Unit::Abstract<Q,Child, T,P,N> operator - (const Yuni::Unit::Abstract<Q,Child, T,P,N>& lhs,
	const Yuni::Unit::Abstract<Q,Child1, T1,P1,N1>& rhs)
{ return Yuni::Unit::Abstract<Q,Child, T,P,N>(lhs) -= rhs; }


template<class Q, class Child, typename T, typename P, typename N,
	class Child1, typename T1, typename P1, typename N1>
inline const Yuni::Unit::Abstract<Q,Child, T,P,N> operator * (const Yuni::Unit::Abstract<Q,Child, T,P,N>& lhs,
	const Yuni::Unit::Abstract<Q,Child1, T1,P1,N1>& rhs)
{ return Yuni::Unit::Abstract<Q,Child, T,P,N>(lhs) *= rhs; }


template<class Q, class Child, typename T, typename P, typename N,
	class Child1, typename T1, typename P1, typename N1>
inline const Yuni::Unit::Abstract<Q,Child, T,P,N> operator / (const Yuni::Unit::Abstract<Q,Child, T,P,N>& lhs,
	const Yuni::Unit::Abstract<Q,Child1, T1,P1,N1>& rhs)
{ return Yuni::Unit::Abstract<Q,Child, T,P,N>(lhs) /= rhs; }
