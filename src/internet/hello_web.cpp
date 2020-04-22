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
#include <yuni/thread/timer.h>
#include <yuni/thread/thread.h>

#include <regex>


#include <antares/antares.h>
#include <yuni/yuni.h>
#include <yuni/uuid/uuid.h>
#include <yuni/io/file.h>
#include <yuni/datetime/timestamp.h>
#include <antares/sys/appdata.h>
#include <antares/logs.h>
#include <yuni/core/system/environment.h>
#include <yuni/core/system/suspend.h>
#include <antares/proxy/proxy.h>
#include "../internet/limits.h"

#include "license.h"
#include "licensetimer.h"
#include "base64/cencode.h"
#include "decrypt.hxx"
#include <curl/curl.h>
#include <fstream>

#include "getstarttime.h"

using namespace Yuni;
using namespace std;

#define SEP Yuni::IO::Separator


namespace Antares
{
namespace License
{


	//! Global mutex for licensing
	static Yuni::Mutex licenseMutex;

	/*!
	** \brief Global flag to determine whether the OpenSSL engine is initialized or not
	**
	** The access to this variable must be protected by \p licenseMutex
	*/
	static bool encryptionEngineInitialized = false;

	/*extern*/ bool hasLicenseServers = false;

	/*extern*/ enum Error lastError = errNone;

	/*extern*/ Yuni::Event<void ()> onDisconnect;

	/*extern*/ ProxySettings proxy;

	/*extern*/ enum Status statusOnline = stInitialize;

	/*extern*/ bool hasSimulatorAuthorization = false;
	
	/*extern*/ bool timerlaunched=false;

	vector<Yuni::String> used_Machines;

	static void launchLicenseTimer(uint tick)
	{
		if (tick>0){
			timerlaunched=true;
			static LicenseTimer* licenseTimer= new LicenseTimer(10000);
			licenseTimer->cycleCount(-1);
			licenseTimer->interval(tick * 1000); // in milliseconds
			licenseTimer->start();
		}
	}

	enum
	{
		minimalLicenseActivationKeyLength = 10,
	};





	static void ExitWithoutAsking()
	{
		exit(42);
	}





	static inline void CABundleFile(String& file)
	{
		String localAppData;
		if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
			localAppData.clear();
		
		// build file directory
		file << localAppData;
		file << SEP << ANTARES_CA_BUNDLE;
	}


	static inline bool Fcreaterootca()
	{
		String filename;
		CABundleFile(filename);

		std::remove(filename.c_str());
		ofstream rootca(filename.c_str(), ios::out | ios::trunc);

		// if the file can be opened
		if(rootca)
		{
			rootca << ANTARES_ROOT_CERT;
			rootca.close();
			return true;
		}

		return false;
	}


	static inline void Fremoverootca()
	{
		String filename;
		CABundleFile(filename);

		LICENSE_DEBUG("remove rootca");
		std::remove(filename.c_str());
	}

#ifndef NDEBUG
	// Debug mode for the license manager
	inline CURLcode curlDebugPerform(CURL* x)
	{
		auto tempname = tmpnam(NULL);
		auto pFile = fopen(tempname, "w");
		curl_easy_setopt(x, CURLOPT_VERBOSE, 1); 
		curl_easy_setopt(x, CURLOPT_STDERR, pFile); 
		auto code = curl_easy_perform(x);
		//TODO : removed because cause crashes on windows (maybe related to curl version tested (7.53.1) : does curl close file handle ?
		//fclose(pFile);
		std::ifstream filestream(tempname);
		string ligne;
		if (filestream)  // si l'ouverture a fonctionné
		{
			while (getline(filestream, ligne))
			{
				logs.debug() << ligne;
			}
		}
		filestream.close();
		remove(tempname);
		return code;
	}

# define CURL_PERFORM(x)  curlDebugPerform(x)
#else
# define CURL_PERFORM(x) curl_easy_perform(x)
# endif


	template<class PredicateT>
	static void IterateAllLicenseActivationKeys(uint version, const PredicateT& callback)
	{
		bool stop = false;
		String filename;

		String localAppData;
		if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
			localAppData.clear();

		String localAppDataAllUsers;
		if (not OperatingSystem::FindAntaresLocalAppData(localAppDataAllUsers, true))
			localAppDataAllUsers.clear();

		# ifndef YUNI_OS_WINDOWS
		// The license has not been found, continuing
		// TODO  This method should be improved and should not rely on the variable env
		String hostname;
		if (System::Environment::Read("HOSTNAME", hostname))
		{
			for (float v = version / 100.f; v > 3.9f; v -= 0.1f)
			{
				CString<64, false> vstr;
				vstr << SEP << "antares-" << v;
				vstr.trimRight('0');
				if (vstr.last() == '.')
					vstr += '0';
				vstr << '-' << hostname << ".hwb";

				if (not localAppData.empty())
				{
					filename = localAppData;
					filename += vstr;
					callback(stop, filename, false);
					if (stop)
						return;
				}
				if (not localAppDataAllUsers.empty())
				{
					filename = localAppDataAllUsers;
					filename += vstr;
					callback(stop, filename, true);
					if (stop)
						return;
				}
			}
		}
		# endif

		for (float v = version / 100.f; v > 3.9f; v -= 0.1f)
		{
			CString<64, false> vstr;
			vstr << SEP << "antares-" << v;
			vstr.trimRight('0');
			if (vstr.last() == '.')
				vstr += '0';
			vstr += ".hwb";

			if (not localAppData.empty())
			{
				filename = localAppData;
				filename += vstr;
				callback(stop, filename, false);
				if (stop)
					return;
			}
			if (not localAppDataAllUsers.empty())
			{
				filename = localAppDataAllUsers;
				filename += vstr;
				callback(stop, filename, true);
				if (stop)
					return;
			}
		}
	}


	void WriteLastError(String& message, String& errType)
	{
		String licenseErr = "Antares Internet server";
		String connectionErr = "Connection to the Internet";

		switch (lastError)
		{
			case errNone:
			{
				message << "Antares token is valid";
				return;
			}

			//License errors
			case errLSTooManyLicense:
			{
				message << "The maximum number of tokens has been reached from the Antares server.\n";
				if(used_Machines.size()>0){
					size_t used_Size=used_Machines.size();
					message<<"These users are currently using antares :\n";
					for(size_t i_used=0;i_used<used_Machines.size();i_used++){
						message<<used_Machines[i_used]<<"\n";
					}
				}
				errType << licenseErr;
				return;
			}
			case errLSOnline:
			{
				message << "Unable to connect (check your proxy settings or try later)";
				errType << licenseErr;
				return;
			}
			case errCAFileRoot:
			{
				message << "Curl root CAcertificat error";
				errType << licenseErr;
				return;
			}
			case errHostid:
			{
				message << "Invalid key";
				errType << licenseErr;
				return;
			}
			case errStart:
			{
				message << "Invalid start date";
				errType << licenseErr;
				return;
			}
			case errSession:
			{
				message << "Invalid session id";
				errType << licenseErr;
				return;
			}
			case errMac:
			{
				message << "Invalid mac address";
				errType << licenseErr;
				return;
			}
			case errToken:
			{
				message << "Invalid token number";
				errType << licenseErr;
				return;
			}
			case errProductExpired:
			{
				message << "License has expired";
				errType << licenseErr;
				return;
			}

			// Connection or server side errors
			case errLSHostDown:
			{
				message << "Impossible to connect to Antares server";
				errType << connectionErr;
				return;
			}
			case errLSPoxy:
			{
				message << "Unable to connect (check your proxy settings)";
				errType << connectionErr;
				return;
			}
			case errDBConnexion:
			{
				message << "Unable to connect (Please try later)";
				errType << connectionErr;
				return;
			}

			// Unexpected error
			default:
			{
				message << "An unexpected error has occured.\nPlease, contact the Antares Support Team";
				return;
			}
		}
	}


	


	void InitializeEncryptionEngine()
	{
		Yuni::MutexLocker locker(licenseMutex);

		if (not encryptionEngineInitialized)
		{
			// note : the order for initializing flex / openssl should not matter
			// initialize all OpenSSL-release resources
			OpenSSL_add_all_algorithms();

			// mark the license engine as initialized
			encryptionEngineInitialized = true;
		}

		if (onDisconnect.empty())
			onDisconnect.connect(& ExitWithoutAsking);
	}


	void ReleaseEncryptionEngine()
	{
		Yuni::MutexLocker locker(licenseMutex);

		if (encryptionEngineInitialized)
		{
			// note : the order for releasing flex / openssl should not matter
			// mark the license as not initialized
			encryptionEngineInitialized = false;
			// release all OpenSSL-release resources
			EVP_cleanup();
		}
	}


	bool RetrieveActivationKey(uint version, String& activationKey, String* error)
	{
		// we will try to retrieve the activation key from a mere file
		// we will iterate through all available files (from the user's home and
		// from /etc) and through all Antares versions
		IterateAllLicenseActivationKeys(version, [&] (bool& stop, const String& filename, bool /*allusers*/)
		{
			if (IO::errNone == IO::File::LoadFromFile(activationKey, filename))
			{
				// We have found something !
				// (one file matching our criteria)
				LICENSE_DEBUG("reading " << filename);

				// trivial check
				// (to not take care of pseudo-empty files)
				stop = (activationKey.size() > minimalLicenseActivationKeyLength);
			}
			else
			{
				stop = false;
				LICENSE_DEBUG("could not load " << filename);
			}
		});

		// second pseudo-check, used in the same time to detect error
		// (license file not found)
		if (activationKey.size() < minimalLicenseActivationKeyLength)
		{
			LICENSE_DEBUG("impossible to find a antares.hwb file (not enough permissions or the file is missing)");
			if (error)
				*error = "impossible to find a antares.hwb file (not enough permissions or the file is missing)";
			return false;
		}
		return true;
	}


	bool RevokeAllUserActivationKeys(uint version)
	{
		// iterating through all activation keys we can find
		// and removing them (if not dedicated to all users, where we should
		// not have enough permissions anyway)
		IterateAllLicenseActivationKeys(version, [&] (bool& /*stop*/, const String& filename, bool allusers)
		{
			if (not allusers) // do not warm global activation keys
			{
				if (IO::File::Exists(filename))
				{
					logs.info() << "  license: trying to delete " << filename;
					IO::File::Delete(filename);
				}
			}
		});
		return true;
	}


	static bool CheckAntaresLicenseValidity(uint version, String* error)
	{
		String activationKey;
		if (not RetrieveActivationKey(version, activationKey, error))
		{
			LICENSE_DEBUG("invalid activation key structure");
			return false;
		}
		if (not CheckActivationKeyValidity(version, activationKey, error))
		{
			LICENSE_DEBUG("The antares.hwb activation key is INVALID");
			return false;
		}
		return true;
	}


	bool CheckLicenseValidity(uint version, String* error)
	{
		if (error)
			error->clear();

		// Checking for Antares corporate LICENSE
		// The Antares License may contain a flag for using
		return CheckAntaresLicenseValidity(version, error);
	}


	bool CheckActivationKeyValidity(uint version, Yuni::String& activationKey, Yuni::String* error, bool ignoreLicenseServer)
	{
		if (error)
			error->clear();
		lastError = errFailed;

		if (activationKey.size() < minimalLicenseActivationKeyLength) // early detection
		{
			LICENSE_DEBUG("Activation key too small");
			return false;
		}

		Properties hostproperties;
		Properties licenseproperties;
		String tmp;

		Yuni::MutexLocker locker(licenseMutex);

		if (not RetrieveHostProperties(hostproperties, tmp))
		{
			LICENSE_DEBUG("impossible to retrieve host properties");
			return false;
		}

		if (not DecodeAntaresKey<false>(licenseproperties, activationKey))
		{
			if (error)
				*error = "invalid key";
			LICENSE_DEBUG("impossible to decode the key");
			return false;
		}

		// we should reset once we have successfully retrieved the license
		// informations
		Limits::areaCount = 0;
		Limits::thermalClusterCount = 0;
		hasLicenseServers = false;

		// global checks
		// we may have several allowed hostid or productid
		bool checkHostID = false;
		bool checkProduct = false;
		bool floatingLicenses = false;
		// customer caption, for delayed updated
		String customerCaption;
		String customerId;;

		// check if need to verify on the internet server
		if (licenseproperties.find("internet.server.check") == licenseproperties.end())
			statusOnline = stNotRequested;

		// check the solver parameter
		if (not hasSimulatorAuthorization) // we're launched from the command line
		{
			// check whether the parameter exist
			if (licenseproperties.find("stand.alone.simulator") != licenseproperties.end())
			{
				if (licenseproperties.find("solver.authorized") != licenseproperties.end() )
				{
					if (not licenseproperties["solver.authorized"].to<bool>())
					{	if (error)
							*error = "simulator permission denied";
						return false;
					}
				}
				//\todo clean this ^^
				/*if (not licenseproperties["stand.alone.simulator"].to<bool>())
				{	
					if (error)
							*error = "simulator permission denied";
						return false;
				}*/

			}
		}

		// check each the property
		foreach (auto& keyvalue, licenseproperties)
		{
			auto& key = keyvalue.first;

			if (key.empty())
				continue;

			char c = key[0];
			if (c < 'a' or c > 'z')
				continue;

			auto& value = keyvalue.second;

			switch (c)
			{
				case 'i':
					{
						if(key == "internet.server.check")
						{
							if (value.to<bool>() == true)
							{
								if (statusOnline == stInitialize)
								{
									//checkOnline = true;
									// load proxy parameters
									statusOnline = stWaiting;
									// read proxy parameters from file if it is not configured from interface
									if (not proxy.enabled)
										proxy.loadProxyFile();
								}
							}
							else
								statusOnline = stNotRequested;
							break;
						}
						if(key == "internet.server.url")
						{
							Limits::urlLicenseServer = value;
							break;
						}
						break;
					}
				case 'c':
					{
						if (key == "customer.caption")
						{
							customerCaption = value;
							customerCaption.trim();
							Limits::customerCaption = customerCaption;
						}
						else if (key == "customer.id")
						{
							customerId = value;
							customerId.trim();
							Limits::customerId = customerId;
						}
						else if (key == "customer.tick")
						{
							int tick=atoi(value.c_str());
							if (tick>0){
								Limits::tick = tick;
							}
						}
						break;
					}
				case 'e':
					{
						if (key == "expires")
						{
							if (DateTime::Now() > value.to<sint64>())
							{
								LICENSE_DEBUG("product expired");
								if (error)
									*error = "The license has expired";
								return false;
							}
							Limits::expiryDate = value;
						}
						break;
					}
				case 'h':
					{
						if (key == "host.allow")
						{
							auto& hostid = hostproperties[(tmp = "k")];
							if (value != hostid and value != "*")
							{
								LICENSE_DEBUG("checking hostid : " << hostid << ", from activation key: " << value << ", status: failed");
								continue; // can have several hostid
							}
							checkHostID = true;
							LICENSE_DEBUG("checking hostid : " << hostid << ", from activation key: " << value << ", status: success");
							Limits::hostID = hostid;//If value=* we send the hostid to the server and not the * value
						}
						break;
					}
				case 'l':
					{
						if (key == "license.servers")
						{
							if (value.to<bool>() == true)
							{
								floatingLicenses = true;
								hasLicenseServers = true;
							}
						}
						break;
					}
				case 'm':
					{
						if (key == "memory.max")
						{
							uint available = (uint) Math::Round(((double) System::Memory::Total() / 1024 / 1024 / 1024));
							if (available > value.to<uint>())
							{
								LICENSE_DEBUG("Too much available memory");
								return false;
							}
						}
						break;
					}
				case 'o':
					{
						if (key == "os.require")
						{
							if (value != "*")
							{
								# ifdef YUNI_OS_WINDOWS
								if (value != "windows")
								# else
								if (value != "linux")
								# endif
								{
									LICENSE_DEBUG("invalid operating system");
									if (error)
										*error = "invalid operating system";
									return false;
								}
							}
						}
						break;
					}
				case 'p':
					{
						if (key == "product.allow")
						{
							if (value == ANTARES_PRODUCT_UUID or value == "*")
							{
								// we may have several product authorization
								checkProduct = true;
							}
							break;
						}
						if (key == "product.version.max")
						{
							if (version > value.to<uint>())
							{
								LICENSE_DEBUG("product version too high");
								return false;
							}
							break;
						}
						if (key == "product.version.min")
						{
							if (version < value.to<uint>())
							{
								LICENSE_DEBUG("product version too low");
								return false;
							}
							break;
						}
						break;
					}
				case 's':
					{
						if (key == "study.max.areas")
						{
							Limits::areaCount = value.to<uint>();
							break;
						}
						if (key == "study.max.thermal.clusters.per.area")
						{
							Limits::thermalClusterCount = value.to<uint>();
							break;
						}
						break;
					}
				case 'v':
					{
						if (key == "vendor")
						{
							if (value != "rte")
							{
								LICENSE_DEBUG("invalid vendor");
								if (error)
									*error = "invalid vendor";
								return false;
							}
							break;
						}
						break;
					}

			} // switch first char

		} // each property

		// if check license on the server is not requested
		//! This condition should be checked first
		//if (not checkOnline)
			//statusOnline = stNotRequested;

		// if there is not product declared => not need to continue
		if (not checkProduct)
		{
			if (error)
				*error = "invalid product";
			LICENSE_DEBUG("invalid product (end of scope)");
			Limits::customerCaption = customerCaption;
			hasLicenseServers = false;
			return false;
		}

		// if there is not host id declared => we need to check floting license
		if (not checkHostID)
		{
			// we may not have verify the hostid yet, because we may have to
			// use floating licenses
			if (floatingLicenses)
			{
				if (not ignoreLicenseServer)
				{
					// updating
					Limits::customerCaption = customerCaption;
					return (lastError == errNone);
				}
				else
				{
					Limits::customerCaption = customerCaption;
					lastError = errNone;
					return true;
				}
			} // floating licenses

			// otherwise, continuing
			if (error)
				*error = "invalid host";
			LICENSE_DEBUG("invalid host (end of scope)");
			Limits::customerCaption = customerCaption;
			hasLicenseServers = false;
			lastError = errFailed;
			return false;
		}

		Limits::customerCaption = customerCaption;
		lastError = errNone;
		return true;
	}


	static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		std::string test = (char *)ptr;
		if(strstr((char *)ptr, "available"))
	  {
			statusOnline = stValidOnline;
			lastError = errNone;
	  }
	  else
	  {
		if(strstr((char *)ptr, "connexion database error"))
		{
			statusOnline = stInvalidOnline;
			lastError = errDBConnexion;
		}
		if(strstr((char *)ptr, "license expired"))
		{
			statusOnline = stInvalidOnline;
			lastError = errProductExpired;
		}
		else if(strstr((char *)ptr, "invalid host"))
		{
			statusOnline = stInvalidOnline;
			lastError = errHostid;
		}
		else if(strstr((char *)ptr, "invalid session"))
		{
			statusOnline = stInvalidOnline;
			lastError = errSession;
		}
		else if(strstr((char *)ptr, "invalid token"))
		{
			statusOnline = stInvalidOnline;
			lastError = errToken;
		}
		else if(strstr((char *)ptr, "invalid mac"))
		{
			statusOnline = stInvalidOnline;
			lastError = errMac;
		}
		else if(strstr((char *)ptr, "invalid start"))
		{
			statusOnline = stInvalidOnline;
			lastError = errStart;
		}
		else if(strstr((char *)ptr, "Too many tokens"))
		{
			statusOnline = stInvalidOnline;
			lastError = errLSTooManyLicense;
			used_Machines.clear();
			string tag_name= "<!-- Name -->";
			string::size_type tag_name_size=tag_name.size();
			string end_tag_name= "<!-- /Name -->";
			string::size_type end_tag_name_size=end_tag_name.size();
			//regex expression("(?<=(<!-- Name -->))([^<]{1,20})(?<=(<!-- /Name -->))");
			regex expression("("+tag_name+")[^<]{1,20}("+end_tag_name+")");
			string str((char *)ptr);
			smatch regexRes;
			string result;
			string::size_type size;
			string::const_iterator searchStart( str.cbegin() );
			while (regex_search(searchStart,str.cend(),regexRes,expression)) {
				result=regexRes[0];
				/*string b=m[1];
				string x=m[2];*/
				int c=regexRes.size();
				size=result.size()-(tag_name_size+end_tag_name_size);
				result=result.substr(tag_name_size,size);
				searchStart += regexRes.position() +regexRes.length();
				used_Machines.push_back(result);
		  }
		}
		else
		{
			statusOnline = stInvalidOnline;
			lastError = errLSOnline;
		}
	  }
	  return nmemb;
	}


	bool CheckOnlineLicenseValidity(uint version, bool recheck)
	{
		Yuni::MutexLocker locker(licenseMutex);


		// recheck license online if it is requested
		if (recheck)
		{
			if (statusOnline != stNotRequested)
				statusOnline = stWaiting;
		}

		switch(statusOnline)
		{
			case stInitialize:
			{
				// check the license key first;
				if (not CheckLicenseValidity(version))
					return false;

				// !here, not need break
			}
			case stWaiting:
			{
				// reintialize the status
				statusOnline = stInvalidOnline;

				// connection to proxy with curl
				CURL *curl_handle;
				CURLcode res;
				curl_global_init(CURL_GLOBAL_ALL);
				curl_handle = curl_easy_init();

				// build url
				YString url;
				char *host=curl_easy_escape(curl_handle, Limits::hostID.c_str(), Limits::hostID.length());
				url << Limits::urlLicenseServer << ANTARES_SERVER_SCRIPT_ROOT;
				// build post request
				YString postfiled;
				postfiled << "hostid=" << host;
				YString macAddress;
				firstMacAddress(macAddress);
				postfiled << "&macaddress=" << "5E:FF:56:A2:AF:15";
				int startTime= 1531094400;// Monday 9 July 2018 00:00:00
				String client = Limits::customerId; 
				postfiled << "&customerid=" << client;
				postfiled << "&tick=" << Limits::tick;
				postfiled << "&starttime=" << startTime;
				static String timestampOrig;
				if(timestampOrig.empty())
				{
				  timestampOrig= Yuni::DateTime::Now();
				}
				postfiled << "&sessionid=" << host << "_" << timestampOrig;
				postfiled << "&tokenorder="<< System::CPU::Count();
				curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
				curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postfiled.c_str());

				curl_free(host);

				// added since Curl 7.21.7
				curl_easy_setopt(curl_handle, CURLOPT_TRANSFER_ENCODING, 1);

				// set curl options
				curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
				curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

				if(Antares::License::proxy.enabled)
				{
					// build proxy parameters
					YString proxyParameters;
					proxyParameters << proxy.host << ":" << proxy.port;

					curl_easy_setopt(curl_handle, CURLOPT_HTTPPROXYTUNNEL, 1L);
					curl_easy_setopt(curl_handle, CURLOPT_PROXY, proxyParameters.c_str());
								
					if( !proxy.login.empty() )
					{
						// build passeword
						YString userPassword;
						char *user = curl_easy_escape(curl_handle, proxy.login.c_str(), proxy.login.length());

						userPassword << user;
						if( !proxy.password.empty() )
							userPassword << ":" << proxy.password;
						
						curl_easy_setopt(curl_handle, CURLOPT_PROXYUSERPWD, userPassword.c_str());
					}
				}

				curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
				
				curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 1);

				// build root CAcertificates
				if(not Fcreaterootca())
				{
					lastError = errCAFileRoot;
					return false;
				}

				String cafile;
				CABundleFile(cafile);
				curl_easy_setopt(curl_handle, CURLOPT_CAINFO, cafile.c_str());
			
				// send all data to this function
				curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
			
				// time out (second)
				curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10);

				// get the result
				res = CURL_PERFORM(curl_handle);

				// check the result
				if(res != CURLE_OK)
				{
					LICENSE_DEBUG("checking proxy failed! You need to verify your proxy");
					lastError = errLSOnline;

					// redefine error type
					if(Antares::License::proxy.enabled)
					{
						if ((res == CURLE_OPERATION_TIMEDOUT) or (res == CURLE_COULDNT_CONNECT)
						or (res == CURLE_RECV_ERROR) or (res == CURLE_COULDNT_RESOLVE_HOST)
						or (res == CURLE_COULDNT_RESOLVE_PROXY))
							lastError = errLSPoxy;
					}
					else
					{
						if (res == CURLE_COULDNT_CONNECT)
							lastError = errLSHostDown;
					}
				}

				// remove the root CAcertificates
				Fremoverootca();
				// cleanup curl stuff
				curl_easy_cleanup(curl_handle);

				//check the last error
				if (lastError != errNone or statusOnline != stValidOnline)
					return false;

				LICENSE_DEBUG("checking activation key on the server success");

				if(timerlaunched==false)
				{
					launchLicenseTimer(Limits::tick);
				}
			
				break;
			}
			case stValidOnline:
			{
				LICENSE_DEBUG("checking activation key on the server always success");
				break;
			}
			case stNotRequested:
			{
				// do nothing
				LICENSE_DEBUG("checking activation key on the server not requested");
				break;
			}
			default:
			{
				// always failed
				lastError = errLSOnline;
				return false;
			}
		}

		lastError = errNone;
		return true;
	}



} // namespace License
} // namespace Antares

