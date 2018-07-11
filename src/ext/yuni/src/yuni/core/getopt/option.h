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
#include "../string.h"
#include <cassert>



namespace Yuni
{
namespace Private
{
namespace GetOptImpl
{

	// Forward declaration
	class Tokenizer;


	template<class T>
	class Value final
	{
	public:
		static bool Add(T& out, const char* c_str, const String::size_type len)
		{
			out = (len) ? String(c_str, len).to<T>() : T();
			return true;
		}

	};


	template<uint ChunkT, bool ExpT>
	class Value<CString<ChunkT, ExpT> >
	{
	public:
		static bool Add(CString<ChunkT,ExpT>& out, const char* c_str, const String::size_type len)
		{
			out.assign(c_str, len);
			return true;
		}
	};


	template<class C, class Traits, class Alloc>
	class Value<std::basic_string<C, Traits, Alloc> >
	{
	public:
		static bool Add(std::basic_string<C,Traits,Alloc>& out, const char* c_str, const String::size_type len)
		{
			if (len)
				out += c_str;
			else
				out.clear();
			return true;
		}
	};


	template<template<class,class> class L, class T, class Alloc>
	class Value<L<T, Alloc> >
	{
	public:
		static bool Add(L<T,Alloc>& out, const char* c_str, const String::size_type len)
		{
			if (len)
				out.push_back(String(c_str, len).to<T>());
			else
				out.push_back(T());
			return true;
		}
	};


	template<template<class, class> class L, uint ChunkT, bool ExpT, class Alloc>
	class Value<L<CString<ChunkT,ExpT>, Alloc> >
	{
	public:
		static bool Add(L<CString<ChunkT,ExpT>, Alloc>& out, const char* c_str, const String::size_type len)
		{
			out.push_back(CString<ChunkT,ExpT>(c_str, len));
			return true;
		}
	};


	template<template<class, class> class L, class C, class Traits, class AllocS, class Alloc>
	class Value<L<std::basic_string<C, Traits, AllocS>, Alloc> >
	{
	public:
		static bool Add(L<std::basic_string<C, Traits, AllocS>, Alloc>& out, const char* c_str, const String::size_type len)
		{
			if (len)
				out.push_back(std::string(c_str, len));
			else
				out.push_back(std::string());
			return true;
		}
	};


	template<class T>
	class Flag
	{
	public:
		static void Enabled(T& out)
		{
			out = T(1);
		}
	};


	template<> class Flag<bool>
	{
	public:
		static void Enable(bool& out)
		{
			out = true;
		}
	};

	template<> class Flag<sint16>
	{
	public:
		static void Enable(sint16& out)
		{
			out = 1;
		}
	};

	template<> class Flag<sint32>
	{
	public:
		static void Enable(sint32& out)
		{
			out = 1;
		}
	};

	template<> class Flag<sint64>
	{
	public:
		static void Enable(sint64& out)
		{
			out = 1;
		}
	};

	template<> class Flag<uint16>
	{
	public:
		static void Enable(uint16& out)
		{
			out = 1u;
		}
	};

	template<> class Flag<uint32>
	{
	public:
		static void Enable(uint32& out)
		{
			out = 1u;
		}
	};

	template<> class Flag<uint64>
	{
	public:
		static void Enable(uint64& out)
		{
			out = 1u;
		}
	};






	template<uint ChunkT, bool ExpT>
	class Flag<CString<ChunkT,ExpT> >
	{
	public:
		static void Enable(CString<ChunkT,ExpT>& out)
		{
			out = "true";
		}
	};


	template<class C, class Traits, class Alloc>
	class Flag<std::basic_string<C, Traits, Alloc> >
	{
	public:
		static void Enable(std::basic_string<C,Traits,Alloc>& out)
		{
			out = "true";
		}
	};

	template<template<class,class> class L, class T, class Alloc>
	class Flag<L<T, Alloc> >
	{
	public:
		static void Enable(L<T,Alloc>& out)
		{
			out.push_back(T(1));
		}
	};


	template<template<class, class> class L, uint ChunkT, bool ExpT, class Alloc>
	class Flag<L<CString<ChunkT,ExpT>, Alloc> >
	{
	public:
		static void Enable(L<CString<ChunkT,ExpT>, Alloc>& out)
		{
			out.push_back("true");
		}
	};



	template<template<class, class> class L, class C, class Traits, class AllocS, class Alloc>
	class Flag<L<std::basic_string<C, Traits, AllocS>, Alloc> >
	{
	public:
		static void Enable(L<std::basic_string<C, Traits, AllocS>, Alloc>& out)
		{
			out.push_back("true");
		}
	};


	/*!
	** \brief Display the help for an option in particulare, with text formatting
	**
	** \param[in,out] out The stream where to write data
	** \param shortName The short name of the option
	** \param longName The long name of the option
	** \param description The description of the option
	*/
	void DisplayHelpForOption(std::ostream& out, const char shortName, const String& longName,
		const String& description, bool requireParameter = false);

	/*!
	** \brief Display a text paragraph
	**
	** \param[in,out] out The stream where to write data
	** \param text The text
	*/
	void DisplayTextParagraph(std::ostream& out, const String& text);




	class IOption
	{
	public:
		IOption() :
			pShortName('\0')
		{}
		IOption(const IOption& rhs) :
			pShortName(rhs.pShortName),
			pLongName(rhs.pLongName),
			pDescription(rhs.pDescription)
		{}

		explicit IOption(char s) :
			pShortName(s)
		{}

		IOption(char s, const AnyString& name) :
			pShortName(s),
			pLongName(name)
		{
		}

		IOption(char s, const AnyString& name, const AnyString& description) :
			pShortName(s),
			pLongName(name),
			pDescription(description)
		{
		}

		virtual ~IOption() {}

		/*!
		** \brief Add a value
		**
		** \param c_str A pointer to the beginining of the CString (must not be null)
		** \param len Length of the string (can be zero)
		** \return True if the operation succeded, false otherwise
		*/
		virtual bool addValue(const char* c_str, String::size_type len) = 0;

		virtual void enableFlag() = 0;

		virtual bool requireAdditionalParameter() const = 0;

		//! Get the short name of the option
		char shortName() const {return pShortName;}

		//! Get the long name of the option
		const String& longName() const {return pLongName;}

		//! Get the description of the option
		const String& description() const {return pDescription;}

		virtual void helpUsage(std::ostream& out) const = 0;

	protected:
		//! The short name of the option
		const char pShortName;
		//! The long name
		const String pLongName;
		//! Description
		const String pDescription;

	}; // class IOption





	/*!
	** \brief A single command line option
	*/
	template<class T, bool Visible, bool AdditionalParam = true>
	class Option final : public IOption
	{
	public:
		enum
		{
			//! True if the option is hidden from the help usage
			flagVisible = Visible,
		};

	public:
		//! \name Constructors & Destructor
		//@{
		Option(const Option& rhs) :
			IOption(rhs),
			pVariable(rhs.pVariable)
		{}

		Option(T& var, char c) :
			IOption(c),
			pVariable(var)
		{}

		Option(T& var, const AnyString& name) :
			IOption('\0', name),
			pVariable(var)
		{}

		Option(T& var, char c, const AnyString& name) :
			IOption(c, name),
			pVariable(var)
		{}

		Option(T& var, char s, const AnyString& name, const AnyString& description) :
			IOption(s, name, description),
			pVariable(var)
		{
		}

		//! Destructor
		virtual ~Option() {}
		//@}


		/*!
		** \brief Add a value
		**
		** \param c_str A pointer to the beginining of the CString (must not be null)
		** \param len Length of the string (can be zero)
		** \return True if the operation succeded, false otherwise
		*/
		virtual bool addValue(const char* c_str, String::size_type len) override
		{
			return Private::GetOptImpl::Value<T>::Add(pVariable, c_str, len);
		}

		virtual void enableFlag() override
		{
			Private::GetOptImpl::Flag<T>::Enable(pVariable);
		}

		virtual void helpUsage(std::ostream& out) const override
		{
			if (Visible)
				DisplayHelpForOption(out, pShortName, pLongName, pDescription, AdditionalParam);
		}

		virtual bool requireAdditionalParameter() const override {return AdditionalParam;}

	private:
		//! The destination variable, where to add values
		T& pVariable;

	}; // class Option



	/*!
	** \brief A text paragraph
	*/
	class Paragraph final : public IOption
	{
	public:
		//! \name Constructors & Destructor
		//@{
		Paragraph(const Paragraph& rhs) :
			IOption(rhs)
		{}

		explicit Paragraph(const AnyString& description) :
			IOption(' ', nullptr, description)
		{}

		//! Destructor
		virtual ~Paragraph() {}
		//@}


		/*!
		** \brief Add a value
		**
		** \param c_str A pointer to the beginining of the CString (must not be null)
		** \param len Length of the string (can be zero)
		** \return True if the operation succeded, false otherwise
		*/
		virtual bool addValue(const char* c_str, String::size_type len) override
		{
			// Does nothing - This is not an option
			(void) c_str;
			(void) len;
			return false;
		}

		virtual void helpUsage(std::ostream& out) const override
		{
			DisplayTextParagraph(out, pDescription);
		}

		virtual void enableFlag() override
		{
			// Does nothing
		}

		virtual bool requireAdditionalParameter() const override {return false;}

	}; // class Paragraph



	// Forward declaration
	class Context;




} // namespace GetOptImpl
} // namespace Private
} // namespace Yuni

