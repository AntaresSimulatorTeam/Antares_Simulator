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
#ifndef __ANTARES_LICENSE_DECRYPT_HXX__
# define __ANTARES_LICENSE_DECRYPT_HXX__

#include <yuni/yuni.h>
#include "license.h"
#include "base64/cdecode.h"
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>

#include <yuni/core/system/cpu.h>
#include <yuni/core/system/memory.h>
#include <antares/logs.h>

#include "../config.h"
#include "keys/allkeys.hxx"

// Debug mode for the license manager
#ifndef NDEBUG
# define LICENSE_DEBUG(x)  logs.debug() << "LICENSE: " << x
#else
# define LICENSE_DEBUG(x)
# endif

# ifdef YUNI_OS_WINDOWS
#	define _WIN32_DCOM
#	include <comdef.h>
#	include <Wbemidl.h>
#   include <winsock2.h>
#	pragma comment(lib, "wbemuuid.lib")
# endif

namespace Antares {
	namespace License {
		typedef unsigned char uchar;
# ifdef YUNI_OS_WINDOWS

		static bool WideCharIntoString(Yuni::String& out, const wchar_t* wcstr)
		{
			if (not wcstr)
			{
				out.clear();
				return false;
			}
			int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wcstr, -1, nullptr, 0, nullptr, nullptr);
			if (sizeRequired <= 0)
			{
				out.clear();
				return false;
			}
			out.reserve((unsigned int)sizeRequired + 1);
			WideCharToMultiByte(CP_UTF8, 0, wcstr, -1, (char*)out.data(), sizeRequired, nullptr, nullptr);
			out.resize(((unsigned int)sizeRequired) - 1);
			return true;
		}
# endif

		template<class T>
		static const char* IndexToCString(T index)
		{
			switch (index)
			{
				// the most common case
			case 0: return nullptr;
				// possible cases
			case 1: return ".1";
			case 2: return ".2";
			case 3: return ".3";
			case 4: return ".4";
			case 5: return ".5";
			default: return ".*";
			}
		}

		template<class StringT>
		static bool Encrypt(StringT& out, const AnyString& in, const EncryptionKey& key)
		{
			EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
			EVP_CIPHER_CTX_init(ctx);

			EVP_EncryptInit(ctx, EVP_bf_cbc(), (const uchar*)key.key.c_str(), (const uchar*)key.iv.c_str());

			enum
			{
				paddingLength = encryptionKeyLength + encryptionIVLength + 8 /*arbitrary*/,
			};
			out.reserve(in.size() + paddingLength);
			int olen;
			if (EVP_EncryptUpdate(ctx, (uchar*)out.data(), &olen, (const uchar*)in.c_str(), in.size()) == 1)
			{
				int tlen;
				if (EVP_EncryptFinal(ctx, (uchar*)out.data() + olen, &tlen) == 1)
				{
					olen += tlen;
					assert((uint)olen < out.capacity());
					out.resize(olen);

					EVP_CIPHER_CTX_free(ctx);
					return true;
				}
			}

			EVP_CIPHER_CTX_free(ctx);
			out.clear();
			return false;
		}

		template<class StringT>
		static bool Decrypt(StringT& out, const AnyString& in, const EncryptionKey& key)
		{
			if (in.empty())
			{
				out.clear();
				return true;
			}

			EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
			EVP_CIPHER_CTX_init(ctx);
			EVP_DecryptInit(ctx, EVP_bf_cbc(), (const uchar*)key.key.c_str(), (const uchar*)key.iv.c_str());

			enum
			{
				paddingLength = encryptionKeyLength + encryptionIVLength + 8 /*arbitrary*/,
			};
			out.reserve(in.size() + paddingLength);
			int olen;
			if (EVP_DecryptUpdate(ctx, (uchar*)out.data(), &olen, (const uchar*)in.c_str(), in.size()) == 1)
			{
				int tlen;
				if (EVP_DecryptFinal(ctx, (uchar*)out.data() + olen, &tlen) == 1)
				{
					olen += tlen;
					assert((uint)olen < out.capacity());
					out.resize(olen);

					EVP_CIPHER_CTX_free(ctx);
					return true;
				}
			}

			EVP_CIPHER_CTX_free(ctx);
			out.clear();
			return false;
		}

		


	}
}

using namespace Antares;

namespace // anonymous
{

	/*!
	** \brief Read a host key and extract informations into a property set
	**
	** A hostkey is something like that :
	** \code
	** -----BEGIN LICENSE REQUEST KEY-----
	** UBBVAxNlnxAxSyeUGKmGVqQLiMGFIRD+P1Wrt54Lvt53MZAw/DkyMk4hDdSX+HnwVlcjHl+eviaHdVlGK
	** E0zHJK4HjGAHcR0SK5FUQ3Q4KjDYgZTaI5j6t8YitWpeoym9PKwKAmTiZlRZocZLLJ8t+sPTao+R4M5RM
	** Ykcx1diDU=-voJ6mjlvrIzOVwk3nzXlhvgKkPE9Vmh59piwCEOiWuboqlpIaJ4PQz0rhTo5dZiUFETPst
	** QsMrVrGsgCX0g7JoPdVaflsvQkljqMzu+jX2rilg52iI/pH3Bhks+FlfQdexvdzdFcVeYRzYHCKIDEFQ8
	** yvNJaWSnK/kgxtZszxMpoi9T32K1JHx2q6186l73pcQnr4+kqVxWv4Gzc9lmh+A==
	** -----END LICENSE REQUEST KEY-----
	** \endcode
	*/
	template<bool ForHostKey, class StringT>
	static bool DecodeAntaresKey(License::Properties& properties, StringT& content)
	{
		properties.clear();
		content.replace("\r", "");
		content.trim();
		content += '\n';

		if (content.size() < 10) // not really possible to have a very short hostkey
		{
			LICENSE_DEBUG("license: activation key too short");
			return false;
		}

		auto beginHostKey = content.find("-BEGIN LICENSE REQUEST KEY-");
		if (beginHostKey >= content.size())
		{
			beginHostKey = content.find("-BEGIN LICENSE ACTIVATION KEY-");
			if (beginHostKey >= content.size())
			{
				LICENSE_DEBUG("invalid header: requires BEGIN LICENSE (REQUEST|ACTIVATION) KEY");
				return false;
			}
		}
		auto startKey = content.find('\n', beginHostKey);
		if (startKey >= content.size())
			return false;
		++startKey;
		if (startKey >= content.size())
			return false;

		auto endHostKey = content.find("-END LICENSE REQUEST KEY-");
		if (endHostKey >= content.size())
		{
			endHostKey = content.find("-END LICENSE ACTIVATION KEY-");
			if (endHostKey >= content.size())
			{
				LICENSE_DEBUG("invalid header: requires END LICENSE (REQUEST|ACTIVATION) KEY");
				return false;
			}
		}
		if (endHostKey <= startKey)
			return false;

		auto endKey = content.rfind('\n', endHostKey);
		if (endKey <= startKey or 0 == endKey)
			return false;

		// removing the useless end part of the key
		content.resize(endKey);
		content.consume(startKey);

		content.replace("\n", "");
		content.replace("\r", "");

		auto separator = content.find('-');
		if (separator >= content.size() - 1 or 0 == separator)
			return false;

		// In a first time, we will extract the encrypted key (encrypted by our public RSA key, base64 encoded)
		// used to encrypt informations about the host
		Yuni::String encryptedKey;
		{
			AnyString rawEncryptedKey(content, 0, separator);
			encryptedKey.reserve(rawEncryptedKey.size());
			base64_decodestate state;
			base64_init_decodestate(&state);
			auto length = base64_decode_block(rawEncryptedKey.c_str(), rawEncryptedKey.size(), encryptedKey.data(), &state);
			if (length <= 0)
				return false;
			assert((uint) length < encryptedKey.capacity());
			encryptedKey.resize(length);
		}
		// The encrypted key is now available from encryptedKey, we have to decipher it with
		// our private key.

		// loading this private key first
		Yuni::String membio;
		if (ForHostKey)
			PRIVATE_KEY_FOR_HOSTID(membio);
		else
			PUBLIC_KEY_FOR_LICENSEKEY(membio);

		BIO* bp = BIO_new_mem_buf((void*) membio.data(), -1);
		if (not bp)
			return false;

		RSA* privkey;
		if (ForHostKey)
			privkey = PEM_read_bio_RSAPrivateKey(bp, 0, 0, 0);
		else
			privkey = PEM_read_bio_RSA_PUBKEY(bp, 0, 0, 0);

		if (not privkey)
			return false;
		BIO_free(bp);

		auto rsaSize = RSA_size(privkey);
		if (rsaSize <= 0)
		{
			RSA_free(privkey);
			return false;
		}

		Yuni::String decryptedKey;
		decryptedKey.reserve(rsaSize);

		// deciphering the
		int decryptedLen;
		if (ForHostKey)
		{
			decryptedLen = RSA_private_decrypt(encryptedKey.size(), (const unsigned char*) encryptedKey.c_str(),
				(unsigned char*) decryptedKey.data(),
				privkey, RSA_PKCS1_OAEP_PADDING);
		}
		else
		{
			decryptedLen = RSA_public_decrypt(encryptedKey.size(), (const unsigned char*) encryptedKey.c_str(),
				(unsigned char*) decryptedKey.data(),
				privkey, RSA_PKCS1_PADDING);
		}

		RSA_free(privkey);

		if (decryptedLen > 0)
		{
			decryptedKey.resize((uint) decryptedLen);
			// From now on, we have the key used to cipher informations about the host
			License::EncryptionKey msgkey;
			msgkey.key = decryptedKey;
			msgkey.iv.assign(decryptedKey, msgkey.key.size());

			// the real message to decipher
			AnyString encodedBase64(content, separator + 1, content.size() - (separator + 1));

			Yuni::String encoded;
			encoded.reserve(encodedBase64.size());
			base64_decodestate state;
			base64_init_decodestate(&state);
			auto length = base64_decode_block(encodedBase64.c_str(), encodedBase64.size(), encoded.data(), &state);
			if (length <= 0)
				return false;
			assert((uint) length < encoded.capacity());
			encoded.resize(length);


			Yuni::String informations;
			informations.reserve(encoded.size());
			if (not License::Decrypt(informations, encoded, msgkey))
				return false;

			// from now on, we have the original message !
			informations.trim(); // normalizing
			informations += '\n';

			// reading all keys => values to file our dictionary
			Yuni::String key;
			Yuni::String value;
			informations.words("\r\n", [&] (AnyString& word) -> bool
			{
				word.extractKeyValue(key, value, true);
				if (key.empty() or value.empty())
					return true;

				if (ForHostKey)
				{
					if (key.size() == 1)
					{
						if (key == 'a')
						{
							properties[(key = "antares.arch")] = value;
							return true;
						}
						if (key == 'c')
						{
							properties[(key = "cpu.count")] = value;
							return true;
						}
						if (key == 'f')
						{
							properties[(key = "cpu.frequency")] = value;
							return true;
						}
						if (key == 'k')
						{
							properties[(key = "hostid")] = value;
							return true;
						}
						if (key == 'm')
						{
							properties[(key = "memory.available")] = value;
							return true;
						}
					}
					else
					{
						if (key == "bm")
						{
							properties[(key = "motherboard.manufacturer")] = value;
							return true;
						}
						if (key == "mac")
						{
							properties[(key = "iface")] = value;
							return true;
						}

						if (key == "nm")
						{
							key = "os.name";
							if (value == "win7pro")
								properties[key] = "Windows 7 Professional";
							else if (value == "win8pro")
								properties[key] = "Windows 8 Professional";
							else if (value == "winXPpro")
								properties[key] = "Windows XP Professional";
							else if (value == "win7home")
								properties[key] = "Windows 7 Home";
							else if (value == "win8home")
								properties[key] = "Windows 8 Home";
							else if (value == "winXPhome")
								properties[key] = "Windows XP Home";
							else
								properties[key] = value;
							return true;
						}
						if (key == "os")
						{
							key = "os.type";
							if (value == "w")
								properties[key] = "Windows";
							else if (value == "n")
								properties[key] = "Linux";
							else if (value == "m")
								properties[key] = "Mac OS";
							else
								properties[key] = value;
							return true;
						}
						if (key == "osa")
						{
							properties[(key = "os.arch")] = value;
							return true;
						}
						if (key == "pk")
						{
							properties[(key = "os.service.pack")] = value;
							return true;
						}
					}
				}
				else
					properties[key] = value;

				return true; // continue next keyword
			});
			return true;
		}
		return false;
	}



} // anonymous namespace

#endif // __ANTARES_LICENSE_DECRYPT_HXX__
