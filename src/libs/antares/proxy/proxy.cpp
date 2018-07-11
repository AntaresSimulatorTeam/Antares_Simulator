/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "../inifile/inifile.h"
#include <yuni/io/file.h>
#include <yuni/yuni.h>
#include <antares/sys/appdata.h>
#include <yuni/core/system/environment.h>
#include "../internet/limits.h"
#include "../../../config.h"
#include "proxy.h"

#include "../../../internet/license.h"
#include "../../../internet/base64/cencode.h"
#include "../../../internet/decrypt.hxx"
#include <curl/curl.h>
#include <openssl/des.h>
#include <openssl/rand.h>
#include <sstream>
#include <yuni/core/utils/hexdump.h>

using namespace Yuni;

#define SEP Yuni::IO::Separator
#define BUFSIZE 256 

namespace Antares
{

	//! Global mutex for proxy
	Yuni::Mutex ProxyMutex;

	static inline void obfuscate(YString& out, YString& value)
	{
		// preparing the message
		Clob message;
		message = value;
		
		// encrypting the message with the key 
		License::EncryptionKey enckey;
		enckey.key = "ENId99w5R466zoQt";
		enckey.iv.assign("ENId99w5R466zoQt", enckey.key.size());
		
		// -1- encrypt
		Clob encrypted;
		encrypted.reserve(message.size()*2+enckey.key.size()*2);
		Encrypt(encrypted, message, enckey);
		
		String encoded;
		encoded.reserve(encrypted.size());

		// -2- encode
		base64_encodestate state;
		base64_init_encodestate(&state);

		auto length = base64_encode_block(encrypted.c_str(), encrypted.size(), encoded.data(), &state);
		length += base64_encode_blockend(encoded.data() + length, &state);
		encoded.resize(length);

		out = encoded;

		/*static const AnyString& keyCBF64 = "ENId99w5R466zoQt";
		static const AnyString& ivecStr = "ENId99w5R466zoQt";
	
		int len;
		int n = 0;
		out.resize(BUFSIZE);

		DES_cblock ivec;
		DES_key_schedule keySchedule;

		// Converting the key into architecture dependent format 
		DES_set_key_checked((C_Block *)keyCBF64.c_str(), &keySchedule);

		memcpy(ivec, (C_Block *)ivecStr.c_str(), sizeof(ivec));
		len = value.size() + 1;

		// Encryption
		DES_cfb64_encrypt((const unsigned char*)value.c_str(), (unsigned char*)out.data(), len, &keySchedule, &ivec, &n, DES_ENCRYPT);
		*/
		
	}

	static inline void deobfuscate(YString& out, YString& value)
	{
		// decrypting the message with the key
		License::EncryptionKey enckey;
		enckey.key = "ENId99w5R466zoQt";
		enckey.iv.assign("ENId99w5R466zoQt", enckey.key.size());
		
		// -1- decode
		// the real message to decipher
		String decoded;
		decoded.reserve(value.size());
		
		base64_decodestate state;
		base64_init_decodestate(&state);
		auto length = base64_decode_block(value.c_str(), value.size(), decoded.data(), &state);
		
		assert((uint) length < decoded.capacity());
		decoded.resize(length);

		// -2- decrypt
		String decrypted;
		decrypted.reserve(decoded.size());
		License::Decrypt(decrypted, decoded, enckey);
		
		out = decrypted;

		/*static const AnyString& keyCBF64 = "ENId99w5R466zoQt";
		static const AnyString& ivecStr = "ENId99w5R466zoQt";

		out.resize(BUFSIZE);

		int len;
		int n = 0;

		if( !value.empty() )
		{
			len = value.size() + 1;

			DES_cblock ivec;
			DES_key_schedule keySchedule;

			// Converting the key into architecture dependent format 
			DES_set_key_checked((C_Block *)keyCBF64.c_str(), &keySchedule);

			memcpy(ivec, (C_Block *)ivecStr.c_str(), sizeof(ivec));
		
			// Unciphering
			DES_cfb64_encrypt((unsigned char*)value.c_str(), (unsigned char*)out.data(), len, &keySchedule, &ivec, &n, DES_DECRYPT);	
		}
		*/
	}

	static inline bool FindProxyFile(String& filename)
	{
		filename.clear();
		
		// search or create the directory		
		if (not OperatingSystem::FindAntaresLocalAppData(filename, false))
			return false;
		
		if (not IO::Directory::Create(filename))
			return false;

		// proxy filename
		filename << SEP << ANTARES_PROXY_PARAMETERS_FILENAME;
		return true;
	}


	bool ProxySettings::check()
	{
		int intPort;

		// Triming settings strings
		host.trim();
        host.replace(" ", "");
        host.trim();
        host.toLower();

		login.trim();
		password.trim();

		port.trim();
		port.replace(" ", "");

		// Check for missing parameters
		//if(password.empty()||login.empty()||host.empty()||port.empty())
		
		if(host.empty()||port.empty())
		{
			return false;
		}

		if(!port.to<int>(intPort))
		{
			return false;
		}

		if((intPort<10) || (intPort>65535))
		{
			return false;
		}

		return true;
	}


	bool ProxySettings::saveProxyFile()
	{
		Yuni::MutexLocker locker(ProxyMutex);

		Clob content;
		String plainCredentials, encCredentials;

		content << "[proxy]\n";
		content << "proxy.enabled = " << enabled << '\n';
		content << "proxy.host  = " << host << '\n';
		
		
		if( !login.empty() )
		{
			plainCredentials << login;

			if( !password.empty())
			{
				plainCredentials << ";:;" << password;
			}

			obfuscate(encCredentials, plainCredentials);
			content << "proxy.credentials = " << encCredentials << '\n';
		}
		
		content << "proxy.port = " << port << '\n';

		// search or create the directory
		String filename;
		if (not FindProxyFile(filename))
		{	
			logs.error() << "impossible to create the proxy directory. Please check your user account privileges";
			return false;
		}

		return IO::File::SetContent(filename, content);
	}


	bool ProxySettings::loadProxyFile()
	{
		Yuni::MutexLocker locker(ProxyMutex);

		// search the proxy file
		String filename;
		if (not FindProxyFile(filename))
			return false;

		String encCredentials, plainCredentials, encPass;
		IniFile ini;
		if (ini.open(filename, false))
		{
			for (auto* section = ini.firstSection; section; section = section->next)
			{
				for (auto* property = section->firstProperty; property; property = property->next)
				{
					auto & key   = property->key;
					auto & value = property->value;

					if (key.empty())
						continue;

					if (key == "proxy.enabled")
					{
						enabled = value.to<bool>();
						continue;
					}
					if (key == "proxy.host")
					{
						host.clear();
						host = value;
						host.trim();
						continue;
					}
					if (key == "proxy.port")
					{
						port.clear();
						port = value;
						continue;
					}
					if (key == "proxy.credentials")
					{
						encCredentials = value;
						continue;
					}
				}
			}

			if (not encCredentials.empty())
			{
				login.clear();
				password.clear();
				deobfuscate(plainCredentials, encCredentials);

				auto sep = plainCredentials.find(";:;");
				if (sep != YString::npos)
				{
					login = AnyString(plainCredentials, 0, sep);
					password = AnyString(plainCredentials, sep+3);
				}
				else
					login = plainCredentials;
			}
			
			return true;
		}
		else
		{
			//clear proxy parameters and disable proxy
			host.clear();
			port.clear();
			login.clear();
			password.clear();
			codingType.clear();
		}
		return false;
	}

} // namespace Antares

