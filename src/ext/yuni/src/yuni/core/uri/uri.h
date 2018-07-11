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
#include <limits.h>
#include <ostream>
#include "../../net/protocol.h"
#include "uri.private.h"
#include "../string.h"



namespace Yuni
{

	/*!
	** \brief Uniform resource Identifier
	**
	**
	** This implementation of URIs follows the RFC 3986
	** \see http://www.ietf.org/rfc/rfc3986.txt
	**
	** Here are some examples of URIs (extracted from the RFC itself)
	** \code
	** file:///etc/hosts
	** ftp://ftp.is.co.za/rfc/rfc1808.txt
	** http://www.ietf.org/rfc/rfc3986.txt
	** ldap://[2001:db8::7]/c=GB?objectClass?one
	** mailto:John.Doe@example.com
	** news:comp.infosystems.www.servers.unix
	** tel:+1-816-555-1212
	** telnet://192.0.2.16:80/
	** urn:oasis:names:specification:docbook:dtd:xml:4.1.2
	** \endcode
	**
	** Composition (extracted from the RFC too) :
	** \code
	**  foo://example.com:8042/over/there?name=ferret#nose
	**  \_/   \______________/\_________/ \_________/ \__/
	**   |           |            |            |        |
	** scheme     authority       path        query   fragment
	**   |   _____________________|__
	**  / \ /                        \
	**  urn:example:animal:ferret:nose
	** \endcode
	**
	**
	** Here is a typical way of how to use this class :
	** \code
	** Uri uri("http://www.example.org");
	** uri.defaultPathIfEmpty(); // Ensure the path is not empty
	** switch (uri.protocol())
	** {
	**		case Net::Protocol::http  : ...; break; // Deals with the HTTP protocol
	**		case Net::Protocol::ftp   : ...; break; // Deals with the FTP protocol
	**		default: ...; // Unsupported protocol
	** }
	** \endcode
	**
	**
	** It is possible to modify only a part of the URI :
	** \code
	** // The original URI
	** Uri uri("http://www.example.org/?myquery=foo");
	**
	** uri.query("anotherquery=bar");
	** std::cout << uri << std::endl; // http://www.example.org/?anotherquery=bar
	**
	** uri.user("myuser");
	** std::cout << uri << std::endl; // http://myuser@www.example.org/?anotherquery=bar
	** \endcode
	**
	**
	**
	** \note An URL refers to a subset of URIs
	**
	** \note <b>Modifying an URI</b> : If any modification is brought to any part
	** of the URI, the returned value of the method `isValid()` should no longer
	** be used, because this variable is set when the URI is built from a simple
	** string.
	**
	**
	**
	** \note <b>Special scheme</b> : When the scheme `file` is used, the server
	** field should not be used (`file://foo/tmp` should lead to `/tmp`, but
	** `file:///foo/tmp` will lead to `/foo/tmp`).
	**
	**
	**
	** \note <b>Suffix reference</b> : The URI syntax is designed for unambiguous
	** reference to resources but traditional uses allow (for convenient reasons)
	** a suffix of the URI as a reference consisting of only the authority and path
	** portions of the URI, such as `www.libyuni.org` or `google`. This class does
	** not make any changes on the server name :
	** \code
	** Uri uri("google");
	** std::cout << uri.server() << std::endl; // `google`
	**
	** Uri uri2("www.libyuni.org");
	** std::cout << uri2.server() << std::endl; // `www.libyuni.org`
	**
	** Uri uri3("www.libyuni.org/downloads");
	** // Server: `www.libyuni.org`, path: `/downloads`
	** std::cout << "Server: `" << uri3.server() << "`, path: `" << uri3.path() << "`" << std::endl;
	** \endcode
	**
	**
	**
	** \note <b>IPv6 addresses</b> : According to the RFC, IPv6 addresses must be
	** enclosed in brackets (ex: `http://[2001:db8::7]`)
	**
	**
	**
	** \note <b>Dot segments Removal</b> : In the most cases, the special segments
	** `.` and `..` will be removed (`file:///path/./to/../somewhere` will give
	** `file:///path/somewhere`). However it can not be done for the begining of
	** relative paths (`file://../../relative/./path` will give
	** `file://../../relative/path`). Consequently a path that starts with a
	** dot is a relative path.
	**
	**
	** \bug This implementation does not handle at all escaped caracters
	*/
	class YUNI_DECL Uri final
	{
	public:
		//! \name Constructors
		//@{
		/*!
		** \brief Default constructor
		*/
		Uri();

		/*!
		** \brief Constructor with a string, to directly construct the URI
		*/
		Uri(const AnyString& string);

		/*!
		** \brief Copy constructor
		*/
		explicit Uri(const Uri& rhs);
		//@}


		//! \name Informations about the URI structure
		//@{
		/*!
		** \brief Clear all informations about the URI structure
		*/
		void clear();

		//! Scheme (lowercase)
		const String& scheme() const;
		//! Set the scheme
		void scheme(const AnyString& newscheme);
		void scheme(const Net::Protocol::Type& type);

		//! Server
		const String& server() const;
		//! Set the server
		void server(const AnyString& newserver);

		//! Port (equals to `INT_MIN` if none)
		int port() const;
		//! Set the port value (only if strictly positive, or equals to INT_MIN do disable it)
		void port(int p);
		//! Get if a port has been provided
		bool hasPort() const;

		//! User
		const String& user() const;
		//! Set the user
		void user(const AnyString& newuser);

		//! Password
		const String& password() const;
		//! Set the password
		void password(const AnyString& newpassword);

		//! Path
		const String& path() const;
		//! Set the path
		void path(const AnyString& newpath);
		//! Set a default path (`/`) if there is no path
		void defaultPathIfEmpty();

		//! Query
		const String& query() const;
		//! Set the query
		void query(const AnyString& newquery);

		//! Fragment
		const String& fragment() const;
		//! Set the fragment
		void fragment(const AnyString& newfragment);

		/*!
		** \brief Get if the URI was valid during the last extract
		**
		** \note If any modification is brought to any part of the URI, the returned
		** value of this method should no longer be used, because this variable is set
		** when the URI is built from a simple string.
		*/
		bool isValid() const;
		//@}


		//! \name Scheme
		//@{
		/*!
		** \brief Try to find out the network protocol from the scheme
		*/
		Net::Protocol::Type protocol() const;

		/*!
		** \brief Convenient method to know if the URI is merely a file
		*/
		bool schemeIsFile() const;

		//! Get if the scheme is `HTTP` or `HTTPS`
		bool schemeIsHTTP() const;

		//! Get if the scheme is `FTP`
		bool schemeIsFTP() const;

		//! Get if the scheme is `SSH`
		bool schemeIsSSH() const;

		//! Get if the scheme is `LDAP`
		bool schemeIsLDAP() const;
		//@}


		//! \name Conversion
		//@{
		/*!
		** \brief Reconstruct the URI to a string
		*/
		String toString() const;

		/*!
		** \brief Print this URI to a stream output
		** \param out The stream output
		*/
		std::ostream& print(std::ostream& out) const;
		//@}


		//! \name Operators
		//@{
		//! The operator =
		Uri& operator = (const Uri& rhs);
		template<class StringT> Uri& operator = (const StringT& rhs);

		//! The operator ==
		bool operator == (const Uri& rhs) const;
		//! The operator !=
		bool operator != (const Uri& rhs) const;
		//! The operator ()
		String operator() () const;
		//@}


	private:
		/*!
		** \brief Build URI Informations from a string
		** \param raw The string
		*/
		void extractURIFromString(const AnyString& raw);

	private:
		//! Structured Informations about the URI
		Private::Uri::Informations pInfos;

	}; // class Uri





} // namespace Yuni

# include "uri.hxx"

