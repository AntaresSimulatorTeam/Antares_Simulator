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

#include <yuni/yuni.h>
#include <antares/sys/appdata.h>
#include <yuni/core/system/environment.h>
#include "../internet/limits.h"

#include "license.h"
#include "base64/cencode.h"
#include "decrypt.hxx"

#include <fstream>

#ifdef YUNI_OS_LINUX
# include <yuni/core/dictionary.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netdb.h>
# include <ifaddrs.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ioctl.h>
# include <netinet/in.h>
# include <net/if.h>
#endif
#ifdef YUNI_OS_WINDOWS
# include <stdio.h>
# include <intrin.h>
# include <yuni/core/system/windows.hdr.h>
# include <winsock2.h>
# include <iphlpapi.h>
# pragma comment(lib, "IPHLPAPI.lib")
#endif

using namespace Yuni;




namespace Antares
{
namespace License
{


	# ifndef YUNI_OS_WINDOWS
	template<class StringT>
	static inline void RemoveProcCPUInfoKey(String& out, const StringT& key)
	{
		// we must remove all occurrences
		do
		{
			auto start = out.find(key);
			if (start < out.size())
			{
				auto end = out.find('\n', start);
				if (end < out.size())
					out.erase(start, end - start + 1);
				else
					out.resize(start);
			}
			else
				break;
		}
		while (true);
	}
	# endif


	# ifdef YUNI_OS_LINUX
	static inline void GetMACAddress(String& out, const AnyString& iface)
	{
		# ifndef SIOCGIFADDR
		// The kernel does not support the required ioctls
		(void) iface;
		return;
		# else

		int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		if (fd >= 0)
		{
			struct ifreq ifr;
			ifr.ifr_addr.sa_family = AF_INET;
			strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
			if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
			{
				if (not out.empty())
					out.append(", ", 2);
				out << iface << '=';
				out.appendFormat("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[0]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[1]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[2]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[3]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[4]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[5]));
			}
			close(fd);
		}
		# endif
	}

	static inline void ListAllMacAddressesLinux(String& out)
	{
		struct ifaddrs* ifaddr;

		if (getifaddrs(&ifaddr) == -1)
			return;

		// must be used while ifaddr is alive
		Set<AnyString>::Hash alreadyProcessed;

		for (struct ifaddrs* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (ifa->ifa_addr == NULL or ifa->ifa_addr->sa_family == AF_PACKET)
				continue;
			AnyString name = ifa->ifa_name;
			if (name != "lo" and alreadyProcessed.count(name) == 0)
			{
				alreadyProcessed.insert(name);
				GetMACAddress(out, name);
			}
		}
		freeifaddrs(ifaddr);
	}
	# endif


	# ifdef YUNI_OS_WINDOWS
	static inline void ListAllMacAddressesWindows(String& out)
	{
		IP_ADAPTER_INFO adapterInfo[32];
		DWORD dwBufLen = sizeof(adapterInfo);
		DWORD dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen);

		if (dwStatus == ERROR_SUCCESS)
		{
			// temporary string for description
			AnyString description;
			// temporary string
			char buffer[48];
			(void)::memset(buffer, '\0', sizeof(buffer));

			PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
			while (pAdapterInfo)
			{
				if (MIB_IF_TYPE_ETHERNET == pAdapterInfo->Type)
				{
					if (not out.empty())
						out.append(", ", 2);

					description = (const char*) pAdapterInfo->Description;
					description.trim();
					out << description << '=';

					uint bindex = 0;
					for (uint i = 0; i < pAdapterInfo->AddressLength and bindex < sizeof(buffer) - 4; ++i)
					{
						if (i != 0)
						{
							buffer[bindex] = ':';
							++bindex;
						}
						int v = (int)((unsigned char) pAdapterInfo->Address[i]);
						int n = sprintf_s(((char*)buffer) + bindex, 3, "%.2x", v);
						if (n <= 0)
							break;
						bindex += n; // n should be equal to 2
					}

					if (bindex != 0)
						out.append((const char*) buffer, (uint) bindex);
					else
						out += "<unknown>";
				}
				pAdapterInfo = pAdapterInfo->Next;
			}
		}
	}
	# endif


	static inline void ListAllMacAddresses(String& out)
	{
		# ifdef YUNI_OS_LINUX
		ListAllMacAddressesLinux(out);
		# endif // LINUX
		# ifdef YUNI_OS_WINDOWS
		ListAllMacAddressesWindows(out);
		# endif
	}




	bool RetrieveHostProperties(Properties& properties, String& n)
	{
		properties.clear();
		String signature;

		// common code
		{
			# ifdef YUNI_OS_WINDOWS
			signature += "Win,";
			properties[(n = "os")] = 'w';
			# else
			#	ifdef YUNI_OS_LINUX
			signature += "Nux,";
			properties[(n = "os")] = 'n';
			#	else
			#		ifdef YUNI_OS_MACOS
			signature += "mac,";
			properties[(n = "os")] = 'm';
			#		else
			signature += YUNI_OS_NAME;
			signature += ',';
			properties[(n = "os")] = YUNI_OS_NAME;
			#		endif
			#	endif
			# endif

			# ifdef YUNI_OS_32
			properties[(n = 'a')] = "32";
			# else
			properties[(n = 'a')] = "64";
			# endif

			// CPU
			uint cpucount = System::CPU::Count();
			signature << "c:" << cpucount << ',';
			properties[(n = 'c')] = cpucount;
			// Memory
			properties[(n = 'm')] = (uint) Math::Round(((double) System::Memory::Total() / (1024 * 1024 * 1024)));

			// MAC addresses, not very useful for Antares, but can be used to generate
			ListAllMacAddresses(properties[(n = "mac")]);
		}
		// end common


		# ifdef YUNI_OS_WINDOWS

		// cpufreq
		LARGE_INTEGER cpufreq;
		if (TRUE == QueryPerformanceFrequency(&cpufreq))
		{
			auto& text = properties[(n = "f")];
			text = Math::Round((double) cpufreq.QuadPart / (1000. * 1000.), 2);
			text.trimRight('0');
			text.trimRight('.');
		}

		int CPUInfo[4] = {-1, -1, -1, -1};
		__cpuid(CPUInfo, 1);
		int model = (CPUInfo[0] >> 4) & 0xf;
		int family = (CPUInfo[0] >> 8) & 0xf;
		int processorType = (CPUInfo[0] >> 12) & 0x3;
		int extendedmodel = (CPUInfo[0] >> 16) & 0xf;

		__cpuid(CPUInfo, 0x80000006);
		int cacheLineSize    = (CPUInfo[2] & 0xff);
		int nL2Associativity = (CPUInfo[2] >> 12) & 0xf;
		int cacheSizeK       = (CPUInfo[2] >> 16) & 0xffff;

		signature << model << ',' << family << ',' << processorType << ',' << extendedmodel << ',';
		signature << cacheLineSize << ',' << nL2Associativity << '.' << cacheSizeK << ';';

		bool comObjectAlreadyInit = false;
		HRESULT hres =  CoInitializeEx(0, COINIT_APARTMENTTHREADED);
		switch (hres)
		{
			case S_OK:
				break;
			case S_FALSE:
				comObjectAlreadyInit = true;
				break;
			default:
				return false;
		}

		hres =  CoInitializeSecurity(
			NULL,
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities
			NULL                         // Reserved
		);
		if (FAILED(hres))
		{
			// it seems that the code works even if this call failed...
			// if (not comObjectAlreadyInit)
			//	CoUninitialize();
			//return false;
		}

		IWbemLocator* pLoc = nullptr;

		hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID *) &pLoc);
		if (FAILED(hres))
		{
			if (not comObjectAlreadyInit)
				CoUninitialize();
			return false;
		}

		IWbemServices* pSvc = nullptr;

		hres = pLoc->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			NULL,                    // User name. NULL = current user
			NULL,                    // User password. NULL = current
			0,                       // Locale. NULL indicates current
			NULL,                    // Security flags.
			0,                       // Authority (e.g. Kerberos)
			0,                       // Context object
			&pSvc                    // pointer to IWbemServices proxy
		);

		if (FAILED(hres))
		{
			pLoc->Release();
			if (not comObjectAlreadyInit)
				CoUninitialize();
			return false;
		}

		hres = CoSetProxyBlanket(
			pSvc,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			NULL,                        // Server principal name
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities
		);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();
			if (not comObjectAlreadyInit)
				CoUninitialize();
			return false;
		}

		IEnumWbemClassObject* pEnumerator = nullptr;

		hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_OperatingSystem"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL, &pEnumerator);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();
			if (not comObjectAlreadyInit)
				CoUninitialize();
			return false;
		}

		signature.append("os:", 3);
		uint index = 0;

		String text;
		for (index = 0; pEnumerator; ++index)
		{
			ULONG uReturn = 0;
			IWbemClassObject *pclsObj;

			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
			if (0 == uReturn)
				break;

			VARIANT vtProp;

			// caption
			hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				text.trim();

				if (text.icontains("Microsoft Windows 9 Professionnel"))
					text = "win9pro";
				else if (text.icontains("Microsoft Windows 9 Professional"))
					text = "win9pro";
				else if (text.icontains("Microsoft Windows 9 Familiale"))
					text = "win9home";
				else if (text.icontains("Microsoft Windows 9 Home"))
					text = "win9home";
				else if (text.icontains("Microsoft Windows 8 Professionnel"))
					text = "win8pro";
				else if (text.icontains("Microsoft Windows 8 Professional"))
					text = "win8pro";
				else if (text.icontains("Microsoft Windows 8 Familiale"))
					text = "win8home";
				else if (text.icontains("Microsoft Windows 8 Home"))
					text = "win8home";
				if (text.icontains("Microsoft Windows 7 Professionnel"))
					text = "win7pro";
				else if (text.icontains("Microsoft Windows 7 Professional"))
					text = "win7pro";
				else if (text.icontains("Microsoft Windows 7 Familiale"))
					text = "win7home";
				else if (text.icontains("Microsoft Windows 7 Home"))
					text = "win7home";
				else if (text.icontains("Microsoft Windows XP Professionnel"))
					text = "winXPpro";
				else if (text.icontains("Microsoft Windows XP Professional"))
					text = "winXPpro";
				else if (text.icontains("Microsoft Windows XP Familiale"))
					text = "winXPhome";
				else if (text.icontains("Microsoft Windows XP Home"))
					text = "winXPhome";
				else
				{
					// Fallback - only replacing one part of the string, hoping this works
					text.replace("Microsoft ", "");
				}

				signature << "n:" << text << ',';
				properties[(n.clear() << "nm" << IndexToCString(index))] = text;
			}

			// caption
			hr = pclsObj->Get(L"OSArchitecture", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				if (text == "64 bits")
					text = "64";
				else if (text == "32 bits")
					text = "32";
				signature << "ach:" << text << ',';
				properties[(n.clear() << "osa" << IndexToCString(index))] = text;
			}

			// service pack
			hr = pclsObj->Get(L"ServicePackMajorVersion", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				properties[(n.clear() << "pk" << IndexToCString(index))] = vtProp.intVal;
				VariantClear(&vtProp);
			}

			// organization
			//hr = pclsObj->Get(L"Organization", 0, &vtProp, 0, 0);
			//if (hr == S_OK)
			//{
			//	WideCharIntoString(text, vtProp.bstrVal);
			//	VariantClear(&vtProp);
			//	properties[(n.clear() << "org" << IndexToCString(index))] = text;
			//}

			// hostname
			//hr = pclsObj->Get(L"CSName", 0, &vtProp, 0, 0);
			//if (hr == S_OK)
			//{
			//	WideCharIntoString(text, vtProp.bstrVal);
			//	VariantClear(&vtProp);
			//	properties[(n = "h")] = text;
			//}

			// license
			hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << "l:" << text << ',';
				//properties[(n.clear() << "l" << IndexToCString(index))] = text;
			}

			pclsObj->Release();
		}
		pEnumerator->Release();


		hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_BaseBoard"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL, &pEnumerator);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();
			if (not comObjectAlreadyInit)
				CoUninitialize();
			return false;
		}

		signature << "b:";

		for (index = 0; pEnumerator; ++index)
		{
			ULONG uReturn = 0;
			IWbemClassObject *pclsObj;

			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
			if (0 == uReturn)
				break;

			VARIANT vtProp;

			// caption
			hr = pclsObj->Get(L"Product", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			// motherboard serial number
			hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			// sku
			hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				//properties[(n.clear() << "bm" << IndexToCString(index))] = text;
				signature << text << ',';
			}

			// sku
			hr = pclsObj->Get(L"ConfigOptions", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ';';
			}

			pclsObj->Release();
		}
		pEnumerator->Release();


		hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t("SELECT * FROM Win32_BIOS"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL, &pEnumerator);

		if (FAILED(hres))
		{
			pSvc->Release();
			pLoc->Release();
			if (not comObjectAlreadyInit)
				CoUninitialize();
			return false;
		}

		signature.append("bs:", 3);

		for (index = 0; pEnumerator; ++index)
		{
			ULONG uReturn = 0;
			IWbemClassObject *pclsObj;

			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
			if (0 == uReturn)
				break;

			VARIANT vtProp;

			// caption
			hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			// motherboard serial number
			hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			// sku
			hr = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			// sku
			hr = pclsObj->Get(L"SoftwareElementID", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			hr = pclsObj->Get(L"Description", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			hr = pclsObj->Get(L"SMBIOSBIOSVersion", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				signature << text << ',';
			}

			hr = pclsObj->Get(L"ReleaseDate", 0, &vtProp, 0, 0);
			if (hr == S_OK)
			{
				WideCharIntoString(text, vtProp.bstrVal);
				VariantClear(&vtProp);
				if (text.size() > 8)
					text.resize(8);
				signature << text << ';';
			}

			pclsObj->Release();
		}
		pEnumerator->Release();

		pSvc->Release();
		pLoc->Release();
		if (not comObjectAlreadyInit)
			CoUninitialize();

		# else

		String text;
		// /etc/redhat-release
		IO::File::LoadFromFile(text, "/etc/centos-release");
		if (text.empty())
		{
			IO::File::LoadFromFile(text, "/etc/redhat-release");
			if (text.empty())
			{
				IO::File::LoadFromFile(text, "/etc/debian-release");
				if (text.empty())
				{
					IO::File::LoadFromFile(text, "/etc/debian-version");
					if (text.empty())
					{
						IO::File::LoadFromFile(text, "/etc/mandriva-release");
						if (text.empty())
						{
							IO::File::LoadFromFile(text, "/etc/fedora-release");
							if (text.empty())
								IO::File::LoadFromFile(text, "/etc/issue");
						}
					}
				}
			}
		}
		if (not text.empty())
		{
			auto offset = text.find('\n');
			if (offset < text.size())
				text.resize(offset);
			text.trim();
			if (not text.empty())
			{
				properties[(n = "nm")] = text;
				offset = text.find(' ');
				if (offset < text.size())
					text.resize(offset);
				if (not text.empty())
					signature << "n:" << text << ',';
			}
		}

		// /proc/cpuinfo
		IO::File::LoadFromFile(text, "/proc/cpuinfo");
		if (not text.empty())
		{
			// looking for the first cpu frequency in the list
			// it should be good enough
			auto offset = text.find("cpu MHz");
			if (offset < text.size())
			{
				offset = text.find(':', offset);
				if (offset < text.size())
				{
					offset++;
					auto end = text.find('\n', offset);
					if (end < text.size())
					{
						CString<32,false> cpufreq(text, offset, end - offset);
						auto& freq = properties[(n = "f")];
						freq = Math::Round((double) cpufreq.to<double>() / 1000., 2);
						freq.trimRight('0');
						freq.trimRight('.');
					}
				}
			}

			// looking for volative values in /proc/cpuinfo
			RemoveProcCPUInfoKey(text, "cpu MHz");
			RemoveProcCPUInfoKey(text, "bogomips");
			signature << text << ',';
		}

		//
		IO::File::LoadFromFile(text, "/proc/version");
		if (not text.empty())
		{
			bool x64 = text.contains("x86_64");
			properties[(n = "osa")] = x64 ? "64" : "32";
			auto offset = text.find('(');
			if (offset < text.size())
				text.resize(offset);
			properties[(n = "pk")] = text;
		}

		# endif
		
		//add Timestamp to the signature
		long timestamp;
		//search for the GDPR configuration file
		String GDPR_filename;

		String localAppData;
		if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
		{
			localAppData.clear();
		}


		if (not localAppData.empty())
		{
			GDPR_filename = localAppData;
		}

		GDPR_filename << Yuni::IO::Separator << "antares.hwb";
		if (IO::File::Exists(GDPR_filename))
		{
			//load the GDPR status from file
			std::ifstream ifs;

			ifs.open(GDPR_filename.to<std::string>(), std::ifstream::in);
			ifs >> timestamp;
			ifs.close();

		}
		signature << timestamp;
		
		// signing the hostid
		SHA256_CTX context;
		unsigned char md[SHA256_DIGEST_LENGTH];
		SHA256_Init(&context);
		SHA256_Update(&context, (unsigned char*) signature.c_str(), signature.size());
		SHA256_Final(md, &context);


		auto& keyentry = properties[(n = "k")];
		keyentry.reserve(SHA256_DIGEST_LENGTH * 2 + 1);

		base64_encodestate state;
		base64_init_encodestate(&state);
		auto length = base64_encode_block((const char*) md, SHA256_DIGEST_LENGTH, keyentry.data(), &state);
		length += base64_encode_blockend(keyentry.data() + length, &state);
		assert((uint)length < keyentry.capacity());
		keyentry.resize(length);
		return true;
	}



	# ifdef YUNI_OS_LINUX


		static inline void GetFirstMACAddress(String& out, const AnyString& iface)
	{
		# ifndef SIOCGIFADDR
		// The kernel does not support the required ioctls
		(void) iface;
		return;
		# else

		int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		if (fd >= 0)
		{
			struct ifreq ifr;
			ifr.ifr_addr.sa_family = AF_INET;
			strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ - 1);
			if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
			{
				if (not out.empty())
					out.append(", ", 2);
				out.appendFormat("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[0]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[1]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[2]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[3]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[4]),
					(int)((unsigned char) ifr.ifr_hwaddr.sa_data[5]));
			}
			close(fd);
		}
		# endif
	}

	static inline void firstMacAddressLinux(String& out)
	{
		struct ifaddrs* ifaddr;

		if (getifaddrs(&ifaddr) == -1)
			return;

		// must be used while ifaddr is alive
		Set<AnyString>::Hash alreadyProcessed;

		struct ifaddrs* ifa = ifaddr;
		if(ifa != NULL)
		{
			if (!(ifa->ifa_addr == NULL or ifa->ifa_addr->sa_family == AF_PACKET))
			{
				AnyString name = ifa->ifa_name;
				if (name != "lo" and alreadyProcessed.count(name) == 0)
				{
					alreadyProcessed.insert(name);
					GetFirstMACAddress(out, name);
				}
			}
		}
		freeifaddrs(ifaddr);
	}
	# endif


# ifdef YUNI_OS_WINDOWS
	static inline void firstMacAddressWindows(String& out)
	{
		IP_ADAPTER_INFO adapterInfo[32];
		DWORD dwBufLen = sizeof(adapterInfo);
		DWORD dwStatus = GetAdaptersInfo(adapterInfo, &dwBufLen);

		if (dwStatus == ERROR_SUCCESS)
		{
			// temporary string for description
			AnyString description;
			// temporary string
			char buffer[48];
			(void)::memset(buffer, '\0', sizeof(buffer));

			PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
			while (pAdapterInfo && out.empty())
			{
				if (MIB_IF_TYPE_ETHERNET == pAdapterInfo->Type)
				{

					uint bindex = 0;
					for (uint i = 0; i < pAdapterInfo->AddressLength and bindex < sizeof(buffer) - 4; ++i)
					{
						if (i != 0)
						{
							buffer[bindex] = ':';

							++bindex;
						}
						int v = (int)((unsigned char)pAdapterInfo->Address[i]);
						int n = sprintf_s(((char*)buffer) + bindex, 3, "%.2x", v);
						if (n <= 0)
							break;
						bindex += n; // n should be equal to 2
					}

					if (bindex != 0)
						out.append((const char*)buffer, (uint)bindex);
				}
				pAdapterInfo = pAdapterInfo->Next;
			}
		}
	}
# endif


	void firstMacAddress(YString& out)
	{
		# ifdef YUNI_OS_LINUX
		firstMacAddressLinux(out);
		# endif // LINUX
		# ifdef YUNI_OS_WINDOWS
		firstMacAddressWindows(out);
		# endif
	}
} // namespace License
} // namespace Antares

