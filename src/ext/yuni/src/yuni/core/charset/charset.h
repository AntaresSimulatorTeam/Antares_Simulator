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
#include "../string.h"
#include <errno.h>


namespace Yuni
{
//! Everything related to charset conversions (EXPERIMENTAL)
namespace Charset
{

	/*!
	** \brief Enumeration of common charsets
	**
	** \warning The order of this enum matches
	** the order of the strings in the TypeAsString[] array.
	** You should only add types to this list, not change its
	** order in order to preserve the ABI.
	*/
	enum Type
	{
		/* European Languages */
		ASCII,
		ISO_8859_1,
		ISO_8859_2,
		ISO_8859_3,
		ISO_8859_4,
		ISO_8859_5,
		ISO_8859_7,
		ISO_8859_9,
		ISO_8859_10,
		ISO_8859_13,
		ISO_8859_14,
		ISO_8859_15,
		ISO_8859_16,
		KOI8_R,
		KOI8_U,
		KOI8_RU,
		CP1250,
		CP1251,
		CP1252,
		CP1253,
		CP1254,
		CP1257,
		CP850,
		CP866,
		CP1131,
		MacRoman,
		MacCentralEurope,
		MacIceland,
		MacCroatian,
		MacRomania,
		MacCyrillic,
		MacUkraine,
		MacGreek,
		MacTurkish,
		Macintosh,

		/* Semitic Languages */
		ISO_8859_6,
		ISO_8859_8,
		CP1255,
		CP1256,
		CP862,
		MacHebrew,
		MacArabic,

		/* Japanese */
		EUC_JP,
		SHIFT_JIS,
		CP932,
		ISO_2022_JP,
		ISO_2022_JP_2,
		ISO_2022_JP_1,

		/* Chinese */
		EUC_CN,
		HZ,
		GBK,
		CP936,
		GB18030,
		EUC_TW,
		BIG5,
		CP950,
		BIG5_HKSCS,
		BIG5_HKSCS_2001,
		BIG5_HKSCS_1999,
		ISO_2022_CN,
		ISO_2022_CN_EXT,

		/* Korean */
		EUC_KR,
		CP949,
		ISO_2022_KR,
		JOHAB,

		/* Armenian */
		ARMSCII_8,

		/* Georgian */
		Georgian_Academy,
		Georgian_PS,

		/* Tajik */
		KOI8_T,

		/* Kazakh */
		PT154,
		RK1048,

		/* Thai */
		TIS_620,
		CP874,
		MacThai,

		/* Laotian */
		MuleLao_1,
		CP1133,

		/* Vietnamese */
		VISCII,
		TCVN,
		CP1258,

		/* Platform specifics */
		HP_ROMAN8,
		NEXTSTEP,

		/* Full Unicode */
		UTF_8,
		UCS_2,
		UCS_2BE, UCS_2LE,
		UCS_4, UCS_4BE, UCS_4LE,
		UTF_16, UTF_16BE, UTF_16LE,
		UTF_32, UTF_32BE, UTF_32LE,
		UTF_7,
		C99,
		JAVA,

		// Full Unicode, in terms of uint16_t or uint32_t
		// (with machine dependent endianness and alignment)
		UCS_2_INTERNAL,
		UCS_4_INTERNAL,

		// Locale dependent, in terms of char or wchar_t
		// (with machine dependent endianness and alignment, and with semantics
		//	depending on the OS and the current LC_CTYPE locale facet)
		CHAR,
		WCHAR_T,

	}; // enum Type

	/*!
	** \brief Enumeration of common charsets (as strings)
	**
	** \warning The order of this enum matches
	** the order of the items in the Type enum.
	** You should only add types to this list, not change its
	** order in order to preserve the ABI.
	*/
	static const char* const TypeAsString[] =
	{
		/* European Languages */
		"ASCII",
		"ISO-8859-1",
		"ISO-8859-2",
		"ISO-8859-3",
		"ISO-8859-4",
		"ISO-8859-5",
		"ISO-8859-7",
		"ISO-8859-9",
		"ISO-8859-10",
		"ISO-8859-13",
		"ISO-8859-14",
		"ISO-8859-15",
		"ISO-8859-16",
		"KOI8-R",
		"KOI8-U",
		"KOI8-RU",
		"CP1250",
		"CP1251",
		"CP1252",
		"CP1253",
		"CP1254",
		"CP1257",
		"CP850",
		"CP866",
		"CP1131",
		"MacRoman",
		"MacCentralEurope",
		"MacIceland",
		"MacCroatian",
		"MacRomania",
		"MacCyrillic",
		"MacUkraine",
		"MacGreek",
		"MacTurkish",
		"Macintosh",

		/* Semitic Languages */
		"ISO-8859-6",
		"ISO-8859-8",
		"CP_1255",
		"CP_1256",
		"CP862",
		"MacHebrew",
		"MacArabic",

		/* Japanese */
		"EUC-JP",
		"SHIFT_JIS",
		"CP932",
		"ISO-2022-JP",
		"ISO-2022-JP-2",
		"ISO-2022-JP-1",

		/* Chinese */
		"EUC-CN",
		"HZ",
		"GBK",
		"CP936",
		"GB18030",
		"EUC-TW",
		"BIG5",
		"CP950",
		"BIG5-HKSCS",
		"BIG5-HKSCS:2001",
		"BIG5-HKSCS:1999",
		"ISO-2022-CN",
		"ISO-2022-CN-EXT",

		/* Korean */
		"EUC-KR",
		"CP949",
		"ISO-2022-KR",
		"JOHAB",

		/* Armenian */
		"ARMSCII-8",

		/* Georgian */
		"Georgian-Academy",
		"Georgian-PS",

		/* Tajik */
		"KOI8-T",

		/* Kazakh */
		"PT154",
		"RK1048",

		/* Thai */
		"TIS-620",
		"CP874",
		"MacThai",

		/* Laotian */
		"MuleLao-1",
		"CP1133",

		/* Vietnamese */
		"VISCII",
		"TCVN",
		"CP1258",

		/* Platform specifics */
		"HP-ROMAN8",
		"NEXTSTEP",

		/* Full Unicode */
		"UTF-8",
		"UCS-2",
		"UCS-2BE", "UCS-2LE",
		"UCS-4", "UCS-4BE", "UCS-4LE",
		"UTF-16", "UTF-16BE", "UTF-16LE",
		"UTF-32", "UTF-32BE", "UTF-32LE",
		"UTF-7",
		"C99",
		"JAVA",

		// Full Unicode, in terms of uint16_t or uint32_t
		// (with machine dependent endianness and alignment)
		"UCS_2_INTERNAL",
		"UCS_4_INTERNAL",

		// Locale dependent, in terms of char or wchar_t
		// (with machine dependent endianness and alignment, and with semantics
		//   depending on the OS and the current LC_CTYPE locale facet)
		"char",
		"wchar_t",

	}; // static const char * const TypeAsString


	//! Conversion options
	enum Unconvertable
	{
		//! Transliterate characters, and ignore invalid ones in the source.
		uncTranslitIgnore,
		//! Transliterate characters, and stop on invalid ones in the source.
		uncTranslit,
		//! Ignore all unrepresentable characters in destination encoding.
		uncIgnore,
		//! Stop the conversion when encountering any unrepresentable character.
		uncStop,
	};


	/*!
	** \brief Charset conversion
	**
	** This class provides a character set conversion system.
	** You instanciate it by passing the FROM and TO charsets,
	** and it can do stateful conversions. (ie, convert multiple
	** buffers one after the other, while taking care of characters
	** split between two buffers.
	**
	*/
	class Converter
	{
	public:
		//! \name Constructors and destructors
		//@{
		/*!
		** Constructs a converter, but do not set a charset.
		**
		** You must use open() before calling anything other than valid().
		** \see open(), valid()
		*/
		Converter();

		Converter(Charset::Type fromCS, Charset::Type toCS, Charset::Unconvertable opts = Charset::uncTranslit);

		/*!
		** Constructs a converter using real charset names and flags.
		**
		** Using this method, you can open a converter the iconv_open(3) way:
		** \code
		** Charset::Converter converter("UTF-8", "ASCII");
		** Charset::Converter converter("UTF-8", "ASCII//TRANSLIT");
		** Charset::Converter converter("UTF-8", "ISO-8859-1//TRANSLIT//IGNORE");
		** Charset::Converter converter("UTF-8", "UTF-16//IGNORE");
		** \endcode
		*/
		Converter(const char* fromCS, const char* toCS);

		/*!
		** Destructor
		*/
		~Converter();
		//@}

		/*!
		** Returns the last error encountered (errno-style)
		**
		** \return the last error encountered by the object.
		*/
		int lastError() const;

		/*!
		** Checks the validity of the conversion context
		**
		** All operations except checking the validity will fail on
		** an invalid conversion context.
		** This function may return false for a number of reasons:
		**   - The conversion is not valid (invalid from/to charsets,
		**     conversion not implemented by the underlying iconv() functions...)
		**   - The object was constructed using the default constructor and a
		**     conversion context was not open()ed.
		**
		** \return true if the converter has been properly initialized.
		*/
		bool valid() const;

		/*!
		** \brief Closes the conversion context
		**
		** Closing and re-opening a conversion context is pratically
		** equivalent to creating a new conversion object. We recommend
		** creating a new object instead of closing and reopening one,
		** to preserve the legibility of the code.
		*/
		void close();

		/*!
		** \brief Resets the conversion context
		**
		** You must reset the conversion context when you are
		** reusing the same conversion object to perform operations
		** on unrelated strings.
		**
		** This is because certain encodings are stateful (can shift
		** state between 1-byte and 2-bytes characters for example).
		*/
		void reset();

		/*!
		** \brief Performs a conversion on memory buffers.
		**
		** Performs a conversion, reading in the source encoding from the
		** source buffer, and writing to the destination buffer. The
		** destination buffer will be resized automatically if needed.
		**
		** Incomplete character sequences will be ignored temporarily if
		** isLastBuffer is set to false. This behavior can be useful if
		** you are reading a file block-by-block, and therefore only want
		** to do this check on the last converted block.
		**
		** On completion:
		**  - the source memory buffer will be depleted of
		**    the converted characters
		**  - the destination buffer will be appended the converted
		**    characters
		**
		** \return True if the conversion was successful, false otherwise.
		*/
		template <typename SrcStringT, typename DstStringT>
		bool convert(SrcStringT& source, DstStringT& destination, bool isLastBuffer = true);

	public:
		/*!
		** Returns the real name of the specified charset.
		**
		** \return Returns a pointer on a static C string containing
		** the name of the specified charset.
		*/
		static const char* Name(Charset::Type type);

	private:
		Converter& operator = (const Converter & /* rhs */);
		Converter(const Converter & /* rhs */);

		/*!
		** Creates a conversion context.
		**
		** Takes two encoding names, as strings, according to the
		** libiconv documentation. //TRANSLIT and //IGNORE options are
		** supported depending on the underlying iconv library.
		*/
		bool open(const char* from, const char* to);

		/*!
		** Wrapper around iconv().
		**
		** We need to avoid using iconv.h (iconv_t, etc) in headers.
		** See iconv(3).
		*/
		size_t IconvWrapper(void* cd, char** inbuf, size_t* inbytesleft,
			char** outbuf, size_t* outbytesleft);

	private:
		//! This is really an iconv_t.
		void* pContext;
		//! Last error
		int pLastError;

	}; // class Converter





} // namespace Charset
} // namespace Yuni

#include "charset.hxx"
