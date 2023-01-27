#! /bin/sh

# -- Default settings --
pkgName=""
pkgVersion=""
pkgRevision="r1"
pkgOS=""
pkgArch="`uname -m`"
pkgCompiler=""
pkgSource=""
pkgCompiler="g++"
pkgTarget=""

zip=""
zipFromUser=0
find=""
mktemp=""

targetFolder=`dirname "$0"`



#
# \brief Print the absolute path of a given path
#
# \param $1 A relative or absolute path
# \return (echo) The absolute path
#
rel2abs()
{
	# make sure file is specified
	if [ -z "$1" ]
	then
		echo "$1"
		return 1
	fi
	# already absolute case
	if [ "${1:0:1}" = "/" ] || [ "$PWD" = "/" ]
	then
		ABS=""
	else
		ABS="$PWD"
	fi
	# loop thru path
	IFS="/"
	for DIR in $1
	do
		if [ -n "$DIR" ]
		then
			if [ "$DIR" = ".." ]
			then
				ABS="${ABS%/*}"
			elif [ "$DIR" != "." ]
			then
				ABS="$ABS/$DIR"
			fi
		fi
	done
	IFS=":"
	echo "$ABS"
	return 0
}



#
# \brief Try to find all needed programs
#
checkEnv()
{
	if [ "x${find}" = "x" -o ! -x "${find}" ]; then
		find=`which find`
		if [ "x${find}" = "x" -o ! -x "${find}" ]; then
			echo "The program 'find' is missing or could not be found. Aborting now."
			exit 3
		fi
	fi
	if [ "x${mktemp}" = "x" -o ! -x "${mktemp}" ]; then
		mktemp=`which mktemp`
		if [ "x${mktemp}" = "x" -o ! -x "${mktemp}" ]; then
			echo "The program 'mktemp' is missing or could not be found. Aborting now."
			exit 3
		fi
	fi
	if [ "x${zip}" = "x" -o ! -x "${zip}" ]; then
		zip=`which zip`
		if [ "x${zip}" = "x" -o ! -x "${zip}" ]; then
			echo "The program 'zip' is missing or could not be found. Aborting now."
			exit 3
		fi
	fi
}




#
# \brief Generate a summary of all settings
#
printPkgSettings()
{
	if [ ! "x${pkgName}" = "x" ]; then
		echo " * Package: '${pkgName}'"
	fi
	if [ ! "x${pkgVersion}" = "x" ]; then
		echo " * Version: '${pkgVersion}'"
	fi
	if [ ! "x${pkgRevision}" = "x" ]; then
		echo " * Revision: '${pkgRevision}'"
	fi
	if [ ! "x${pkgOS}" = "x" ]; then
		echo " * OS: '${pkgOS}'"
	fi
	if [ ! "x${pkgArch}" = "x" ]; then
		echo " * Arch: '${pkgArch}'"
	fi
	if [ ! "x${pkgCompiler}" = "x" ]; then
		echo " * Compiler: '${pkgCompiler}'"
	fi
	if [ ! "x${pkgSource}" = "x" ]; then
		echo " * Source: '${pkgSource}'"
	fi
	if [ ! "x${pkgTarget}" = "x" ]; then
		echo " * Source: '${pkgTarget}'"
	fi
	if [ "${zipFromUser}" -eq 1 ]; then
		echo " * zip: '${zip}' (user)"
	else
		echo " * zip: '${zip}' (auto)"
	fi
}



#
# \brief Check if a folder exists
#
# If the folder does not exist, the script will abort (exit 5)
#
checkSingleFolder()
{
	if [ ! -d "${1}" ]; then
		echo "[ERROR] Invalid DevPack structure: The folder '${1}' is missing"
		exit 5
	fi
}

#
# \brief Check if a file exists
#
# If the file does not exist, the script will abort (exit 5)
#
checkSingleFile()
{
	if [ ! -f "${1}" ]; then
		echo "[ERROR] Invalid DevPack structure: The file '${1}' is missing"
		exit 5
	fi
}


#
# \brief Check the consistency of a dev pack folder
# \param $1 The folder
#
checkDevPackFolderConsistency()
{
	checkSingleFolder "${1}/${pkgVersion}/${pkgRevision}/${pkgArch}/cmake"
	cmakelist="CMakeLists-${pkgName}-${pkgOS}-${pkgCompiler}-${pkgTarget}.cmake"
	checkSingleFile "${1}/${pkgVersion}/${pkgRevision}/${pkgArch}/cmake/${cmakelist}"
	checkSingleFolder "${1}/${pkgVersion}/${pkgRevision}/${pkgArch}/${pkgCompiler}"
	#checkSingleFolder "${1}/${pkgVersion}/${pkgRevision}/${pkgArch}/${pkgCompiler}/include"
	#checkSingleFolder "${1}/${pkgVersion}/${pkgRevision}/${pkgArch}/${pkgCompiler}/lib"
}





#
# \brief Generate the Help
#
help()
{
	echo "Yuni - Package maker for external dependencies"
	echo "Usage: `basename "$0"` [<options>] <sourcedir>"
	echo "Options:"
	echo "   -n  : Name of the package (ex: '-n irrlicht', '-n lua')"
	echo "   -v  : Version of the package (ex: '-v1.4.1')"
	echo "   -r  : Revision for the package (ex: '-r2')"
	echo "   -o  : Operating system (ex: '-o macos', '-o linux', '-o windows)"
	echo "   -a  : Architecture (ex: '-a 386', '-a ppc', '-a ub')"
	echo "   -f  : Target folder for the package"
	echo "   -c  : Compiler (ex: '-c vs9', '-c mingw', '-c g++', '-c g++4.2')"
	echo "   -t  : Target (ex: '-t release', '-t debug')"
	echo "   -z  : The absolute path to the program 'zip' to use to compress (archive) files (ex: '-z/usr/bin/zip')"
	echo "   -h  : This help"
	echo
	echo "Example:"
	echo "  ./makepackage.sh -n lua -v5.1.4 -r3 ~/somewhere/on/my/disks/my-devpacks/lua"
	echo
	echo "Note: The source directory should be like this :"
	echo "  + <the source dir>"
	echo "    |- <version>"
	echo "       \- <revision>"
	echo "          \- <arch>"
	echo "             |- cmake (.cmake)"
	echo "             |- <compiler> (g++, vs9...)"
	echo "                |- include (.h)"
	echo "                \- lib"
	echo "                   |- debug/ (.a,.so,.dll,.lib)"
	echo "                   \- release/ (.a,.so,.dll,.lib)"
	echo
	exit 0  # Exits now
}




#
# \brief Operating System Auto Detection
#
operatingSystemAutoDetection()
{
	unamedata=`uname -a`
	pkgOS=""
	if [ ! "`echo "${unamedata}" | grep -i Darwin`" = "" ]; then
		pkgOS="macos" ; return
	fi
	if [ ! "`echo "${unamedata}" | grep -i Linux`" = "" ]; then
		pkgOS="linux" ; return
	fi
	if [ ! "`echo "${unamedata}" | grep -i SunOS`" = "" ]; then
		pkgOS="sun" ; return
	fi
	if [ ! "`echo "${unamedata}" | grep -i HP-UX`" = "" ]; then
		pkgOS="hpux" ; return
	fi
	if [ ! "`echo "${unamedata}" | grep -i FreeBSD`" = "" ]; then
		pkgOS="freebsd" ; return
	fi
	if [ ! "`echo "${unamedata}" | grep -i DargonFly`" = "" ]; then
		pkgOS="dragonfly" ; return
	fi
	if [ ! "`echo "${unamedata}" | grep -i Cygwin`" = "" ]; then
		pkgOS="cygwin" ; return
	fi
}

# Try to detect the OS
operatingSystemAutoDetection





# Parse all command line arguments
#
# All options
args=`getopt n:v:r:o:c:a:s:z:f:t:h $*`
# Help
if [ $? != 0 ]; then
	help
fi
# All other options
set -- $args
for i
do
	case "$i"
	in
		-h)		help; shift;;
		-n)		pkgName="$2"; shift; shift;;
		-v)		pkgVersion="$2"; shift; shift;;
		-r)		pkgRevision="r$2"; shift; shift;;
		-o)		pkgOS="$2"; shift; shift;;
		-a)		pkgArch="$2"; shift; shift;;
		-c)		pkgCompiler="$2"; shift; shift;;
		-t)		pkgTarget="$2"; shift; shift;;
		-f)		targetFolder="$2"; shift; shift;;
		-z)		zip="$2"; zipFromUser=1; shift; shift;;
        --)		shift; break;;
    esac
done

# The unmatched command line argument is devpack source folder
pkgSource="$*"

# Get the absolute path for the target folder
targetFolder=`rel2abs "${targetFolder}"`



# Check the env (all needed programs)
checkEnv



#
# --- Start the packaging ---
#
echo "Yuni - Package maker for external dependencies"
printPkgSettings





# Check the consistency of all settings
good='1'
if [ "x${pkgName}" = "x" ]; then
	echo "[ERROR] The name of the package is missing (ex: '-n irrlicht', '-n lua')"
	good='0'
fi
if [ "x${pkgVersion}" = "x" ]; then
	echo "[ERROR] The version is missing (ex: '-v1.4.1')"
	good='0'
fi
if [ "x${pkgRevision}" = "x" ]; then
	echo "[ERROR] The revision is missing (ex: '-r2')"
	good='0'
fi
if [ "x${pkgOS}" = "x" ]; then
	echo "[ERROR] The operating system is missing (ex: '-o macos', '-o linux', '-o windows')"
	good='0'
fi
if [ "x${pkgArch}" = "x" ]; then
	echo "[ERROR] The architecture is missing (ex: '-a i386', '-a ppc', '-a ub')"
	good='0'
fi
if [ "x${pkgCompiler}" = "x" ]; then
	echo "[ERROR] The compiler is missing (ex: '-c vs9', '-c mingw', '-c g++', '-c g++4.2')"
	good='0'
fi
if [ "x${pkgTarget}" = "x" ]; then
	echo "[ERROR] The target is missing (ex: '-t release', '-t debug')"
	good='0'
fi


if [ "x${pkgSource}" = "x" ]; then
	echo "[ERROR] The source directory is missing"
	good='0'
fi


if [ "${good}" -eq 0 ]; then
	echo "An error has occured. To have more informations please use type ''`basename "$0"` -h''"
	echo "Aborting."
	exit 1
fi


# The target DevPack - compressed file
target="${targetFolder}/${pkgName}-${pkgVersion}-${pkgRevision}-${pkgOS}-${pkgArch}-${pkgCompiler}-${pkgTarget}.zip"
echo " * Archive : ${target}"
if [ -f "${target}" ]; then
	echo "The archive file already exists. Aborting."
	exit 1
fi


# Check the source folder consistency
checkDevPackFolderConsistency "${pkgSource}"



# The `yndevpack-*` file
devpackfile="yndevpack-${pkgName}-${pkgVersion}-${pkgRevision}-${pkgOS}-${pkgArch}-${pkgCompiler}-${pkgTarget}"
devpack="${pkgSource}/${devpackfile}"
if [ ! -f "${devpack}" ]; then
	shouldDeleteDevPackIndex=1
else
	shouldDeleteDevPackIndex=0
fi
echo 1 > "${devpack}"

# The tmpfile is the list of files to include
tmpfile=`"${mktemp}" -t yunipackagemaker.XXXXXX` || exit 4


# && "${find}" . '(' -path ''*/${pkgVersion}/r${pkgRevision}/${pkgArch}/${pkgCompiler}/*'' -and ! -path '*/.*' -and ! -name '*.o' -and ! -name '#*' ')' > "${tmpfile}" \

# What files to include
cmakelist="CMakeLists-${pkgName}-${pkgOS}-${pkgCompiler}-${pkgTarget}.cmake"
cd "${pkgSource}" \
	&& echo "Searching files to include into the package..." \
	&& "${find}" . '(' -regex "\./${pkgVersion}/${pkgRevision}/${pkgArch}/${pkgCompiler}/.*" -and -not -name '.*' -and -not -iname '*.o' -and -not -name '#*' ')' > "${tmpfile}" \
	&& echo "./${pkgVersion}/${pkgRevision}/${pkgArch}/cmake/${cmakelist}" >> "${tmpfile}" \
	&& echo "./${devpackfile}" >> "${tmpfile}" \
	&& echo "Creating the archive... Please wait..." \
	&& "${zip}" -n .Z:.zip:.png:.gif:.snd:.mp3:.jpg:.rar:.bz2:.gz -Xyr "${target}" . -i@"${tmpfile}"
result="$?"
if [ "${result}" -eq 0 ]; then
	echo "The package is ready: '${target}'"
else
	echo "[ERROR] An error has occured. Aborting."
fi



# Removing the tmp files
rm -f "${tmpfile}"
if [ -f "${tmpfile}" ]; then
	echo "[WARNING] The temporary file should be removed: '${tmpfile}'"
fi
if [ "${shouldDeleteDevPackIndex}" -eq 1 ]; then
	rm -f "${devpack}"
fi


# Done.
echo "Done."

