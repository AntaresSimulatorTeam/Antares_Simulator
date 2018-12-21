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
#ifndef __ANTARES_LIBS_LICENSE_H__
# define __ANTARES_LIBS_LICENSE_H__

# include <antares/antares.h>
# include <yuni/core/string.h>
# include <map>
# include <antares/study/version.h>
# include <yuni/core/event.h>
# include <antares/proxy.h>

# define ANTARES_LICENSE_GUI_SHOW_ACTIVATION_KEY  \
		do \
		{ \
			/* asking to the user for a new license */ \
			{ \
				auto* mainFrm = Antares::Forms::ApplWnd::Instance(); \
				if (mainFrm) \
				{ \
					Antares::Window::License form(mainFrm); \
					form.ShowModal(); \
					/* reset the status bar which uses informations from the status bar */ \
					mainFrm->resetDefaultStatusBarText(); \
				} \
				else \
					exit(1); \
			} \
			/* aborting if still invalid */ \
			if (not Antares::License::CheckLicenseValidity(Data::versionLatest)) \
			{ \
				Dispatcher::StudyClose(true, true); \
				return; \
			} \
		} \
		while(0)


# define ANTARES_LICENSE_GUI_SHOW_PROXY  \
		do \
		{ \
			/* asking to the user for a new proxy */ \
			{ \
				auto* mainFrm = Antares::Forms::ApplWnd::Instance(); \
				if (mainFrm) \
				{ \
					Antares::Window::LicenseCouldNotConnectToInternetServer form(mainFrm); \
					form.ShowModal(); \
					/* reset the status bar which uses informations from the status bar */ \
					mainFrm->resetDefaultStatusBarText(); \
				} \
				else \
					exit(1); \
			} \
			/* aborting if still invalid */ \
			if (not Antares::License::CheckOnlineLicenseValidity(Data::versionLatest)) \
			{ \
				Dispatcher::StudyClose(true, false); \
				return; \
			} \
		} \
		while(0)




# define ANTARES_LICENSE_GUI_CHECK_FOR_ACTIVATION_KEY_ON_LINE  \
		do \
		{ \
			/* checking the license */ \
			if (not Antares::License::CheckOnlineLicenseValidity(Data::versionLatest)) \
			{ \
				/* asking to the user for a new license */ \
				ANTARES_LICENSE_GUI_SHOW_PROXY; \
			} \
		} \
		while(0)



namespace Antares
{
namespace License
{

	void WriteLastError(Yuni::String& message, Yuni::String& errType);
		

	/*!
	** \brief Initialize the encryption engine (aka OpenSSL)
	*/
	void InitializeEncryptionEngine();

	/*!
	** \brief Release the encryption engine (aka OpenSSL)
	*/
	void ReleaseEncryptionEngine();


	/*!
	** \brief Check if the installed lincese is valid
	**
	** This method may take some time to complete, since it may
	** require a network connection to a license server (such as FlexNET).
	** It is recommended to run this method into a dedicated thread, especially
	** for GUI.
	*/
	bool CheckLicenseValidity(uint version = Antares::Data::versionLatest, Yuni::String* error = nullptr);

	/*!
	** \brief Check if an activation key is valid
	**
	** \param activationKey: A license Activation key. The variable is likely to be modified internally
	*/
	bool CheckActivationKeyValidity(uint version, YString& activationKey, YString* error = nullptr,
		bool ignoreLicenseServer = false);

	/*!
	** \brief Check if an activation key is valid on line
	**
	** \param recheck Whether need to recheck the license Activation key on the server
	*/
	bool CheckOnlineLicenseValidity(uint version = Antares::Data::versionLatest, bool recheck = false);

	/*!
	** \brief Try to retrieve the activation key from the current installation
	*/
	bool RetrieveActivationKey(uint version, YString& activationKey, YString* error = nullptr);

	/*!
	** \brief Remove all user license activation keys
	**
	** \note The global activation keys will be preserved
	** \return True if the operation succeeded
	*/
	bool RevokeAllUserActivationKeys(uint version);




	enum Error
	{
		//! No error, the license is valid
		errNone = 0,
		//! Generic error
		errFailed,
		//! The maximum number of licenses has been reached from the Internet Server (LS)
		errLSTooManyLicense,
		//! The Internet server is down \o/
		errLSHostDown,
		//! The proxy is not correct
		errLSPoxy,
		//! The License is not valid on the internet server
		errLSOnline,
		//! The license root CAcertificat error
		errCAFileRoot,
		//! The hostid is not valid
		errHostid,
		//! The product is expired
		errProductExpired,
		//! The license database connexion error
		errDBConnexion,
		//! The licence server rejected the machine start date
		errStart,
		//! The licence server rejected the session id
		errSession,
		//! The licence server rejected the mac address
		errMac,
		//! The licence server rejected the token request (NaN or <0)
		errToken,
	};



	enum Status
	{
		//! The license has not been verfied on the internet server
		stInitialize = 0,
		//! The verification on the server is not requested
		stNotRequested,
		//! The verification is running
		stWaiting,
		//! The license is validated on the Internet Server
		stValidOnline,
		//! The license is not validated in the Internet Server
		stInvalidOnline,
	};



	/*!
	** \brief Flag to determine whether the current toiken has internet servers enabled
	*/
	extern bool hasLicenseServers;

	/*!
	** \brief Last error when checking the license
	*/
	extern enum Error lastError;


	/*!
	** \brief Event triggerred when the license has been disconnected from the server
	*/
	extern Yuni::Event<void ()> onDisconnect;

	/*!
	** \brief Proxy settings
	**
	** \todo Should be located here
	*/
	extern ProxySettings proxy;

	/*!
	** \brief Status for checking the license online
	*/
	extern enum Status statusOnline;

	/*!
	** \brief Flag to determine whether we need to check the current license.
	**        This parameter should set to true when the request comes from simulator interface
	*/
	extern bool hasSimulatorAuthorization;

	extern bool timerlaunched;
		
	// ---------------------------------------------------------------------------
	// low-level routines
	// ---------------------------------------------------------------------------

	enum
	{
		encryptionKeyLength = 16,
		encryptionIVLength  = 8,
		encryptionLength    = encryptionKeyLength + encryptionIVLength
	};

	//! Key used for encryption
	typedef struct
	{
		Yuni::CString<encryptionKeyLength, false> key;
		Yuni::CString<encryptionIVLength,  false> iv;
	} EncryptionKey;

	//! Dictionary for properties
	typedef std::map<YString, YString> Properties;


	/*!
	** \brief Retrieve all properties + the signature of the current host
	**
	** \param properties map string -> string
	** \param A string for temporary uses
	** \return True if the operation succeeded, false otherwise
	*/
	bool RetrieveHostProperties(Properties& properties, YString& n);

	/*!
	** \brief Retrieve the mac address of the host
	**
	** \param out the string containing the mac address -> string
	*/
	void firstMacAddress(YString& out);







} // namespace License
} // namespace Antares

# include "licensecacert.h"

#endif // __ANTARES_LIBS_LICENSE_H__
