#!/bin/sh

root=`dirname "${0}"`
file="${root}/allkeys.hxx"


echo "#ifndef __ANTARES_LICENSE_KEYS_HXX__" > "${file}"
echo "# define __ANTARES_LICENSE_KEYS_HXX__" >> "${file}"
echo "" >> "${file}"
echo "" >> "${file}"
echo "//! Antares product uuid" >> "${file}"
echo "# define ANTARES_PRODUCT_UUID  \"18ed9039-7d7e-47d9-951d-046656c29df3\"" >> "${file}"
echo "" >> "${file}"
echo "" >> "${file}"


rsakey2code -n PUBLIC_KEY_FOR_HOSTID      -i "${root}/key.hostkey.pub" -o "${file}"
rsakey2code -n PRIVATE_KEY_FOR_HOSTID     -i "${root}/key.hostkey.pem" -o "${file}"
rsakey2code -n PUBLIC_KEY_FOR_LICENSEKEY  -i "${root}/key.licensekey.pub" -o "${file}"
rsakey2code -n PRIVATE_KEY_FOR_LICENSEKEY -i "${root}/key.licensekey.pem" -o "${file}"


echo "" >> "${file}"
echo "" >> "${file}"
echo "#endif // __ANTARES_LICENSE_KEYS_HXX__" >> "${file}"
