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
#ifndef YUNI_UNIT_IMPL



# define YUNI_UNIT_IMPL(CLASS,NAME,SYMBOL,COEFFTYPE,COEFF) \
	\
	template<typename T = double, typename P = T> \
	class CLASS final : public Abstract<Quantity, CLASS<T,P>, T, P, COEFFTYPE> \
	{ \
	public: \
		/*! Base class */ \
		typedef Abstract<Quantity, CLASS<T,P>, T,P, COEFFTYPE> AbstractType;  \
		/*! Type */ \
		typedef CLASS<T,P> Type; \
		/*! Unit of measurement */ \
		static const char* Symbol() {return SYMBOL;} \
		/*! Unit of measurement */ \
		static const char* UnitName() {return NAME;} \
		/*! Relation to SI */ \
		static COEFFTYPE RelationToSI() {return (COEFFTYPE)(COEFF);}\
		\
	public: \
		/*! Default constructor */ \
		CLASS() {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const sint16 u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const sint32 u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const sint64 u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const uint16 u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const uint32 u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const uint64 u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const float u) :AbstractType(u) {} \
		/*! Constructor from a fundamental value */ \
		CLASS(const double u) :AbstractType(u) {} \
		/*! Constructor from another unit */ \
		template<class Q1, typename T1, class C1, typename P1, typename N1> \
		CLASS(const Abstract<Q1,C1, T1,P1, N1>& rhs) :AbstractType(rhs) {} \
		\
		/*! \name Operator `=` */ \
		/*@{*/ \
		/*! Builtin types */ \
		AbstractType& operator = (const sint16 u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const sint32 u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const sint64 u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const uint16 u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const uint32 u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const uint64 u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const float u) {return AbstractType::operator = (u);} \
		AbstractType& operator = (const double u) {return AbstractType::operator = (u);} \
		/*! Itself (same metric) */ \
		template<typename P1> \
		AbstractType& operator = (const Abstract<Quantity,Type,T,P1,COEFFTYPE>& rhs) {return AbstractType::operator = (rhs);} \
		/* Another metric system */ \
		template<class Q1, typename T1, class C1, typename P1, typename N1> \
		AbstractType& operator = (const Abstract<Q1,C1,T1,P1,N1>& rhs) \
		{return AbstractType::operator = (rhs);} \
		/*@}*/ \
	}



#endif

