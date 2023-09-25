
#
# DevPacks Settings
#
set(DevPackSourceFolder     "${CMAKE_CURRENT_SOURCE_DIR}/../devpacks")
set(DevPackReceiptsFolder   "${CMAKE_CURRENT_SOURCE_DIR}/../devpacks/receipts")
set(DevPackRepositoryURL    "http://devpacks.libyuni.org/")
set(DevPackSourceURL        "http://devpacks.libyuni.org/downloads")
set(DevPackPrefix           "yndevpack")






# Where is unzip ?
set(DevPackSourceZIP)
if (WIN32 AND NOT CMAKE_CROSSCOMPILING)
	Find_Program(DevPackSourceZIP NAMES "unzip.exe" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
else()
	Find_Program(DevPackSourceZIP NAMES "unzip")
endif()
String(COMPARE EQUAL "${DevPackSourceZIP}" "DevPackSourceZIP-NOTFOUND" DevPackUnzipHasNotBeenFound)
if (DevPackUnzipHasNotBeenFound)
	Message(FATAL_ERROR "The program 'unzip' has not been found. It is required to ")
endif()
if (WIN32)
	YMESSAGE("unzip: ${DevPackSourceZIP}")
endif()




set(DEVPACK_OS					) # macos, windows, linux, sun
set(DEVPACK_ARCH				) # i386, ppc
set(DEVPACK_COMPILER  "unknown"	) # g++, msvc, mingw



if (CMAKE_COMPILER_IS_GNUCXX)
	set(DEVPACK_COMPILER "g++")
endif()
# Trying to find out the operating system
if (WIN32)
	set(DEVPACK_OS   "windows")
	set(DEVPACK_ARCH "i386")
	if (MINGW)
		set(DEVPACK_COMPILER "mingw")
	else()
		set(DEVPACK_COMPILER "msvc")
		if (MSVC70)
			set(DEVPACK_COMPILER "vs7")
		endif()
		if (MSVC80)
			set(DEVPACK_COMPILER "vs8")
		endif()
		if (MSVC90)
			set(DEVPACK_COMPILER "vs9")
		endif()
	endif()
else()
	execute_process(COMMAND "uname" "-p" OUTPUT_VARIABLE UNAME_P OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(TOLOWER "${UNAME_P}" UNAME_P)
	string(COMPARE EQUAL "${UNAME_P}" "i386" IsI386)
	string(COMPARE EQUAL "${UNAME_P}" "powerpc" IsPPC)
	if (IsI386)
		set(DEVPACK_ARCH "i386")
	endif()
	if (IsPPC)
		set(DEVPACK_ARCH "ppc")
	endif()
endif()

if (NOT DEVPACK_OS)
	String(TOLOWER "${CMAKE_SYSTEM_NAME}" DevPackSystemName)
	String(COMPARE EQUAL "${DevPackSystemName}" "linux" IsLinux)
	if (IsLinux)
		set(DEVPACK_OS "linux")
	endif()
	String(COMPARE EQUAL "${DevPackSystemName}" "windows" IsWindows)
	if (IsWindows)
		set(DEVPACK_OS "windows")
	endif()
	String(COMPARE EQUAL "${DevPackSystemName}" "freebsd" IsFreeBSD)
	if (IsFreeBSD)
		set(DEVPACK_OS "freebsd")
	endif()
	String(COMPARE EQUAL "${DevPackSystemName}" "darwin" IsDarwin)
	if (IsDarwin)
		set(DEVPACK_OS "macos")
	endif()
endif()
if (NOT DEVPACK_ARCH)
	set(DEVPACK_ARCH "i386")
endif()




macro(DEVPACK_LIST_CONTAINS var value)
	set(${var})
	foreach(value2 ${ARGN})
		if ("${value}" STREQUAL "${value2}")
			set(${var} TRUE)
		endif()
	endforeach()
endmacro()


#
# Check if a DevPack is installed
#
macro(DEVPACK_IS_INSTALLED var dpname dpversion dprelease dpos dparch dpcompiler dptarget)

	# Specific to the given pack
	set(DevPackFolder "${DevPackSourceFolder}/${dpname}")
	set(DevPackShortFilename "${dpname}-${dpversion}-r${dprelease}-${dpos}-${dparch}-${dpcompiler}-${dptarget}")
	# File for informations about the pack
	set(DevPackFileInfo "${DevPackFolder}/${DevPackPrefix}-${DevPackShortFilename}")
	# Title for this pack
	set(DevPackTitle "DevPack: ${dpname} (${dpversion}-r${dprelease}, ${dpos}, ${dparch}, ${dpcompiler}, ${dptarget})")
	# Is this pack already available
	set(var FALSE)
	set(DevPackReady)

	# Trying to find the appropriate file
	File(GLOB FilesFound RELATIVE "${DevPackFolder}" "${DevPackFolder}/${DevPackPrefix}-*")
	DEVPACK_LIST_CONTAINS(Contains "${DevPackPrefix}-${DevPackShortFilename}" ${FilesFound})
	if (Contains)
		file(STRINGS "${DevPackFileInfo}" DevPackInfo)
		list(FIND "${DevPackInfo}" "1" DevPackReady)
		DEVPACK_LIST_CONTAINS(DevPackReady "1" ${DevPackInfo})
		if (NOT DevPackReady)
			DEVPACK_LIST_CONTAINS(DevPackReady "ok" ${DevPackInfo})
		endif()
	endif()

	if (DevPackReady)
		set(${var} TRUE)
	endif()

endmacro()




#
# Check if a DevPack is already avialable in the `receipts` folder
#
macro(DEVPACK_IS_IN_RECEIPTS var dpname dpversion dprelease dpos dparch dpcompiler dptarget)

	# Specific to the given pack
	set(DevPackFolder "${DevPackSourceFolder}/${dpname}")
	set(DevPackShortFilename "${dpname}-${dpversion}-r${dprelease}-${dpos}-${dparch}-${dpcompiler}-${dptarget}")
	# Is this pack already available
	set(var FALSE)
	set(DevPackReady)

	# Trying to find the appropriate file
	file(GLOB FilesFound RELATIVE "${DevPackReceiptsFolder}" "${DevPackReceiptsFolder}/${DevPackShortFilename}.zip")
	DEVPACK_LIST_CONTAINS(Contains "${DevPackShortFilename}.zip" ${FilesFound})
	if (Contains)
		set(${var} TRUE)
	endif()

endmacro()






#
# Import settings from a DevPack
#
macro(DEVPACK_IMPORT dpname dpversion dprelease dpos dparch dpcompiler dptarget)

	# Specific to the given pack
	set(DevPackFolder "${DevPackSourceFolder}/${dpname}")
	set(DevPackShortFilename "${dpname}-${dpversion}-r${dprelease}-${dpos}-${dparch}-${dpcompiler}-${dptarget}")
	# File for informations about the pack
	set(DevPackFileInfo "${DevPackFolder}/${DevPackPrefix}-${DevPackShortFilename}")
	set(DevPackURL "${DevPackSourceURL}/${DevPackShortFilename}.zip")

	# Title for this pack
	set(DevPackTitle "DevPack: ${dpname} (${dpversion}-r${dprelease}, ${dpos}, ${dparch}, ${dpcompiler}, ${dptarget})")
	# Is this pack already available
	set(DevPackReady FALSE)

	DEVPACK_IS_INSTALLED(DevPackReady "${dpname}" "${dpversion}" "${dprelease}" "${dpos}" "${dparch}"
		"${dpcompiler}" "${dptarget}")

	if (NOT DevPackReady)

		YMESSAGE("   ${DevPackTitle}: Missing")
		file(MAKE_DIRECTORY "${DevPackReceiptsFolder}")
		file(MAKE_DIRECTORY "${DevPackFolder}")
		set(DevPackReceiptReady false)
		set(DevPackHasBeenDownloaded  false)

		DEVPACK_IS_IN_RECEIPTS(DevPackReceiptReady "${dpname}" "${dpversion}" "${dprelease}" "${dpos}" "${dparch}"
			"${dpcompiler}" "${dptarget}")
		if (NOT DevPackReceiptReady)
			YMESSAGE("    . Downloading ${DevPackURL}")
			set(DevPackHasBeenDownloaded  true)
			# Download the Pack

			file(DOWNLOAD "http://www.gdfslsdflsdffsdfoogle.com" "${DevPackReceiptsFolder}/${DevPackShortFilename}.zip"
				STATUS DevPackDwnlStatus)
			message(STATUS "${DevPackDwnlStatus}")
			if (NOT "${DevPackDwnlStatus}")
				message(STATUS "OK")
			else()
				message(STATUS "FAILED")
			endif()

			file(DOWNLOAD "${DevPackURL}" "${DevPackReceiptsFolder}/${DevPackShortFilename}.zip"
				STATUS DevPackDwnlStatus)
			message(STATUS "${DevPackDwnlStatus}")
			if ("${DevPackDwnlStatus}")
				DEVPACK_IS_IN_RECEIPTS(DevPackReceiptReady "${dpname}" "${dpversion}" "${dprelease}" "${dpos}"
					"${dparch}" "${dpcompiler}" "${dptarget}")
			else()
				set(DevPackReceiptReady false)
			endif()
			if (NOT DevPackReceiptReady)
				YMESSAGE("    . !! The download has failed")
				file(REMOVE "${DevPackReceiptsFolder}/${DevPackShortFilename}.zip")
			endif()
		endif()

		if (DevPackReceiptReady)
			# Execute `unzip`
			YMESSAGE("    . Extracting the receipt file")
			execute_process(COMMAND "${DevPackSourceZIP}"
				-u "${DevPackReceiptsFolder}/${DevPackShortFilename}.zip"
				-d "${DevPackFolder}"
				WORKING_DIRECTORY "${DevPackFolder}" OUTPUT_QUIET)
		endif()

		DEVPACK_IS_INSTALLED(DevPackReady "${dpname}" "${dpversion}" "${dprelease}" "${dpos}" "${dparch}"
			"${dpcompiler}" "${dptarget}")

	else()
		YMESSAGE("   ${DevPackTitle}")
	endif()

	if (NOT DevPackReady)
		YMESSAGE("")
		YMESSAGE("[!!] The installation of the DevPack `${dpname}` has failed.")
		YMESSAGE("     You can download the DevPack from this url :")
		YMESSAGE("      `${DevPackURL}`")
		YMESSAGE("     And put it into the `receipts` folder :")
		YMESSAGE("      `${DevPackReceiptsFolder}`")
		YMESSAGE("")
		YMESSAGE("[!!] If the devpack is already available in the receipts folder and you still have")
		YMESSAGE("     issues, please manually remove it and try again.")
		YMESSAGE("     Please visit `${DevPackRepositoryURL}` for all devpacks.")
		YMESSAGE("")
		message(FATAL_ERROR "Aborting now.")
	else()
		set(YUNI_CURRENT_DEVPACK_SOURCE_DIR "${DevPackFolder}/${dpversion}/r${dprelease}/${dparch}/${dpcompiler}")
		include("${DevPackFolder}/${dpversion}/r${dprelease}/${dparch}/cmake/CMakeLists-${dpname}-${dpos}-${dpcompiler}-${dptarget}.cmake")
	endif()

endmacro()






macro(DEVPACK_SMART_IMPORT dpname dpversion dprelease dptarget)
	# Import
	DEVPACK_IMPORT("${dpname}" "${dpversion}" "${dprelease}" "${DEVPACK_OS}"
		"${DEVPACK_ARCH}" "${DEVPACK_COMPILER}" "${dptarget}")
endmacro()


