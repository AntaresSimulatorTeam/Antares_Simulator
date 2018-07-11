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
#include "../../yuni.h"
#include "../static/remove.h"
#include "../static/assert.h"
#include "../static/typedef.h"
#include "../traits/cstring.h"
#include "../traits/length.h"
#include "../smartptr.h"

#ifdef YUNI_OS_MSVC
#pragma warning(push)
#pragma warning(disable : 4995)
#pragma warning(push, 0)
#endif

#include <cstdio>
#ifdef YUNI_HAS_STDARG_H
# include <stdarg.h>
#endif

#include <string>
#include <functional> // std::hash


#ifdef YUNI_HAS_VECTOR
# include <vector>
#endif
#include <list>
#include <iosfwd>

#ifdef YUNI_OS_MSVC
#pragma warning(pop)
#endif

#include "utf8char.h"
#include "../iterator.h"
#include "traits/traits.h"
#include "traits/append.h"
#include "traits/assign.h"
#include "traits/fill.h"
#include "traits/vnsprintf.h"
#include "traits/into.h"

#ifdef YUNI_HAS_CPP_MOVE
# include <utility>
#endif



namespace Yuni
{

	/*!
	** \brief Character string
	** \ingroup Core
	**
	** The class manipulates and stores sequences of characters.
	**
	** The class is a template class, you may prefer the convenient alias
	** <tt>Yuni::String</tt> most of the time.
	**
	** The following external types can be inserted / converted to a <tt>Yuni::CString</tt> :
	**  - <tt>C</tt>
	**  - <tt>char*</tt>
	**  - <tt>C[]</tt>
	**  - <tt>std::basic_string<char> </tt>
	**  - <tt>Yuni::CString<...> </tt>
	**  - <tt>SmartPtr<std::basic_string<char>, ...> </tt>
	**  - <tt>SmartPtr<CString<...>, ...> </tt>
	**
	** Example for iterating through all characters (the recommended way)
	** \code
	** String t = "こんにちは";
	** const String::null_iterator end = t.utf8end();
	** for (String::const_utf8iterator i = t.utf8begin(); i != end; ++i)
	**	std::cout << "char at offset " << i.offset() << ": " << *i << std::endl;
	** \endcode
	**
	** Example for conversions :
	** \code
	** String s;
	** s << "example for double = " << 42.6;
	** std::cout << s << std::endl;
	**
	** s = "42";
	** int i = s.to<int>();
	** std::cout << "Conversion without check for int: " << i << std::endl;
	** if (s.to(i))
	** {
	**	std::cout << "Conversion with check for int: " << i << std::endl;
	** }
	** else
	**	std::cout << "Conversion failed for int " << std::endl;
	**
	** Color::RGB<> rgb(142, 230, 12);
	** s.clear() << "example for rgb = " << rgb;
	** std::cout << s << std::endl;
	**
	** Color::RGBA<> rgba;
	** s = " rgb( 42, 58, 234)";
	** s.to(rgba);
	** std::cout << "Conversion from string to rgba : " << rgba << std::endl;
	** s = " rgba( 42, 58, 234, 67)";
	** s.to(rgba);
	** std::cout << "Conversion from string to rgba : " << rgba << std::endl;
	** \endcode
	**
	** \warning This class is not thread-safe
	** \tparam ChunkSizeT The size for a single chunk (> 3)
	** \tparam ExpandableT True to make a growable string. Otherwise it will be a
	**   string with a fixed-length capacity (equals to ChunkSizeT)
	*/
	template<uint ChunkSizeT, bool ExpandableT>
	class YUNI_DECL CString final :
		protected Private::CStringImpl::Data<ChunkSizeT,ExpandableT>
	{
	public:
		//! POD type
		typedef char Char;
		//! Type for the POD type
		typedef char Type;

		//! Ancestor
		typedef Private::CStringImpl::Data<ChunkSizeT,ExpandableT>  AncestorType;
		//! Size type
		typedef typename AncestorType::Size Size;
		//! Self
		typedef CString<ChunkSizeT,ExpandableT>  CStringType;

		//! \name Compatibility with std::string
		//@{
		//! The type of object, charT, stored in the string
		typedef char value_type;
		//! Pointer to char
		typedef char* pointer;
		//! Reference to char
		typedef char& reference;
		//! Const reference to char
		typedef const char& const_reference;
		//! An unsigned integral type
		typedef Size size_type;
		//! A signed integral type
		typedef ssize_t difference_type;
		//@}

		//! Smartptr
		typedef SmartPtr<CStringType> Ptr;
		//! A String vector
		typedef std::vector<CStringType> Vector;
		//! A String vector
		typedef std::vector<Ptr> VectorPtr;
		//! A String list
		typedef std::list<CStringType> List;
		//! A string list
		typedef std::list<Ptr> ListPtr;

		enum
		{
			//! Size for a single chunk
			chunkSize      = AncestorType::chunkSize,
			//! Invalid offset
			npos           = static_cast<Size>(-1),
			//! A non-zero value if the string must be zero terminated
			zeroTerminated = 1, //AncestorType::zeroTerminated,
			//! A non-zero value if the string can be expanded
			expandable     = AncestorType::expandable,
			//! True if the string is a string adapter (only read-only operations are allowed)
			adapter        = (0 == chunkSize and expandable),
		};
		//! char Case
		enum charCase
		{
			//! The string should remain untouched
			soCaseSensitive,
			//! The string should be converted to lowercase
			soIgnoreCase
		};

		//! Self, which can be written
		typedef typename Static::If<adapter or (0 == expandable and chunkSize > 512),
			CString<>, CStringType>::RetTrue  WritableType;

		//! Operator [] return type
		typedef typename Static::If<!adapter, char&, char>::Type OperatorBracketReturnType;
		//! Operator [] return type (const)
		typedef typename Static::If<!adapter, const char&, char>::Type ConstOperatorBracketReturnType;

		// Checking for a minimal chunk size
		YUNI_STATIC_ASSERT(adapter or chunkSize > 3, CString_MinimalChunkSizeRequired);

	public:
		//! \name CString comparison
		//@{
		/*!
		** \brief Compare two strings like strcmp()
		**
		** The comparison is done using ucharacters.
		** \return An integer greater than, equal to, or less than 0, according as the string is greater than,
		**   equal to, or less than the given string
		*/
		static int Compare(const char* const s1, uint l1, const char* const s2, uint l2);

		/*!
		** \brief Compare two strings like stricmp() (case-insensitive)
		**
		** The comparison is done using ucharacters.
		** \return An integer greater than, equal to, or less than 0, according as the string is greater than,
		**   equal to, or less than the given string
		*/
		static int CompareInsensitive(const char* const s1, uint l1, const char* const s2, uint l2);
		//@}


		//! \name Faster implementation of some common routines
		//@{
		//! Upper case to lower case letter conversion (man 3 tolower)
		static int ToLower(int c);
		//! Lower case to upper case letter conversion (man 3 toupper)
		static int ToUpper(int c);
		//! White-space character test (' \t\r\n...')
		static bool IsSpace(int c);
		//! decimal-digit character test (0-9)
		static bool IsDigit(int c);
		//! decimal-digit character test (without zero - 1-9)
		static bool IsDigitNonZero(int c);
		//! alphabetic character test (a-zA-Z)
		static bool IsAlpha(int c);
		//@}



	private:
		// Implements the following iterator models for String
		#include "iterator.inc.hpp"

	public:
		//! \name Iterators
		//@{
		//! Iterator
		typedef IIterator<typename Model::ByteIterator, false>  iterator;
		//! Iterator (const)
		typedef IIterator<typename Model::ByteIterator, true>   const_iterator;
		//! Iterator for UTF8 characters
		typedef IIterator<typename Model::UTF8Iterator, false>  utf8iterator;
		//! Iterator for UTF8 characters (const)
		typedef IIterator<typename Model::UTF8Iterator, true>   const_utf8iterator;
		//! Null iterator
		typedef IIterator<typename Model::NullIterator, true>   null_iterator;
		//@}

	public:
		//! \name Constructors & Destructor
		//@{
		//! Default Constructor
		CString();

		//! Copy constructor
		CString(const CString& rhs);

		//! Constructor with a null-terminated string
		CString(const char* const text);

		//! Constructor from a mere CString
		CString(const char* const block, Size blockSize);

		//! Constructor with a null-terminated wide string
		CString(const wchar_t* text);

		//! Constructor from a wide string
		CString(const wchar_t* block, Size blockSize);

		/*!
		** \brief Constructor from another string
		*/
		template<uint SizeT, bool ExpT>
		CString(const CString<SizeT,ExpT>& string);

		# ifdef YUNI_HAS_CPP_MOVE
		//! Move constructor
		CString(CString&& rhs);
		# endif

		/*!
		** \brief Constructor from a copy of a substring of 's'
		**
		** The substring is the portion of str that begins at the character position
		** 'offset'.
		*/
		template<uint SizeT, bool ExpT>
		CString(const CString<SizeT,ExpT>& s, Size offset);

		/*!
		** \brief Constructor from a copy of a substring of 's'
		**
		** The substring is the portion of str that begins at the character position
		** 'offset' and takes up to 'n' characters (it takes less than n if the end
		** of 's' is reached before).
		*/
		template<uint SizeT, bool ExpT>
		CString(const CString<SizeT,ExpT>& s, Size offset, Size n /*= npos*/);

		//! Constructor from a std::string
		template<class TraitsT, class AllocT>
		CString(const std::basic_string<char,TraitsT,AllocT>& string);

		/*!
		** \brief Constructor from a copy of a substring of 's' (std::string)
		**
		** The substring is the portion of str that begins at the character position
		** 'offset'.
		*/
		template<class TraitsT, class AllocT>
		CString(const std::basic_string<char,TraitsT,AllocT>& s, Size offset);

		/*!
		** \brief Constructor from a copy of a substring of 's' (std::string)
		**
		** The substring is the portion of str that begins at the character position
		** 'offset' and takes up to 'n' characters (it takes less than n if the end
		** of 's' is reached before).
		*/
		template<class TraitsT, class AllocT>
		CString(const std::basic_string<char,TraitsT,AllocT>& s, Size offset, Size n /*= npos*/);

		/*!
		** \brief Constructor by copy from iterator
		**
		** \param begin An iterator pointing to the beginning of a sequence
		** \param end  An iterator pointing to the end of a sequence
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2>
		CString(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end);

		/*!
		** \brief Constructor by copy from iterator
		**
		** \param begin An iterator pointing to the beginning of a sequence
		** \param end  An iterator pointing to the end of a sequence
		** \param separator A string to add between each item
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2, class StringT>
		CString(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end, const StringT& separator);

		//! Construct a string formed by a repetition of the character c, n times
		CString(size_t n, char c);

		//! Construct a string formed by a repetition of the character c, n times
		CString(size_t n, uchar c);

		//! Constructor from a nullptr
		CString(const YuniNullPtr&);

		//! Constructor with a default boolean
		CString(bool value);
		//! Constructor with a int32
		CString(yint32 value);
		//! Constructor with a int64
		CString(yint64 value);
		//! Constructor with a uint32
		CString(yuint32 value);
		//! Constructor with a uint64
		CString(yuint64 value);
		//! Constructor with a float
		CString(float value);
		//! Constructor with a double
		CString(double value);

		//! Constructor with a default value
		// \internal The keyword 'explicit' is mandatory here to avoid invalid and unwanted conversion
		//  to string when used in overloaded methods
		template<class U> explicit CString(const U& rhs);

		//! Destructor
		~CString() {}
		//@}


		//! \name Iterators
		//@{
		//! Get an iterator on UTF8 characters pointing to the beginning of the string
		utf8iterator utf8begin(uint offset = 0);
		//! Get an iterator on UTF8 characters pointing to the beginning of the string
		const_utf8iterator utf8begin(uint offset = 0) const;

		//! Get an iterator on UTF8 characters pointing to the end of the string
		null_iterator utf8end();
		//! Get an iterator on UTF8 characters pointing to the end of the string
		null_iterator utf8end() const;

		//! Get an iterator pointing to the beginning of the string
		iterator begin();
		//! Get an iterator pointing to the beginning of the string
		const_iterator begin() const;

		//! Get an iterator pointing to the end of the string
		null_iterator end();
		//! Get an iterator pointing to the end of the string
		null_iterator end() const;
		//@}


		//! \name Append / Assign / Fill
		//@{
		/*!
		** \brief Assign a new value to the string
		**
		** \param rhs Any supported value
		*/
		template<class U> void assign(const U& rhs);

		/*!
		** \brief Copy a raw C-String
		**
		** \param str A C-String
		** \param size Size of the given string
		*/
		template<class StringT>
		void assign(const StringT& str, Size size);

		/*!
		** \brief Copy a raw C-String
		**
		** \param str A C-String
		** \param size Size of the given string
		** \param offset Offset of the first character to copy
		*/
		template<class StringT>
		void assign(const StringT& str, Size size, Size offset);

		/*!
		** \brief Assign to the string all items within
		**
		** The type held by the iterator can be anything as long as the type can
		** be converted by the string (see specializations in the namespace
		** `Yuni::Extension::CString`).
		**
		** \param begin An iterator pointing to the beginning of a sequence
		** \param end  An iterator pointing to the end of a sequence
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2>
		void assign(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end);

		/*!
		** \brief Assign to the string all items within
		**
		** The type held by the iterator can be anything as long as the type can
		** be converted by the string (see specializations in the namespace
		** `Yuni::Extension::CString`).
		**
		** \code
		** String s = "string: こんにちは";
		** String::const_utf8iterator a = s.utf8begin() + 9;
		** String::const_utf8iterator a = s.utf8begin() + 11;
		** String sub1(a, b);
		** std::cout << sub1 << std::endl; // んに
		**
		** String sub2;
		** sub2.append(a, b, ", ");
		** std::cout << sub2 << std::endl; // ん, に
		** \endcode
		**
		** \param begin An iterator pointing to the beginning of a sequence
		** \param end  An iterator pointing to the end of a sequence
		** \param separator The string separator to use between each item
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2, class StringT>
		void assign(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end, const StringT& separator);

		/*!
		** \brief Assign to thestring all items within
		**
		** The type held by the iterator can be anything as long as the type can
		** be converted by the string (see specializations in the namespace
		** `Yuni::Extension::CString`).
		**
		** \code
		** String s = "string: こんにちは";
		** String::const_utf8iterator a = s.utf8begin() + 9;
		** String::const_utf8iterator a = s.utf8begin() + 11;
		** String sub1(a, b);
		** std::cout << sub1 << std::endl; // んに
		**
		** String sub2;
		** sub2.append(a, b, ", ");
		** std::cout << sub2 << std::endl; // ん, に
		** \endcode
		**
		** \param begin An iterator pointing to the beginning of a sequence
		** \param end  An iterator pointing to the end of a sequence
		** \param separator The string separator to use between each item
		** \param enclosure The enclosure string
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2, class StringT, class EnclosureT>
		void assign(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end,
			const StringT& separator, const EnclosureT& enclosure);


		/*!
		** \brief Append to the end of the string a new value
		**
		** \param rhs Any supported value
		*/
		template<class U> void append(const U& rhs);

		/*!
		** \brief Append to the end of the string all items within
		**
		** The type held by the iterator can be anything as long as it can
		** be converted by the string (see specializations in the namespace
		** Yuni::Extension::CString).
		**
		** \see namespace Yuni::Extension::CString
		** \param begin  An iterator pointing to the beginning of a sequence
		** \param end    An iterator pointing to the end of a sequence
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2>
		void append(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end);

		/*!
		** \brief Append to the end of the string all items within
		**
		** The type held by the iterator can be anything as long as it can
		** be converted by the string (see specializations in the namespace
		** Yuni::Extension::CString).
		**
		** \code
		** String s = "string: こんにちは";
		** String::const_utf8iterator a = s.utf8begin() + 9;
		** String::const_utf8iterator a = s.utf8begin() + 11;
		** String sub1(a, b);
		** std::cout << sub1 << std::endl; // んに
		**
		** String sub2;
		** sub2.append(a, b, ", ");
		** std::cout << sub2 << std::endl; // ん, に
		** \endcode
		**
		** \see namespace Yuni::Extension::CString
		**
		** \param begin     An iterator pointing to the beginning of a sequence
		** \param end       An iterator pointing to the end of a sequence
		** \param separator The string separator to use between each item
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2, class StringT>
		void append(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end,
			const StringT& separator);

		/*!
		** \brief Append to the end of the string all items within a range
		**
		** The type held by the iterator can be anything as long as it can
		** be converted by the string (see specializations in the namespace
		** Yuni::Extension::CString).
		**
		** \code
		** String s = "string: こんにちは";
		** String::const_utf8iterator a = s.utf8begin() + 9;
		** String::const_utf8iterator a = s.utf8begin() + 11;
		** String sub1(a, b);
		** std::cout << sub1 << std::endl; // んに
		**
		** String sub2;
		** sub2.append(a, b, ", ", '"');
		** std::cout << sub2 << std::endl; // "ん", "に"
		** \endcode
		**
		** \see namespace Yuni::Extension::CString
		**
		** \param begin     An iterator pointing to the beginning of a sequence
		** \param end       An iterator pointing to the end of a sequence
		** \param separator The string separator to use between each item
		** \param enclosure The enclosure string
		*/
		template<class ModelT, bool ConstT, class ModelT2, bool ConstT2, class StringT, class EnclosureT>
		void append(const IIterator<ModelT,ConstT>& begin, const IIterator<ModelT2,ConstT2>& end,
			const StringT& separator, const EnclosureT& enclosure);

		/*!
		** \brief Append to the end of the string a new value
		**
		** \param s  Any supported value
		** \param size Size of the container
		*/
		template<class StringT> void append(const StringT& s, Size size);

		/*!
		** \brief Append to the end of the string a wchar_t buffer
		**
		** \note The buffer is always considered as UTF-16 on Windows
		** \param wbuffer A wchar_t buffer (can be null)
		** \param wsize Size of the container
		** \return True if the convertion succeeded and if something has been added
		*/
		bool append(const wchar_t* wbuffer, Size wsize);

		/*!
		** \brief Append to the end of the string a new value
		**
		** \param s      Any supported value
		** \param size   Size of the container
		** \param offset Offset of the first character to append
		*/
		template<class StringT>
		void append(const StringT& s, Size size, Size offset);

		/*!
		** \brief Append to the end of the string a new value
		**
		** \note The buffer is always considered as UTF-16 on Windows
		** \param wbuffer A wchar_t buffer (can be null)
		** \param wsize   Size of the container
		** \param offset Offset of the first character to append
		** \return True if the convertion succeeded and if something has been added
		*/
		bool append(const wchar_t* wbuffer, Size wsize, Size offset);



		// Equivalent to append, provided for compatibility issues with other
		// Yuni containers
		//! \see template<class U> append(const U&, const Size)
		template<class U> void write(const U& cstr);
		//! \see template<class U> append(const U&, const Size)
		template<class U> void write(const U& cstr, Size size);

		/*!
		** \brief Append a single signed char
		*/
		void put(char c);
		/*!
		** \brief Append a single uchar
		*/
		void put(const uchar c);
		// equivalent to append, provided for compatibility with other containers
		template<class U> void put(const U& rhs);

		/*!
		** \brief Insert a raw C-String at a given position in the string
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \param cstr A C-String
		** \param size Size of the string
		** \return True if the given string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		bool insert(Size offset, const char* const cstr, Size size);

		/*!
		** \brief Insert a single item at a given position in the string
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \param c A single item
		** \return True if the string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		bool insert(Size offset, char c);

		/*!
		** \brief Insert an arbitrary C-String at a given position in the string
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \param string Any string
		** \return True if the string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		bool insert(Size offset, const AnyString& string);

		/*!
		** \brief Insert an arbitrary C-String at a given position in the string
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \param string Any string
		** \param size The size to use for the given container
		** \return True if the string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		template<class StringT> bool insert(Size offset, const StringT& string, Size size);

		/*!
		** \brief Insert any arbitrary string at a given offset provided by an iterator
		**
		** If the offset is greater than the size of the string, the value
		** will be merely appended to the string.
		*/
		template<class ModelT, bool ConstT>
		void insert(const IIterator<ModelT,ConstT>& it, const AnyString& string);

		/*!
		** \brief Insert a raw C-String at the beginning of in the string
		**
		** \param cstr A C-String
		** \param size Size of the string
		** \return True if the given string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		bool prepend(const char* const cstr, Size size);

		/*!
		** \brief Insert a single item at the beginning of the string
		**
		** \param c A single item
		** \return True if the string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		bool prepend(char c);

		/*!
		** \brief Insert an arbitrary C-String at the beginning of the string
		**
		** \param string Any CString container
		** \return True if the string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		bool prepend(const AnyString& string);

		/*!
		** \brief Insert an arbitrary C-String at the beginning the string
		**
		** \param string Any CString container
		** \param size The size to use for the given container
		** \return True if the string has been inserted, false otherwise
		**   (size == 0 or offset out of bounds)
		*/
		template<class StringT> bool prepend(const StringT& string, Size size);

		/*!
		** \brief Overwrite a region of the string
		**
		** The size of the string will remain untouched in any cases.
		**
		** \param offset Position of the first character of the region in the string
		** \param string A CString
		*/
		void overwrite(Size offset, const AnyString& string);

		/*!
		** \brief Overwrite a region of the string
		**
		** The size of the string will remain untouched in any cases.
		**
		** \param string A CString
		*/
		void overwrite(const AnyString& string);

		/*!
		** \brief Overwrite a region of the string
		**
		** The size of the string will remain untouched in any cases.
		**
		** \param offset Position of the first character of the region in the string
		** \param cstr A C-String
		** \param size Size of 'cstr'
		*/
		void overwrite(Size offset, const char* const cstr, Size size);

		/*!
		** \brief Overwrite a region of the string from the right of the string
		**
		** The size of the string will remain untouched in any cases.
		**
		** \param s A CString container
		*/
		template<class StringT> void overwriteRight(const StringT& s);

		/*!
		** \brief Overwrite a region of the string from the right of the string
		**
		** The size of the string will remain untouched in any cases.
		**
		** \param offset Position of the first character (from the right) of the region
		**   in the string
		** \param s A CString container
		*/
		template<class StringT> void overwriteRight(Size offset, const StringT& s);


		/*!
		** \brief Overwrite a region of the string from the center of the string
		**
		** The size of the string will remain untouched in any cases.
		** \code
		** String s;
		** s.resize(13, '-');
		** s.overwriteCenter(" Title ");
		** std::cout << s << std::endl; // '--- Title ---'
		** \endcode
		**
		** \param s A CString container
		*/
		template<class StringT> void overwriteCenter(const StringT& s);


		/*!
		** \brief Fill the entire string with a given pattern
		**
		** \code
		** String s = "some text here";
		** s.fill('.');
		** std::cout << s << std::endl; // '..............'
		**
		** s.resize(20);
		** s.fill('.');
		** s.overwrite(0,  "Chapter 1 ");
		** s.overwriteRight(" 4");
		** std::cout << s << std::endl; // 'Chapter 1  ....... 4'
		** \endcode
		**
		** \param pattern The pattern
		*/
		template<class StringT> void fill(const StringT& pattern);

		/*!
		** \brief Fill the entire string with a given pattern from a given offset
		**
		** \param offset Position of the first character where to start from
		** \param pattern The pattern
		*/
		template<class StringT> void fill(Size offset, const StringT& pattern);

		//! Equivalent to append()
		template<class U> void push_back(const U& u);

		//! Equivalent to prepend()
		template<class U> void push_front(const U& u);
		//@}


		//! \name Search / Replace
		//@{
		/*!
		** \brief Find the offset of a sub-string
		**
		** \param c ASCII character
		** \return True if sub-string is found, false otherwise
		*/
		bool contains(char c) const;

		/*!
		** \brief Find the offset of a sub-string (ignoring the case)
		**
		** \param c ASCII character
		** \return True if sub-string is found, false otherwise
		*/
		bool icontains(char c) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes)
		**
		** \param cstr An arbitrary string
		** \param len Size of the given cstr
		** \return True if sub-string is found, false otherwise
		*/
		bool contains(const char* const cstr, Size len) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes) (ignoring the case)
		**
		** \param cstr An arbitrary string
		** \param len Size of the given cstr
		** \return True if sub-string is found, false otherwise
		*/
		bool icontains(const char* const cstr, Size len) const;

		/*!
		** \brief Find the offset of any supported CString
		**
		** \param string Any supported CString
		** \return True if sub-string is found, false otherwise
		*/
		bool contains(const AnyString& string) const;

		/*!
		** \brief Find the offset of any supported CString (ignoring the case)
		**
		** \param string Any supported CString
		** \return True if sub-string is found, false otherwise
		*/
		bool icontains(const AnyString& string) const;


		/*!
		** \brief Find the offset of a sub-string
		**
		** \param c ASCII character
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size find(char c) const;

		/*!
		** \brief Find the offset of a sub-string
		**
		** \param c ASCII character
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size find(char c, Size offset) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes)
		**
		** \param cstr An arbitrary string
		** \param len Size of the given cstr
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size find(const char* const cstr, Size offset, Size len) const;

		/*!
		** \brief Find the offset of any supported CString
		**
		** \param s Any supported CString
		** \return The position of the first occurence found, `npos` if not found
		*/
		template<class StringT> Size find(const StringT& s, Size offset = 0) const;


		/*!
		** \brief Find the offset of a sub-string (ignoring the case)
		**
		** \param c A character
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size ifind(char c) const;

		/*!
		** \brief Find the offset of a sub-string (ignoring the case)
		**
		** \param c ASCII character
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size ifind(char c, Size offset) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes) (ignoring the case)
		**
		** \param cstr An arbitrary string
		** \param offset Start offset
		** \param len Size of the given cstr
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size ifind(const char* const cstr, Size offset, Size len) const;

		/*!
		** \brief Find the offset of any supported CString (ignoring the case)
		**
		** \param s Any supported CString
		** \return The position of the first occurence found, `npos` if not found
		*/
		template<class StringT> Size ifind(const StringT& s, Size offset = 0) const;


		/*!
		** \brief Find the offset of a sub-string
		**
		** \param c A character
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size rfind(char c) const;

		/*!
		** \brief Find the offset of a sub-string
		**
		** \param c A character
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size rfind(char c, Size offset) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes)
		**
		** \param cstr An arbitrary string
		** \param offset The position in the string where to start from
		** \param len Size of the given cstr
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size rfind(const char* const cstr, Size offset, Size len) const;

		/*!
		** \brief Find the offset of any supported CString
		**
		** \param s Any supported CString
		** \param offset The position in the string where to start from
		** \return The position of the first occurence found, `npos` if not found
		*/
		template<class StringT> Size rfind(const StringT& s, Size offset = npos) const;


		/*!
		** \brief Find the offset of a sub-string (ignoring the case)
		**
		** \param c Ascii to find
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size irfind(char c) const;

		/*!
		** \brief Find the offset of a sub-string (ignoring the case)
		**
		** \param c Ascii to find
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size irfind(char c, Size offset) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes) (ignoring the case)
		**
		** \param cstr An arbitrary string
		** \param len Size of the given cstr
		** \return The position of the first occurence found, `npos` if not found
		*/
		Size irfind(const char* const cstr, Size offset, Size len) const;

		/*!
		** \brief Find the offset of any supported CString (ignoring the case)
		**
		** \param s Any supported CString
		** \param offset  Start Offset
		** \return The position of the first occurence found, `npos` if not found
		*/
		template<class StringT> Size irfind(const StringT& s, Size offset = npos) const;

		/*!
		** \brief Get if the string contains at least one occurence of a given char
		*/
		bool hasChar(char c) const;

		/*!
		** \brief Get if the string contains at least one occurence of a given uchar
		*/
		bool hasChar(uchar c) const;

		/*!
		** \brief Get the number of occurrences of a single char
		*/
		uint countChar(char c) const;

		/*!
		** \brief Find the offset of a sub-string from the left
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \param cstr An arbitrary string character
		** \return The position of the first occurence found, `npos` if not found
		*/
		uint indexOf(Size offset, char cstr) const;

		/*!
		** \brief Find the offset of a raw sub-string with a given length (in bytes) from the left
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \param cstr An arbitrary C-string
		** \param len Size of the given string
		** \return The position of the first occurence found, `npos` if not found
		*/
		uint indexOf(Size offset, const char* const cstr, Size len) const;

		/*!
		** \brief Find the offset of any supported CString from the left
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered
		** \return The position of the first occurence found, `npos` if not found
		*/
		template<class StringT> uint indexOf(Size offset, const StringT& s) const;

		/*!
		** \brief Searches the string for an individual character
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_first_of(char c, Size offset = 0) const;

		/*!
		** \brief Searches the string for an individual character (case insensitive)
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size ifind_first_of(char c, Size offset = 0) const;

		/*!
		** \brief Searches the string for any of the characters that are part of `seq`
		**
		** \param sequence List of ASCII characters
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_first_of(const AnyString& sequence, Size offset = 0) const;

		/*!
		** \brief Searches the string for any of the characters that are part of `seq`
		**   (ignoring the case)
		**
		** \param sequence List of ASCII characters
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size ifind_first_of(const AnyString& sequence, Size offset = 0) const;

		/*!
		** \brief Searches the string for the first character that is not `c`
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_first_not_of(char c, Size offset = 0) const;

		/*!
		** \brief Searches the string for the first character that is not `c` (case insensitive)
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size ifind_first_not_of(char c, Size offset = 0) const;

		/*!
		** \brief Searches the string for any of the characters that are not part of `seq`
		**
		** \param sequence List of ASCII characters
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_first_not_of(const AnyString& sequence, Size offset = 0) const;

		/*!
		** \brief Searches the string for any of the characters that are not part of `seq` (case insensitive)
		**
		** \param sequence List of ASCII characters
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size ifind_first_not_of(const AnyString& sequence, Size offset = 0) const;



		/*!
		** \brief Searches the string from the end for an individual character
		**
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_last_of(char c) const;

		/*!
		** \brief Searches the string from the end for an individual character (ignoring the case)
		**
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size ifind_last_of(char c) const;

		/*!
		** \brief Searches the string from the end for an individual character
		**
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_last_of(char c, Size offset) const;

		/*!
		** \brief Searches the string from the end for any of the characters that are part of `seq`
		**
		** \param sequence List of ASCII characters
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size find_last_of(const AnyString& sequence, Size offset = npos) const;

		/*!
		** \brief Searches the string from the end for any of the characters that are part of `seq`
		**   (ignoring the case)
		**
		** \param sequence List of ASCII characters
		** \param offset Position of the first character in the string to be taken
		**   into consideration for possible matches. A value of 0 means that the
		**   entire string is considered.
		** \return The position of the first occurrence in the string (zero-based)
		**   npos if not found
		*/
		Size ifind_last_of(const AnyString& sequence, Size offset = npos) const;

		/*!
		** \brief Get if a given string can be found at the beginning
		*/
		bool startsWith(const char* const cstr, Size len) const;

		/*!
		** \brief Get if a given string can be found at the beginning
		*/
		bool startsWith(const AnyString& string) const;

		/*!
		** \brief Get if a given char can be found at the beginning
		*/
		bool startsWith(char c) const;

		/*!
		** \brief Get if a given string can be found at the beginning (case insensitive)
		*/
		bool istartsWith(const char* const cstr, Size len) const;

		/*!
		** \brief Get if a given string can be found at the beginning (case insensitive)
		*/
		bool istartsWith(const AnyString& string) const;

		/*!
		** \brief Get if a given char can be found at the beginning (case insensitive)
		*/
		bool istartsWith(char c) const;

		/*!
		** \brief Get if a given string can be found at the end
		*/
		bool endsWith(const char* const cstr, Size len) const;

		/*!
		** \brief Get if a given string can be found at the end
		** \param string Any string
		*/
		bool endsWith(const AnyString& string) const;

		/*!
		** \brief Get if a given string can be found at the end
		** \param c Any char
		*/
		bool endsWith(char c) const;

		/*!
		** \brief Get if a given string can be found at the end (case insensitive)
		*/
		bool iendsWith(const char* const cstr, Size len) const;

		/*!
		** \brief Get if a given string can be found at the end (case insensitive)
		** \param string Any string
		*/
		bool iendsWith(const AnyString& string) const;

		/*!
		** \brief Get if a given string can be found at the end (case insensitive)
		** \param c Any char
		*/
		bool iendsWith(char c) const;
		//@}


		//! \brief Erase
		//@{
		/*!
		** \brief Remove up to `n` characters from the end of the string
		*/
		void chop(uint n);

		/*!
		** \brief Remove the last char
		*/
		void removeLast();

		/*!
		** \brief Remove the trailing slash or backslash at the end of the string (if any)
		**
		** Pseudo-code:
		** \code
		** Yuni::String s("/some/path/");
		** if ('\\' == s.last() or '/' == s.last())
		**	s.removeLast();
		** std::cout << s << std::endl;  // -> /some/path
		** \endcode
		*/
		void removeTrailingSlash();
		//@}


		//! \name Trimming
		//@{
		/*!
		** \brief Remove all white-spaces (" \t\r\n") from the beginning and the end
		**   of the string
		*/
		void trim();
		/*!
		** \brief Remove all white-spaces from the beginning and the end of the string
		*/
		void trim(char c);
		/*!
		** \brief Removes all items equal to one of those in 'whitespaces' from the
		**   end of the string
		*/
		void trim(const AnyString& whitespaces);

		/*!
		** \brief Removes all items equal to a space from the end of the string
		*/
		void trimRight();
		/*!
		** \brief Removes all items equal to one of those in 'u' from the end of the string
		*/
		void trimRight(const AnyString& whitespaces);
		/*!
		** \brief Remove all items equal to 'c' from the end of the string
		*/
		void trimRight(char c);

		/*!
		** \brief Removes all items equal to a space from the beginning of the string
		*/
		void trimLeft();
		/*!
		** \brief Removes all items equal to one of those in 'u' from the beginning
		**   of the string
		*/
		void trimLeft(const AnyString& whitespaces);
		/*!
		** \brief Remove all items equal to 'c' from the beginning of the string
		*/
		void trimLeft(char c);
		//@}


		//! \name Remove / Erase
		//@{
		/*!
		** \brief Empty the string
		**
		** The size will be reset to 0 but the internal data will not be freed.
		** If you want to reduce the memory used by the string, use 'shrink()'
		** instead.
		** \see shrink()
		**
		** \return Always *this
		*/
		CString& clear();

		/*!
		** \brief Erase a part of the string
		**
		** \param offset The offset (zero-based) of the first item to erase
		** \param len The length (in number of items) to erase
		*/
		void erase(Size offset, Size len);

		/*!
		** \brief Erase a part of the string
		**
		** \param offset The offset (zero-based) of the first item to erase
		** \param len The length (in number of items) to erase
		*/
		template<class ModelT, bool ConstT>
		void erase(const IIterator<ModelT,ConstT>& offset, Size len);

		/*!
		** \brief Replace all occurrences of a string by another one
		**
		** \param from The string to find
		** \param to   The string to replace with
		** \return The number  the number of replacements performed
		*/
		uint replace(const AnyString& from, const AnyString& to);

		/*!
		** \brief Replace all occurrences of a string by another one
		**
		** \param offset The offset where to start from
		** \param from The string to find
		** \param to   The string to replace with
		** \return The number  the number of replacements performed
		*/
		uint replace(Size offset, const AnyString& from, const AnyString& to);

		/*!
		** \brief Replace all occurrences of a given char by another one
		**
		** \param from The character to search
		** \param to   The replacement
		** \return The number  the number of replacements performed
		*/
		uint replace(char from, char to);

		/*!
		** \brief Replace all occurrences of a given char by another one
		**
		** \param offset The offset where to start from
		** \param from The character to search
		** \param to   The replacement
		** \return The number  the number of replacements performed
		*/
		uint replace(Size offset, char from, char to);

		/*!
		** \brief Replace all occurrences of a given char by another one (case insensitive)
		**
		** \param from The character to search
		** \param to   The replacement
		** \return The number  the number of replacements performed
		*/
		uint ireplace(char from, char to);

		/*!
		** \brief Replace all occurrences of a given char by another one (case insensitive)
		**
		** \param offset The offset where to start from
		** \param from The character to search
		** \param to   The replacement
		** \return The number  the number of replacements performed
		*/
		uint ireplace(Size offset, char from, char to);

		/*!
		** \brief Replace all occurrences of a string by another one (case insensitive)
		**
		** \param from The string to find
		** \param to   The string to replace with
		** \return The number  the number of replacements performed
		*/
		uint ireplace(const AnyString& from, const AnyString& to);

		/*!
		** \brief Replace all occurrences of a string by another one (case insensitive)
		**
		** \param offset The offset where to start from
		** \param from The string to find
		** \param to   The string to replace with
		** \return The number  the number of replacements performed
		*/
		uint ireplace(Size offset, const AnyString& from, const AnyString& to);

		/*!
		** \brief Remove the 'n' first characters
		*/
		void consume(Size n);
		//@}


		//! \name Case conversion
		//@{
		/*!
		** \brief Convert the case (lower case) of characters in the string (O(N))
		*/
		CString& toLower();
		/*!
		** \brief Convert the case (upper case) of characters in the string (O(N))
		*/
		CString& toUpper();
		//@}


		//! \name Comparisons
		//@{
		/*!
		** \brief Get if the string is equals to another one
		**
		** This method is equivalent to the operator '=='
		*/
		bool equals(const AnyString& rhs) const;

		/*!
		** \brief Get if the string is equals to another one (ignoring case)
		**
		** This method is equivalent to the operator '=='
		*/
		bool equalsInsensitive(const AnyString& rhs) const;

		/*!
		** \brief Compare the string with another one
		**
		** The comparison is done using ucharacters.
		** \return An integer greater than, equal to, or less than 0, according as the string is greater than,
		**   equal to, or less than the given string
		*/
		int compare(const AnyString& rhs) const;

		/*!
		** \brief Compare the string with another one (ignoring the case)
		**
		** The comparison is done using ucharacters.
		** \return An integer greater than, equal to, or less than 0, according as the string is greater than,
		**   equal to, or less than the given string
		*/
		int compareInsensitive(const AnyString& rhs) const;
		//@}


		//! \name Conversions
		//@{
		/*!
		** \brief Convert the string into something else
		**
		** The supported types (by default) are :
		** - std::string
		** - const char* (equivalent to `c_str()`)
		** - numeric (int, long, uint, double...)
		** - bool
		*/
		template<class U> U to() const;

		/*!
		** \brief Convert the string into something else
		**
		** This method is strictly equivalent to `to()`, except
		** that we know if the conversion succeeded or not.
		**
		** \param[out] out The variable where to store the result of the conversion
		** \return True if the conversion succeeded. False otherwise
		*/
		template<class U> bool to(U& out) const;
		//@}


		//! \name Iterating through the string
		//@{
		/*!
		** \brief Get the next UTF-8 character
		**
		** \code
		** String t = "An UTF8 string : こんにちは !";
		** std::cout << "string            : " << t             << "\n";
		** std::cout << "valid             : " << t.utf8valid() << "\n";
		** std::cout << "raw size          : " << t.size()      << "\n";
		** std::cout << "nb of characters  : " << t.utf8size()  << "\n";
		**
		** // Iterating through the string
		** String::Size offset = 0;
		** UTF8::Char c;
		** std::cout << "All chars: ";
		** do
		** {
		**	if (offset)
		**		std::cout << ", ";
		**	if (UTF8::errNone != t.utf8next<false>(offset, c))
		**	{
		**		std::cout << "<EOF>\n";
		**		break;
		**	}
		**
		**	std::cout << c;
		** }
		** while (true);
		** \endcode
		**
		** \tparam InvalidateOffsetIfErrorT True to automatically set the \p offset
		**   parameter to `(Size)-1` when the result is not `errNone`
		** \param[out] offset Offset in the string
		** \param[out] out    The UTF-8 char
		** \return True if an UTF8 char has been found, false otherwise (\p offset may become invalid)
		*/
		template<bool InvalidateOffsetIfErrorT>
		UTF8::Error utf8next(Size& offset, UTF8::Char& out) const;
		//@}


		//! \name Memory management
		//@{
		/*!
		** \brief Get the item at a given position in a safe way
		**
		** Contrary to the operator [], it is safe to use an invalid offset
		** \return The item at position \p offset, a default value if the offset is out of bound
		*/
		int at(Size offset) const;

		/*!
		** \brief Truncate the string to the given length
		**
		** Nothing will be done if the new size if greater than the current one.
		** \param newSize The new size (in bytes)
		*/
		void truncate(Size newSize);

		/*!
		** \brief Ensure that there is enough allocated space for X characters
		**
		** The content will remain untouched.
		** \param minCapacity The minimum capacity of the string (in bytes)
		*/
		void reserve(Size minCapacity);

		/*!
		** \brief Resize the string to 'len' bytes
		**
		** The content will remain untouched.
		** \param len The new length (in bytes) of the string
		*/
		void resize(Size len);

		/*!
		** \brief Resize the string to 'len' bytes and fill the new content (if any)
		**
		** The new content (if any) will be filled with 'pattern'.
		** If the string can not be expanded, the new size will not be greater than the capacity.
		**
		** \code
		** String s;
		** s.resize(6, '.');
		** std::cout << s << std::endl; // '......'
		** s.resize(4, "useless pattern");
		** std::cout << s << std::endl; // '....'
		** s.resize(8, "-a");
		** std::cout << s << std::endl; // '....-a-a'
		**
		** s = "s: ";
		** s.resize(14, "\\/");
		** std::cout << s << std::endl; // 's: \/\/\/\/\/ ', note the space at the end
		** \endcode
		**
		** \param len The new length (in bytes) of the string
		** \param pattern The pattern to use to fill the new content
		** \see fill()
		*/
		void resize(Size len, const AnyString& pattern);

		/*!
		** \brief Releases any memory not required to store the character data
		**
		** If the string is empty, the internal buffer will be freed. Otherwise
		** the buffer will be reallocated to reduce as much as possible the amount
		** of memory used by the string.
		** It does not modify the size of the string, only its capacity.
		** This method has no effect if the string is not expandable.
		*/
		void shrink();

		/*!
		** \brief Perform a full check about UTF8 validity
		**
		** This check will iterate through the whole string to
		** detect any bad-formed UTF8 character.
		*/
		bool utf8valid() const;

		/*!
		** \brief Perform a full check about UTF8 validity
		**
		** This check will iterate through the whole string to
		** detect any bad-formed UTF8 character.
		** \param[out] offset The offset in the string of the misformed UTF8 character
		** \return UTF8::errNone if the string is valid
		*/
		UTF8::Error utf8valid(Size& offset) const;


		/*!
		** \brief Perform a fast check about UTF8 validity
		**
		** Contrary to `utf8valid()`, this check is only based on the first
		** code point of an UTF8 sequence. Consequently, it does not perform
		** a full compliance test and you should prefer `utf8valid()`.
		** As a consequence it may report that the string is a valid UTF8 string
		** even if it is not the case, but it may be good enough in some cases.
		** \see utf8valid()
		*/
		bool utf8validFast() const;

		/*!
		** \brief Compute the number of UTF-8 characters
		**
		** \code
		** String s = "こんにちは";
		** std::cout << "Size in bytes:    " << s.size() << std::endl; // 15
		** std::cout << "Nb of UTF8 chars: " << s.utf8size() << std::endl; // 5
		** \endcode
		** The returned value is computed at each call to this routine
		** \return The number of UTF8 character ( <= size )
		*/
		Size utf8size() const;

		/*!
		** \brief Get the current size of the string (in bytes)
		**
		** The returned value is less than or equal to the capacity, and
		** greater or equal to the number of UTF8 characters in the string.
		*/
		Size size() const;
		//! \see size()
		Size length() const;

		//! \see size()
		size_t sizeInBytes() const;

		//! the maximum number of characters that the string object can hold (for STL compliance)
		size_t max_size() const;

		/*!
		** \brief Get if the cstr is empty
		**
		** \code
		** String s;
		** s.empty();          // returns true
		** s.null();           // returns true
		**
		** s = "hello world";  // returns false
		** s.empty();          // returns false
		** s.null();           // returns false
		**
		** s.clear();
		** s.empty();          // returns true
		** s.null();           // returns false
		**
		** s.shrink();
		** s.empty();          // returns true
		** s.null();           // returns true
		** \endcode
		*/
		bool empty() const;

		/*!
		** \brief Get if the cstr is null
		**
		** A null cstr means that no space is reserved, and that the
		** method `data()` will return NULL.
		**
		** \code
		** String s;
		** s.empty();          // returns true
		** s.null();           // returns true
		**
		** s = "hello world";  // returns false
		** s.empty();          // returns false
		** s.null();           // returns false
		**
		** s.clear();
		** s.empty();          // returns true
		** s.null();           // returns false
		**
		** s.shrink();
		** s.empty();          // returns true
		** s.null();           // returns true
		** \endcode
		*/
		bool null() const;

		/*!
		** \brief Get the current capacity of the string (in bytes)
		** \return The amount of memory used by the string
		*/
		Size capacity() const;

		//! \see capacity()
		size_t capacityInBytes() const;

		/*!
		** \brief Returns a pointer to a null-terminated character array with data equivalent to those stored in the string
		*/
		const char* c_str() const;

		/*!
		** \brief A pointer to the original cstr (might be NULL)
		** \see null()
		** \warning Not STL compatible
		*/
		char* data();
		/*!
		** \brief A pointer to the original cstr (might be NULL)
		** \see null()
		** \warning Not STL compatible
		*/
		const char* data() const;
		//@}


		//! \name Formatted buffer
		//@{
		/*!
		** \brief Reset the current value with a formatted string
		**
		** The format is the standard printf format.
		** \param format The format, represented by a zero-terminated string
		** \return Always *this
		*/
		CString& format(const char* format, ...);

		/*!
		** \brief Append formatted string
		**
		** The format is the standard printf format.
		** \param format The format, represented by a zero-terminated string
		** \return Always *this
		*/
		CString& appendFormat(const char* format, ...);

		/*!
		** \brief Append a formatted string to the end of the current string
		**
		** The format is the standard printf format.
		** \param format The format, represented by a zero-terminated C-String
		** \param args   The arguments required by the format
		*/
		void vappendFormat(const char* format, va_list args);
		//@}


		//! \name Misc
		//@{
		/*!
		** \brief Get the hash of the string (SDBM Hash Function)
		*/
		size_t hash() const;

		/*!
		** \brief Get the first char of the string
		** \return The last char of the string if not empty, \0 otherwise
		*/
		char first() const;

		/*!
		** \brief Get the last char of the string
		** \return The last char of the string if not empty, \0 otherwise
		*/
		char last() const;


		/*!
		** \brief Get if the string matches a simple pattern ('*' only managed)
		**
		** \param pattern A pattern
		** \warning This method should not be used in a new code and will be removed
		**   as soon as possible
		** \todo To be removed as soon as possible
		*/
		bool glob(const AnyString& pattern) const;

		/*!
		** \brief Convert all backslashes into slashes
		*/
		void convertBackslashesIntoSlashes();

		/*!
		** \brief Convert all slashes into backslashes
		*/
		void convertSlashesIntoBackslashes();

		/*!
		** \brief Iterate through all words in the string, all separated by one of the given separators
		**
		** \code
		** String t = "a, b, c";
		** t.words(" ,\t\r\n", [&] (const AnyString& word) -> bool
		** {
		**	std::cout << word << std::endl; // 3 elements : 'a', 'b' and 'c'
		**	return true; // continue to the next token
		** });
		** \endcode
		**
		** Dealing with empty words :
		** \code
		** String t = "a, b,, d";
		** t.words(",", [&] (AnyString& word) -> bool
		** {
		**	word.trim();
		**	std::cout << word << std::endl; // 4 elements : 'a', ' b', '' and ' d' (without the trim)
		**	return true; // continue to the next token
		** });
		** \endcode
		*/
		template<class PredicateT>
		bool words(const AnyString& separators, const PredicateT& predicate, bool keepEmptyElements = true) const;

		/*!
		** \brief Split a string into several segments
		**
		** Here is an example of how to convert a string to a list of int :
		** \code
		** std::list<int>  list;
		** String("22::80::443::993").split(list, ":");
		** std::cout << list << std::endl;
		** \endcode
		**
		** \warning Performance Tip: For better performances, the method \p words
		**   would suit better
		**
		** \param[out] out All segments that have been found
		** \param sep Sequence of chars considered as a separator
		** \param keepEmptyElements True to keep empty items
		** \param trimElements Trim each item found
		** \param emptyBefore True to clear the vector before fulfill it
		**
		** \warning This method does not take care of string representation (with `'` or `"`)
		** \see words()
		*/
		template<template<class,class> class ListT, class UType, class Alloc>
		void split(ListT<UType,Alloc>& out, const AnyString& sep,
			bool keepEmptyElements = false, bool trimElements = true, bool emptyBefore = true) const;

		/*!
		** \brief Duplicate N times the content of the string
		*/
		void duplicate(int n);


		/*!
		** \brief Extract the key and its value from a string (mainly provided by TDF
		** files or Ini files)
		**
		** Simple Ini file structure
		** \code
		** [section]
		** key = value
		** \endcode
		**
		** More complex :
		** \code
		** [section]
		** a = b; // Put your comments here
		** b =
		** c = ; // b = c, empty values
		** return carriage = A long string\non two lines
		** "key" = "All characters are allowed here, like semicolons; :)"
		** \endcode
		**
		** \param[out] key The key that has been found
		** \param[out] value The associated value
		** \param ignoreCase True to be case insensitive
		** \param separator The separator used to make the distinction between the key
		**   and the value
		**
		** \see ExtractKeyvalue()
		*/
		template<class StringT1, class StringT2>
		void extractKeyValue(StringT1& key, StringT2& value, bool ignoreCase = false,
			char separator = '=') const;
		//@}


		//! \name Adaptor only
		//@{
		/*!
		** \brief Adapt from a mere C-String
		**
		** \param cstring A C-string
		** \param length Length of the string
		*/
		void adapt(const char* cstring, Size length);

		/*!
		** \brief Adapt from any known string
		*/
		template<class StringT> void adapt(const StringT& string);
		/*!
		** \brief Adapt from any known string
		*/
		template<class StringT> void adapt(const StringT& string, Size length, Size offset);
		//@}


		//! \name Operators
		//@{
		//! The operator `[]`, for accessing to a single char (the offset must be valid)
		ConstOperatorBracketReturnType /*char&*/ operator [] (Size offset) const;
		//! The operator `[]`, for accessing to a single char (the offset must be valid)
		OperatorBracketReturnType /*char&*/ operator [] (Size offset);

		//! The operator `+=` (append)
		template<class U> CString& operator += (const U& rhs);
		//! The operator `<<` (append)
		template<class U> CString& operator << (const U& rhs);

		//! The operator `=` (assign - copy)
		CString& operator = (const CString& rhs);
		//! The operator `=` (assign)
		template<class U> CString& operator = (const U& rhs);

		# ifdef YUNI_HAS_CPP_MOVE
		//! Move operator
		CString& operator = (CString&& rhs);
		# endif


		//! The operator `<`
		bool operator <  (const AnyString& rhs) const;
		//! The operator `>`
		bool operator >  (const AnyString& rhs) const;

		//! The operator `<=`
		bool operator <= (const AnyString& rhs) const;
		//! The operator `>=`
		bool operator >= (const AnyString& rhs) const;

		//! The operator `==`
		bool operator == (const AnyString& rhs) const;
		//! The operator `==`
		bool operator == (char rhs) const;

		//! The operator `!=`
		bool operator != (const AnyString& rhs) const;
		//! The operator `!=`
		bool operator != (char rhs) const;

		//! The operator `!`  (if (!s) ... - equivalent to if (s.empty()))
		bool operator ! () const;

		//! The operator *=, to dupplicate N times the content of the string
		CString& operator *= (int n);
		//@}


	protected:
		//! Assign without checking for pointer validity
		Size assignWithoutChecking(const char* const block, Size blockSize);
		//! Append without checking for pointer validity
		Size appendWithoutChecking(const char* const block, Size blockSize);
		//! Append without checking for pointer validity
		Size appendWithoutChecking(char c);
		//! Assign without checking for pointer validity
		Size assignWithoutChecking(char c);
		//! Adapt without any check
		void adaptWithoutChecking(const char* const cstring, Size size);
		//! Decal the inner data pointer (must only be used when the class is an adapter)
		void decalOffset(Size count);

	private:
		/*!
		** \brief Set the string from a sequence of escaped characters (O(N))
		**
		** \param str The original string
		** \param maxLen The maximum length allowed
		** \param offset The offset where to start from
		*/
		void assignFromEscapedCharacters(const char* const str, Size maxLen, Size offset);


	private:
		// our friends !
		template<class, class, int> friend class Private::CStringImpl::From;
		template<class, class> friend class Yuni::Extension::CString::Append;
		template<class, class> friend class Yuni::Extension::CString::Assign;
		template<class, class> friend class Yuni::Extension::CString::Fill;
		template<class, bool>  friend struct Private::CStringImpl::AdapterAssign;
		template<class, bool>  friend struct Private::CStringImpl::Consume;
		template<uint, bool>   friend class CString;

	}; // class CString









	/*!
	** \brief An empty string
	**
	** This string will always remain empty. It is mainly used
	** for returning references
	*/
	extern const String emptyString;

} // namespace Yuni







namespace Yuni
{
namespace Traits
{



} // namespace Traits
} // namespace Yuni

#include "iterator.hxx"
#include "string.hxx"
#include "operators.hxx"

