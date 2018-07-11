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
#include "uri.private.h"
#include <limits.h>
#include <iostream>



namespace Yuni
{
namespace Private
{
namespace Uri
{


	Informations::Informations()
		: port(INT_MIN)
		, isValid(false)
	{}


	Informations::Informations(const Informations& rhs)
		: scheme(rhs.scheme)
		, user(rhs.user)
		, password(rhs.password)
		, server(rhs.server)
		, port(rhs.port)
		, path(rhs.path)
		, query(rhs.query)
		, fragment(rhs.fragment)
		, isValid(rhs.isValid)
	{}


	void Informations::clear()
	{
		scheme.clear();
		server.clear();
		user.clear();
		password.clear();
		port = INT_MIN;
		path.clear();
		query.clear();
		fragment.clear();
		isValid = false;
	}


	void Informations::assign(const Informations& rhs)
	{
		scheme   = rhs.scheme;
		server   = rhs.server;
		user     = rhs.user;
		port     = rhs.port;
		path     = rhs.path;
		query    = rhs.query;
		fragment = rhs.fragment;
		isValid  = rhs.isValid;
	}


	template<class U>
	static void WriteStructInformationsToStream(const Informations& infos, U& s)
	{
		if (infos.isValid)
		{
			if (not infos.scheme.empty())
				s << infos.scheme << ':';

			if (not infos.server.empty())
			{
				if (not infos.scheme.empty())
					s << "//";
				if (not infos.user.empty())
				{
					s << infos.user;
					if (not infos.password.empty())
						s << ':' << infos.password;
					s << "@";
				}
				s << infos.server;
				if (infos.port > 0)
					s << ':' << infos.port;
			}
			else
			{
				if (not infos.scheme.empty() and "file" == infos.scheme)
					s << "//";
			}

			s << infos.path;
			if (not infos.query.empty())
				s << '?' << infos.query;
			if (not infos.fragment.empty())
				s << '#' << infos.fragment;
		}
	}



	String Informations::toString() const
	{
		if (isValid)
		{
			String s;
			WriteStructInformationsToStream(*this, s);
			return s;
		}
		return String();
	}


	void Informations::print(std::ostream& out) const
	{
		if (isValid)
			WriteStructInformationsToStream(*this, out);
	}


	bool Informations::isEqualsTo(const Informations& rhs) const
	{
		return isValid and rhs.isValid and scheme == rhs.scheme
			 and path == rhs.path and server == rhs.server and port == rhs.port
			 and query == rhs.query and fragment == rhs.fragment
			 and user == rhs.user and password == rhs.password;
	}





} // namespace Uri
} // namespace Private
} // namespace Yuni

