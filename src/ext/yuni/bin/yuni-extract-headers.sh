#!/bin/sh

# This script is designed to extract all headers from the sources and to copy them
# into a given folder
#
# Those headers could be distributed within an archive for example
#

if [ "x${1}" = "x" ]; then
	echo "Target folder: argument required"
	exit 1
fi


root=`dirname "${0}"`
source="${root}/../src/yuni"
target="${1}"


echo "Extracting the Yuni Headers..."
echo " . From '${source}'"
echo " . To '${target}'"
echo " . Searching..."

# Find all files
list=`cd "${source}" && find . '(' \
	'(' -name '*.h' -or -name '*.hxx' ')' \
	-and ! -path "./private*" \
	-and -type f ')' -print -depth | sort `

IFS="
"

count=0
for i in ${list}; do
	count=`expr "${count}" '+' 1`
done

echo "   ${count} headers found"


echo " . Copying..."
if [ -e "${target}" ]; then
	rm -rf "${target}"
fi

count=0
lastFolder=''

for i in ${list}; do
	count=`expr "${count}" '+' 1`
	sourceFile="${source}/${i}"
	targetFile="${target}/${i}"
	folder=`dirname "${targetFile}"`

	if [ ! "x${folder}" = "x${lastFolder}" ]; then
		# The folder has changed
		lastFolder="${folder}"
		mkdir -p "${folder}"
	fi

	cp -f "${sourceFile}" "${targetFile}"
done

