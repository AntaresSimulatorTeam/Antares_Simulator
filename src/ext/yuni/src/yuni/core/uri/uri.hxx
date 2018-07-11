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
#include "uri.h"



namespace Yuni
{

	inline Uri::Uri(const AnyString& string)
	{
		extractURIFromString(string);
	}


	inline void Uri::clear()
	{
		pInfos.clear();
	}


	inline const String& Uri::scheme() const
	{
		return pInfos.scheme;
	}

	inline const String& Uri::server() const
	{
		return pInfos.server;
	}

	inline int Uri::port() const
	{
		return pInfos.port;
	}

	inline bool Uri::hasPort() const
	{
		return (INT_MIN != pInfos.port);
	}

	inline const String& Uri::user() const
	{
		return pInfos.user;
	}


	inline const String& Uri::password() const
	{
		return pInfos.password;
	}


	inline const String& Uri::path() const
	{
		return pInfos.path;
	}

	inline const String& Uri::query() const
	{
		return pInfos.query;
	}


	inline const String& Uri::fragment() const
	{
		return pInfos.fragment;
	}



	inline Net::Protocol::Type Uri::protocol() const
	{
		return pInfos.isValid
			? Net::Protocol::SchemeToType(pInfos.scheme)
			: Net::Protocol::unknown;
	}


	inline bool Uri::schemeIsFile() const
	{
		return (pInfos.scheme.empty() and not pInfos.path.empty()) or "file" == pInfos.scheme;
	}

	inline bool Uri::schemeIsHTTP() const
	{
		return not pInfos.scheme.empty() and ("http" == pInfos.scheme or "https" == pInfos.scheme);
	}

	inline bool Uri::schemeIsFTP() const
	{
		return not pInfos.scheme.empty() and "ftp" == pInfos.scheme;
	}

	inline bool Uri::schemeIsSSH() const
	{
		return not pInfos.scheme.empty() and "ssh" == pInfos.scheme;
	}

	inline bool Uri::schemeIsLDAP() const
	{
		return not pInfos.scheme.empty() and "ldap" == pInfos.scheme;
	}



	inline bool Uri::isValid() const
	{
		return pInfos.isValid;
	}





	inline Uri& Uri::operator = (const Uri& rhs)
	{
		pInfos.assign(rhs.pInfos);
		return *this;
	}


	template<class StringT>
	inline Uri& Uri::operator = (const StringT& rhs)
	{
		extractURIFromString(rhs);
		return *this;
	}


	inline bool Uri::operator == (const Uri& rhs) const
	{
		return pInfos.isEqualsTo(rhs.pInfos);
	}


	inline bool Uri::operator != (const Uri& rhs) const
	{
		return !((*this) == rhs);
	}


	inline String Uri::operator() () const
	{
		return pInfos.toString();
	}


	inline String Uri::toString() const
	{
		return pInfos.toString();
	}



} // namespace Yuni




//! \name Operator overload for stream printing
//@{
inline std::ostream& operator << (std::ostream& out, const Yuni::Uri& u)
{
	return u.print(out);
}
//@}


