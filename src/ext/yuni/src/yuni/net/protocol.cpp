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
#include "protocol.h"



namespace Yuni
{
namespace Net
{
namespace Protocol
{


	static inline Type schemeToType_F(const String& s)
	{
		if ("file" == s)
			return file;
		if ("ftp" == s)
			return ftp;
		return unknown;
	}


	static inline Type schemeToType_H(const String& s)
	{
		if ("http" == s)
			return http;
		if ("https" == s)
			return https;
		return unknown;
	}


	static inline Type schemeToType_I(const String& s)
	{
		if ("imap" == s)
			return imap;
		if ("imaps" == s)
			return imaps;
		if ("irc" == s)
			return irc;
		if ("ircs" == s)
			return ircs;
		return unknown;
	}


	static inline Type schemeToType_K(const String& s)
	{
		if ("kerberos" == s)
			return kerberos;
		return unknown;
	}


	static inline Type schemeToType_L(const String& s)
	{
		if ("ldap" == s)
			return ldap;
		if ("ldaps" == s)
			return ldaps;
		return unknown;
	}


	static inline Type schemeToType_N(const String& s)
	{
		if ("nfs" == s)
			return nfs;
		if ("ntp" == s)
			return ntp;
		if ("news" == s)
			return news;
		return unknown;
	}


	static inline Type schemeToType_P(const String& s)
	{
		if ("pop3" == s)
			return pop3;
		if ("pop3s" == s)
			return pop3s;
		return unknown;
	}


	static inline Type schemeToType_R(const String& s)
	{
		if ("rtsp" == s)
			return rtsp;
		return unknown;
	}



	static inline Type schemeToType_S(const String& s)
	{
		if ("ssh" == s)
			return ssh;
		if ("svn" == s)
			return snmp;
		if ("smtp" == s)
			return smtp;
		if ("sftp" == s)
			return sftp;
		if ("snmp" == s)
			return snmp;
		return unknown;
	}



	Type  SchemeToType(const String& s)
	{
		if (!s.empty())
		{
			// A little tip to reduce the count of string comparisons
			switch (s[0])
			{
				case 'f': return schemeToType_F(s);
				case 'h': return schemeToType_H(s);
				case 'i': return schemeToType_I(s);
				case 'k': return schemeToType_K(s);
				case 'l': return schemeToType_L(s);
				case 'n': return schemeToType_N(s);
				case 'p': return schemeToType_P(s);
				case 'r': return schemeToType_R(s);
				case 's': return schemeToType_S(s);
			}
		}
		return unknown;
	}



	String ToScheme(const Type& type)
	{
		switch (type)
		{
			case unknown : return String();
			case file : return "file";

			case news : return "news";

			case domain : return "ns";

			case ftp : return "ftp";

			case http : return "http";
			case https : return "https";

			case imap : return "imap";
			case imaps : return "imaps";
			case irc : return "irc";
			case ircs : return "ircs";

			case kerberos : return "kerberos";

			case ldap : return "ldap";
			case ldaps : return "ldaps";

			case nfs : return "nfs";
			case ntp : return "ntp";

			case pop3 : return "pop3";
			case pop3s : return "pop3s";

			case rtsp : return "rtsp";

			case sftp : return "sftp";
			case smtp : return "smtp";
			case smtps : return "smtp";
			case snmp : return "snmp";
			case ssh : return "ssh";
			case svn : return "svn";
		}
		return String();
	}





} // namespace Protocol
} // namespace Net
} // namespace Yuni
