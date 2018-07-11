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
#ifndef __YUNI_NET_PROTOCOLS_H__
# define __YUNI_NET_PROTOCOLS_H__

# include "../yuni.h"
# include "../core/string.h"



namespace Yuni
{
namespace Net
{

/*!
** \brief Protocols
** \ingroup Protocols
*/
namespace Protocol
{


	/*!
	** \brief Simple list of protocols and pseudo protocols
	**
	** \internal When adding a protocol in this list, do not forget to add
	** the corresponding scheme in the routine `SchemeToType()` and `ToScheme()`
	** \see SchemeToType()
	** \see ToScheme()
	*/
	enum Type
	{
		//! Unknown protocol
		unknown,

		//! \name Pseudo protocols
		//@{
		//! Local file
		file,
		//! News
		news,
		//@}


		//! \name Protocols
		//@{

		//! Domain (Name-domain Server)
		domain,

		//! FTP (File Transfer Protocol)
		ftp,

		//! HTTP
		http,
		//! HTTP over SSL
		https,

		//! IMAP (Interim Mail Access Protocol)
		imap,
		//! IMAP over SSL
		imaps,
		//! IRC (Internet Relay Chat)
		irc,
		//! IRC over SSL
		ircs,

		//! Kerberos
		kerberos,

		//! LDAP
		ldap,
		//! LDAP over SSL
		ldaps,

		//! NFS
		nfs,
		//! NTP (Network Time Protocol)
		ntp,

		//! POP v3
		pop3,
		//! POP v3 over SSL
		pop3s,

		//! RTSP (Real Time Stream Control Protocol)
		rtsp,

		//! SFTP (SSH File Transfer Protocol)
		sftp,
		//! SMTP
		smtp,
		//! SMTP over SSL
		smtps,
		//! SNMP (Simple Net Mgmt Protocol)
		snmp,
		//! SSH (Secure SHell)
		ssh,
		//! Subversion
		svn,

	}; // enum Type






	/*!
	** \brief Try to determine the protocol from a scheme (URI)
	**
	** \param s A string (ex: `ldap`, `svn`...)
	** \return A protocol identified by the scheme, `unknown` otherwise
	*/
	Type SchemeToType(const String& s);





} // namespace Protocol
} // namespace Net
} // namespace Yuni

#endif // __YUNI_NET_PROTOCOLS_H__
