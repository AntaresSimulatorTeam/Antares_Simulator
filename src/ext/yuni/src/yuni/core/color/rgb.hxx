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
#include "rgb.h"



namespace Yuni
{
namespace Extension
{
namespace Color
{

	template<class RGBType, class T>
	struct RGB final
	{
		typedef typename  RGBType::Type Type;
		typedef typename  RGBType::template Calculation<T>::Type CalculationType;

		static inline void Assign(RGBType& color, const T& value)
		{
			enum
			{
				upperBound = Yuni::Private::Color::DefaultValues<T>::upperBound,
				lowerBound = Yuni::Private::Color::DefaultValues<T>::lowerBound,
			};
			// new value
			const CalculationType v = ((int)RGBType::upperBound == (int)upperBound and (int)RGBType::lowerBound == (int)lowerBound)
				? static_cast<CalculationType>(value)
				: static_cast<CalculationType>(value) * RGBType::upperBound / upperBound;
			// assignment
			color.red   = v;
			color.green = v;
			color.blue  = v;
		}

		static bool IsEqual(RGBType& color, const T& value)
		{
			enum
			{
				upperBound = Yuni::Private::Color::DefaultValues<T>::upperBound,
				lowerBound = Yuni::Private::Color::DefaultValues<T>::lowerBound,
			};
			// new value
			const CalculationType v = ((int)RGBType::upperBound == (int)upperBound and (int)RGBType::lowerBound == (int)lowerBound)
				? static_cast<CalculationType>(value)
				: static_cast<CalculationType>(value) * RGBType::upperBound / upperBound;

			return Math::Equals(color.red, v)
				and Math::Equals(color.green, v)
				and Math::Equals(color.blue, v);
		}

		template<bool PlusT>
		static inline void Increase(RGBType& color, const T& value)
		{
			enum
			{
				upperBound = Yuni::Private::Color::DefaultValues<T>::upperBound,
				lowerBound = Yuni::Private::Color::DefaultValues<T>::lowerBound,
			};
			// new value
			const CalculationType v = ((int)RGBType::upperBound == (int)upperBound and (int)RGBType::lowerBound == (int)lowerBound)
				? static_cast<CalculationType>(value)
				: static_cast<CalculationType>(value) * RGBType::upperBound / upperBound;
			// assignment
			if (PlusT)
			{
				color.red   = Math::MinMaxEx<Type>(static_cast<CalculationType>(color.red)   + v, (CalculationType)lowerBound, (CalculationType)upperBound);
				color.green = Math::MinMaxEx<Type>(static_cast<CalculationType>(color.green) + v, (CalculationType)lowerBound, (CalculationType)upperBound);
				color.blue  = Math::MinMaxEx<Type>(static_cast<CalculationType>(color.blue)  + v, (CalculationType)lowerBound, (CalculationType)upperBound);
			}
			else
			{
				color.red   = Math::MinMaxEx<Type>(static_cast<CalculationType>(color.red)   - v, (CalculationType)lowerBound, (CalculationType)upperBound);
				color.green = Math::MinMaxEx<Type>(static_cast<CalculationType>(color.green) - v, (CalculationType)lowerBound, (CalculationType)upperBound);
				color.blue  = Math::MinMaxEx<Type>(static_cast<CalculationType>(color.blue)  - v, (CalculationType)lowerBound, (CalculationType)upperBound);
			}
		}

	}; // class RGB


	template<class U, class V>
	struct RGB<Yuni::Color::RGB<U>, Yuni::Color::RGB<V> > final
	{
		typedef typename Yuni::Color::RGB<U>::template Calculation<V>::Type CalculationType;

		static inline void Assign(Yuni::Color::RGB<U>& color, const Yuni::Color::RGB<V>& value)
		{

			if ((int)Yuni::Color::RGB<U>::upperBound == (int)Yuni::Color::RGB<V>::upperBound
				and (int)Yuni::Color::RGB<U>::lowerBound == (int)Yuni::Color::RGB<V>::lowerBound)
			{
				color.red   = static_cast<U>(value.red);
				color.green = static_cast<U>(value.green);
				color.blue  = static_cast<U>(value.blue);
			}
			else
			{
				color.red   = static_cast<U>(static_cast<CalculationType>(value.red)   * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound);
				color.green = static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound);
				color.blue  = static_cast<U>(static_cast<CalculationType>(value.blue)  * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound);
			}
		}

		static inline bool IsEqual(Yuni::Color::RGB<U>& color, const Yuni::Color::RGB<V>& value)
		{
			if ((int)Yuni::Color::RGB<U>::upperBound == (int)Yuni::Color::RGB<V>::upperBound
				and (int)Yuni::Color::RGB<U>::lowerBound == (int)Yuni::Color::RGB<V>::lowerBound)
			{
				return
					Math::Equals(color.red, static_cast<U>(value.red))
					and Math::Equals(color.green, static_cast<U>(value.green))
					and Math::Equals(color.blue, static_cast<U>(value.blue));
			}
			else
			{
				return
					Math::Equals(color.red, static_cast<U>(static_cast<CalculationType>(value.red)   * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound))
					and Math::Equals(color.green, static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound))
					and Math::Equals(color.blue, static_cast<U>(static_cast<CalculationType>(value.blue)  * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound));
			}
		}

		template<bool PlusT>
		static inline void Increase(Yuni::Color::RGB<U>& color, const Yuni::Color::RGB<V>& value)
		{
			if ((int)Yuni::Color::RGB<U>::upperBound == (int)Yuni::Color::RGB<V>::upperBound
				and (int)Yuni::Color::RGB<U>::lowerBound == (int)Yuni::Color::RGB<V>::lowerBound)
			{
				if (PlusT)
				{
					color.red   = Math::MinMaxEx<U>(static_cast<CalculationType>(color.red)   + static_cast<U>(value.red),   Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(static_cast<CalculationType>(color.green) + static_cast<U>(value.green), Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue  = Math::MinMaxEx<U>(static_cast<CalculationType>(color.blue)  + static_cast<U>(value.blue),  Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
				else
				{
					color.red   = Math::MinMaxEx<U>(static_cast<CalculationType>(color.red)   - static_cast<U>(value.red),   Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(static_cast<CalculationType>(color.green) - static_cast<U>(value.green), Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue  = Math::MinMaxEx<U>(static_cast<CalculationType>(color.blue)  - static_cast<U>(value.blue),  Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
			}
			else
			{
				if (PlusT)
				{
					color.red = Math::MinMaxEx<U>(
						color.red   + static_cast<U>(static_cast<CalculationType>(value.red) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(
						color.green + static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue = Math::MinMaxEx<U>(
						color.blue  + static_cast<U>(static_cast<CalculationType>(value.blue) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
				else
				{
					color.red = Math::MinMaxEx<U>(
						color.red   - static_cast<U>(static_cast<CalculationType>(value.red) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(
						color.green - static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue = Math::MinMaxEx<U>(
						color.blue  - static_cast<U>(static_cast<CalculationType>(value.blue) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGB<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
			}
		}

	}; // class RGB



	template<class U, class V>
	struct RGB<Yuni::Color::RGB<U>, Yuni::Color::RGBA<V> > final
	{
		typedef typename Yuni::Color::RGB<U>::template Calculation<V>::Type CalculationType;

		static inline void Assign(Yuni::Color::RGB<U>& color, const Yuni::Color::RGBA<V>& value)
		{
			if ((int)Yuni::Color::RGB<U>::upperBound == (int)Yuni::Color::RGBA<V>::upperBound
				and (int)Yuni::Color::RGB<U>::lowerBound == (int)Yuni::Color::RGBA<V>::lowerBound)
			{
				color.red   = static_cast<U>(value.red);
				color.green = static_cast<U>(value.green);
				color.blue  = static_cast<U>(value.blue);
			}
			else
			{
				color.red   = static_cast<U>(static_cast<CalculationType>(value.red)   * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound);
				color.green = static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound);
				color.blue  = static_cast<U>(static_cast<CalculationType>(value.blue)  * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound);
			}
		}

		static inline bool IsEqual(Yuni::Color::RGB<U>& color, const Yuni::Color::RGBA<V>& value)
		{
			if ((int)Yuni::Color::RGB<U>::upperBound == (int)Yuni::Color::RGBA<V>::upperBound
				and (int)Yuni::Color::RGB<U>::lowerBound == (int)Yuni::Color::RGBA<V>::lowerBound)
			{
				return
					Math::Equals(value.alpha, Yuni::Color::RGBA<V>::upperBound)
					and Math::Equals(color.red, static_cast<U>(value.red))
					and Math::Equals(color.green, static_cast<U>(value.green))
					and Math::Equals(color.blue, static_cast<U>(value.blue));
			}
			else
			{
				return
					Math::Equals(value.alpha, Yuni::Color::RGBA<V>::upperBound)
					and Math::Equals(color.red, static_cast<U>(static_cast<CalculationType>(value.red)   * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound))
					and Math::Equals(color.green, static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound))
					and Math::Equals(color.blue, static_cast<U>(static_cast<CalculationType>(value.blue)  * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound));
			}
		}

		template<bool PlusT>
		static inline void Increase(Yuni::Color::RGB<U>& color, const Yuni::Color::RGBA<V>& value)
		{
			if ((int)Yuni::Color::RGB<U>::upperBound == (int)Yuni::Color::RGBA<V>::upperBound
				and (int)Yuni::Color::RGB<U>::lowerBound == (int)Yuni::Color::RGBA<V>::lowerBound)
			{
				if (PlusT)
				{
					color.red   = Math::MinMaxEx<U>(static_cast<CalculationType>(color.red)   + static_cast<U>(value.red),   Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(static_cast<CalculationType>(color.green) + static_cast<U>(value.green), Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue  = Math::MinMaxEx<U>(static_cast<CalculationType>(color.blue)  + static_cast<U>(value.blue),  Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
				else
				{
					color.red   = Math::MinMaxEx<U>(static_cast<CalculationType>(color.red)   - static_cast<U>(value.red),   Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(static_cast<CalculationType>(color.green) - static_cast<U>(value.green), Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue  = Math::MinMaxEx<U>(static_cast<CalculationType>(color.blue)  - static_cast<U>(value.blue),  Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
			}
			else
			{
				if (PlusT)
				{
					color.red = Math::MinMaxEx<U>(
						color.red   + static_cast<U>(static_cast<CalculationType>(value.red) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(
						color.green + static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue = Math::MinMaxEx<U>(
						color.blue  + static_cast<U>(static_cast<CalculationType>(value.blue) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
				else
				{
					color.red = Math::MinMaxEx<U>(
						color.red   - static_cast<U>(static_cast<CalculationType>(value.red) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.green = Math::MinMaxEx<U>(
						color.green - static_cast<U>(static_cast<CalculationType>(value.green) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
					color.blue = Math::MinMaxEx<U>(
						color.blue  - static_cast<U>(static_cast<CalculationType>(value.blue) * Yuni::Color::RGB<U>::upperBound / Yuni::Color::RGBA<V>::upperBound),
						Yuni::Color::RGB<U>::lowerBound, Yuni::Color::RGB<U>::upperBound);
				}
			}
		}

	}; // class RGB



} // namespace Color
} // namespace Extension
} // namespace Yuni


namespace Yuni
{
namespace Color
{

	template<class T>
	inline RGB<T>::RGB()
		: red(lowerBound), green(lowerBound), blue(lowerBound)
	{}


	template<class T>
	inline RGB<T>::RGB(const RGB<T>& rhs)
		: red(rhs.red), green(rhs.green), blue(rhs.blue)
	{}


	template<class T>
	template<class U>
	inline RGB<T>::RGB(const U& value)
	{
		Yuni::Extension::Color::RGB<RGB<T>, U>::Assign(*this, value);
	}


	template<class T>
	template<class R, class G, class B, class A>
	inline RGB<T>::RGB(R r, G g, B b, A /* a */)
		: red( Math::MinMaxEx<T>(r, static_cast<R>(lowerBound), static_cast<R>(upperBound)))
		, green(Math::MinMaxEx<T>(g, static_cast<G>(lowerBound), static_cast<G>(upperBound)))
		, blue( Math::MinMaxEx<T>(b, static_cast<B>(lowerBound), static_cast<B>(upperBound)))
	{}


	template<class T>
	template<class R, class G, class B>
	inline RGB<T>::RGB(R r, G g, B b)
		: red( Math::MinMaxEx<T>(r, static_cast<R>(lowerBound), static_cast<R>(upperBound)))
		, green(Math::MinMaxEx<T>(g, static_cast<G>(lowerBound), static_cast<G>(upperBound)))
		, blue( Math::MinMaxEx<T>(b, static_cast<B>(lowerBound), static_cast<B>(upperBound)))
	{}


	template<class T>
	template<class R, class G, class B, class A>
	inline void RGB<T>::assign(R r, G g, B b, A /* a */)
	{
		red   = Math::MinMaxEx<T>(r, static_cast<R>(lowerBound), static_cast<R>(upperBound));
		green = Math::MinMaxEx<T>(g, static_cast<G>(lowerBound), static_cast<G>(upperBound));
		blue  = Math::MinMaxEx<T>(b, static_cast<B>(lowerBound), static_cast<B>(upperBound));
	}


	template<class T>
	template<class R, class G, class B>
	inline void RGB<T>::assign(R r, G g, B b)
	{
		red   = Math::MinMaxEx<T>(r, static_cast<R>(lowerBound), static_cast<R>(upperBound));
		green = Math::MinMaxEx<T>(g, static_cast<G>(lowerBound), static_cast<G>(upperBound));
		blue  = Math::MinMaxEx<T>(b, static_cast<B>(lowerBound), static_cast<B>(upperBound));
	}


	template<class T>
	template<class U>
	inline void RGB<T>::assign(const U& value)
	{
		Yuni::Extension::Color::RGB<RGB<T>, U>::Assign(*this, value);
	}


	template<class T>
	inline bool RGB<T>::valid() const
	{
		return
			(    red   >= static_cast<T>(lowerBound) and red   <= static_cast<T>(upperBound))
			and (green >= static_cast<T>(lowerBound) and green <= static_cast<T>(upperBound))
			and (blue  >= static_cast<T>(lowerBound) and blue  <= static_cast<T>(upperBound));
	}


	template<class T>
	inline bool RGB<T>::visible() const
	{
		return true;
	}


	template<class T>
	inline RGB<T>& RGB<T>::operator = (const Yuni::NullPtr&)
	{
		red   = T();
		green = T();
		blue  = T();
		return *this;
	}


	template<class T>
	inline RGB<T>& RGB<T>::operator = (const RGB<T>& rhs)
	{
		Yuni::Extension::Color::RGB<RGB<T>, RGB<T> >::Assign(*this, rhs);
		return *this;
	}


	template<class T>
	template<class U>
	inline RGB<T>& RGB<T>::operator = (const U& rhs)
	{
		Yuni::Extension::Color::RGB<RGB<T>, U>::Assign(*this, rhs);
		return *this;
	}


	template<class T>
	template<class U>
	inline bool RGB<T>::operator != (const U& rhs) const
	{
		return ! Yuni::Extension::Color::RGB<RGB<T>, U>::IsEqual(*this, rhs);
	}


	template<class T>
	template<class U>
	inline bool RGB<T>::operator == (const U& rhs) const
	{
		return Yuni::Extension::Color::RGB<RGB<T>, U>::IsEqual(*this, rhs);
	}


	template<class T>
	template<class U>
	inline RGB<T>& RGB<T>::operator += (const U& rhs)
	{
		Yuni::Extension::Color::RGB<RGB<T>, U>::template Increase<true>(*this, rhs);
		return *this;
	}


	template<class T>
	template<class U>
	inline RGB<T>& RGB<T>::operator -= (const U& rhs)
	{
		Yuni::Extension::Color::RGB<RGB<T>, U>::template Increase<false>(*this, rhs);
		return *this;
	}


	template<class T>
	inline T& RGB<T>::operator [] (const uint i)
	{
		switch (i)
		{
			case 0:
				return red;
			case 1:
				return green;
			case 2:
				return blue;
			default:
				assert(false and "Index out of bounds !");
		}
		return red;
	}


	template<class T>
	template<class StreamT>
	void RGB<T>::print(StreamT& stream) const
	{
		stream << "rgb("
			<< static_cast<PrintType>(red) << ','
			<< static_cast<PrintType>(green) << ','
			<< static_cast<PrintType>(blue) << ')';
	}




} // namespace Color
} // namespace Yuni



namespace Yuni
{
namespace Extension
{
namespace CString
{

	template<class CStringT, class T>
	class Append<CStringT, Yuni::Color::RGB<T> > final
	{
	public:
		static void Perform(CStringT& s, const Yuni::Color::RGB<T>& rhs)
		{
			rhs.print(s);
		}
	};


	template<class T>
	class Into<Yuni::Color::RGB<T> > final
	{
	public:
		typedef Yuni::Color::RGB<T> TargetType;
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, TargetType& out)
		{
			if (s.empty())
			{
				out = nullptr;
				return true;
			}
			if (!InternalPerform(s, out))
			{
				out = nullptr;
				return false;
			}
			return true;
		}

		template<class StringT> static TargetType Perform(const StringT& s)
		{
			return TargetType(s.c_str(), s.size());
		}

	private:
		template<class StringT>
		static bool InternalPerform(const StringT& s, TargetType& out)
		{
			enum
			{
				upperBound = Yuni::Private::Color::DefaultValues<T>::upperBound,
				lowerBound = Yuni::Private::Color::DefaultValues<T>::lowerBound,
			};

			const typename StringT::const_utf8iterator end = s.utf8end();
			typename StringT::const_utf8iterator i = s.utf8begin();

			if (!i.findFirstNonSpace(end.offset()))
			{
				out = nullptr; // empty string
				return true;
			}
			// trimming
			if (*i != 'r' or ++i == end or *i != 'g' or ++i == end or *i != 'b' or ++i == end)
				return false;
			bool hasAlpha = (*i == 'a');
			if (hasAlpha and ++i == end)
				return false;
			if (*i != '(')
			{
				if (not i.findFirstNonSpace(end.offset()) or *i != '(')
					return false;
			}
			if (++i == end or !i.findFirstNonSpace(end.offset()))
			{
				out = nullptr; // empty string
				return true;
			}

			// A temporary string buffer for convertion
			Yuni::CString<30, false> tmp;
			// A copy of the offset
			typename StringT::const_utf8iterator j = i;
			// A temporary value for calculations
			typename Yuni::Color::RGB<T>::template Calculation<T>::Type channel;

			// RED CHANNEL
			if (!i.find(',', end.offset()))
				return false;
			tmp.assign(j, i);
			if (!tmp.to(channel) or ++i == end or !i.findFirstNonSpace(end.offset()))
				return false;
			out.red = Math::MinMaxEx<T>(channel, static_cast<T>(lowerBound), static_cast<T>(upperBound));

			// GREEN CHANNEL
			j = i;
			if (!i.find(',', end.offset()))
				return false;
			tmp.assign(j, i);
			if (!tmp.to(channel) or ++i == end or !i.findFirstNonSpace(end.offset()))
				return false;
			out.green = Math::MinMaxEx<T>(channel, static_cast<T>(lowerBound), static_cast<T>(upperBound));

			if (hasAlpha)
			{
				// BLUE CHANNEL
				j = i;
				if (!i.find(',', end.offset()))
					return false;
				tmp.assign(j, i);
				if (!tmp.to(channel) or ++i == end or !i.findFirstNonSpace(end.offset()))
					return false;
				out.blue = Math::MinMaxEx<T>(channel, static_cast<T>(lowerBound), static_cast<T>(upperBound));

				// skipping alpha channel
			}
			else
			{
				// BLUE CHANNEL
				j = i;
				if (!i.find(')', end.rawOffset()))
					return false;
				tmp.assign(j, i);
				if (!tmp.to(channel))
					return false;
				out.blue = static_cast<T>(channel);
			}

			return true;

		}
	};



} // namespace CString
} // namespace Extension
} // namespace Yuni





template<class T>
inline std::ostream& operator << (std::ostream& out, const Yuni::Color::RGB<T>& rhs)
{
	rhs.print(out);
	return out;
}


template<class U, class V>
inline
Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type>
operator + (const Yuni::Color::RGB<U> a, const Yuni::Color::RGB<V>& b)
{
	Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type> r = a;
	r += b;
	return r;
}

template<class U, class V>
inline
Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type>
operator - (const Yuni::Color::RGB<U>& a, const Yuni::Color::RGB<V>& b)
{
	Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type> r = a;
	r -= b;
	return r;
}


template<class U, class V>
inline
Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type>
operator + (const Yuni::Color::RGBA<U> a, const Yuni::Color::RGB<V>& b)
{
	Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type> r = a;
	r += b;
	return r;
}

template<class U, class V>
inline
Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type>
operator - (const Yuni::Color::RGBA<U>& a, const Yuni::Color::RGB<V>& b)
{
	Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type> r = a;
	r -= b;
	return r;
}

template<class U, class V>
inline
Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type>
operator + (const Yuni::Color::RGB<U> a, const Yuni::Color::RGBA<V>& b)
{
	Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type> r = a;
	r += b;
	return r;
}

template<class U, class V>
inline
Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type>
operator - (const Yuni::Color::RGB<U>& a, const Yuni::Color::RGBA<V>& b)
{
	Yuni::Color::RGB<typename Yuni::Color::template RGB<U>::template Calculation<V>::Type> r = a;
	r -= b;
	return r;
}



# define YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(V) \
	template<class T> \
	inline \
	Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> \
	operator + (V v, const Yuni::Color::RGB<T>& rhs) \
	{ \
		Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> r = rhs; \
		r += v; \
		return r; \
	} \
	template<class T> \
	inline \
	Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> \
	operator - (V v, const Yuni::Color::RGB<T>& rhs) \
	{ \
		Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> r = rhs; \
		r -= v; \
		return r; \
	}


# define YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(V) \
	template<class T> \
	inline \
	Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> \
	operator + (const Yuni::Color::RGB<T>& rhs, V v) \
	{ \
		Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> r = rhs; \
		r += v; \
		return r; \
	} \
	template<class T> \
	inline \
	Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> \
	operator - (const Yuni::Color::RGB<T>& rhs, V v) \
	{ \
		Yuni::Color::RGB<typename Yuni::Color::template RGB<T>::template Calculation<V>::Type> r = rhs; \
		r -= v; \
		return r; \
	}


YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::sint8)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::sint16)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::sint32)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::sint64)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::uint8)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::uint16)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::uint32)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(Yuni::uint64)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(float)
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(double)
# ifdef YUNI_HAS_LONG_DOUBLE
YUNI_COLOR_RGB_OPERATOR_TYPE_RGB(long double)
# endif

YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::sint8)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::sint16)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::sint32)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::sint64)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::uint8)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::uint16)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::uint32)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(Yuni::uint64)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(float)
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(double)
# ifdef YUNI_HAS_LONG_DOUBLE
YUNI_COLOR_RGB_OPERATOR_RGB_TYPE(long double)
# endif

# undef YUNI_COLOR_RGB_OPERATOR_TYPE_RGB
# undef YUNI_COLOR_RGB_OPERATOR_RGB_TYPE

