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
#include <iconv.h>
#include "charset.h"

namespace Yuni
{
namespace Charset
{

	Converter::Converter(Charset::Type fromCS, Charset::Type toCS, Charset::Unconvertable opts)
		:pContext((iconv_t)-1), pLastError(0)
	{
		// 64 Should be sufficient to hold ISO-8859-15//IGNORE//TRANSLIT
		char toBuf[64];

		strcpy(toBuf, Charset::TypeAsString[(int)toCS]);

		if (opts == Charset::uncTranslit || opts == Charset::uncTranslitIgnore)
			strcat(toBuf, "//TRANSLIT");

		if (opts == Charset::uncTranslitIgnore || opts == Charset::uncIgnore)
			strcat(toBuf, "//IGNORE");

		// Open the conversion context using our generated charset strings.
		open(Charset::TypeAsString[(int)fromCS], toBuf);
	}


	Converter::Converter(const char* fromCS, const char* toCS)
		: pContext((void*)-1), pLastError(0)
	{
		open(fromCS, toCS);
	}


	Converter::Converter()
		: pContext((void*)-1), pLastError(0)
	{}


	Converter::~Converter()
	{
		if (valid())
			iconv_close((iconv_t)pContext);
		// pContext = -1; This object will be destroyed anyways.
	}


	void Converter::close()
	{
		if (!valid())
			return;
		iconv_close((iconv_t)pContext);
		pContext = (iconv_t)-1;
	}


	void Converter::reset()
	{
		if (!valid())
			return;
		iconv((iconv_t)pContext, NULL, NULL, NULL, NULL);
	}


	const char * Converter::Name(Charset::Type type)
	{
		return Charset::TypeAsString[(int)type];
	}


	bool Converter::open(const char* from, const char* to)
	{
		pContext = (void *)iconv_open(to, from);
		bool isValid = valid();
		if (!isValid)
			pLastError = errno;
		return isValid;
	}


	size_t Converter::IconvWrapper(void* cd, char **inbuf, size_t* inbytesleft,
		char** outbuf, size_t* outbytesleft)
	{
		return iconv((iconv_t)cd, inbuf, inbytesleft, outbuf, outbytesleft);
	}





} // namespace Charset
} // namespace Yuni
