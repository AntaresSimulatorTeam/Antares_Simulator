#!/bin/sh

root=`dirname "${0}"`

headerBuilder="${root}/../../bin/yuni-extract-headers.sh"

if [ ! -f "${headerBuilder}" ]; then
	echo "The script to extract all headers from sources is missing"
	exit 1
fi

revision=`svnversion | cut -d'M' -f1 | cut -d' ' -f1`


tmpFolder="/tmp/org.libyuni.packages.sources"

file_headers_targz="${tmpFolder}/yuni-headers-r${revision}.tar.gz"
file_headers_zip="${tmpFolder}/yuni-headers-r${revision}.zip"


if [ -d "${tmpFolder}" ]; then
	rm -rf "${tmpFolder}"
fi

"${headerBuilder}" "${tmpFolder}/yuni"


echo "Archives"

echo " . Compressing '${file_headers_targz}'"
tar zcsf "${file_headers_targz}" "${tmpFolder}/yuni" 2>/dev/null
echo " . Compressing '${file_headers_zip}'"
`cd "${tmpFolder}" && zip -q -r "${file_headers_zip}" .`


echo "Moving archives..."
mv -f "${file_headers_targz}" "${root}"
mv -f "${file_headers_zip}" "${root}"

echo "Cleaning"
rm -rf "${tmpFolder}"
