#!/bin/bash


# -----------------------------------------------------------------------------

IFS="
"

root=$(cd `dirname "${0}"` && pwd -P)

. "${root}/../common.sh"
if [ ! -f "${root}/../packages.sh" ]; then
	error "Please run cmake before running this script"
	exit 1
fi
. "${root}/../packages.sh"


#
# \brief Start a new package
# \param $1 name of the package
# \param $2 version
# \param $3 revision
# \param $4 publisher
# \param $5 url
#
pkg_init()
{
	pkg_name="${1}"
	pkg_version="${2}"
	pkg_rev="${3}"
	pkg_publisher="${4}"
	pkg_url="${5}"
	notice "package ${pkg_name} [${pkg_version}.${pkg_rev}]"
	"${prgm_wipetree}" > /dev/null
	"${prgm_setuptree}"
	__pkg_cwd="${rpmtopdir}/SOURCES/${pkg_name}-${pkg_version}/"
	__pkg_ori_cwd="/"
	mkdir -p "${__pkg_cwd}"
	if [ $? -ne 0 ]; then
		error "impossible to create directory ${__pkg_cwd}"
		exit 1
	fi
	pkg_spec="${root}/${pkg_name}.spec"
	log "package ${pkg_name}: ${pkg_spec}"
	if [ ! -f "${pkg_spec}" ]; then
		error "impossible to find the spec file from ${pkg_spec}"
		exit 1
	fi

	pkg_filelist=$(mktemp)
	pkg_import=$(mktemp)
	pkg_installscript=$(mktemp)
	pkg_filecount=0
	pkg_foldercount=0

	echo "#!/bin/bash" > "${pkg_import}"
	echo "" >> "${pkg_import}"
	echo ". \"${root}/../common.sh\"" >> "${pkg_import}"
}

pkg_cwd()
{
	((pkg_foldercount++))
	__pkg_ori_cwd="${1}"
	__pkg_cwd="${rpmtopdir}/SOURCES/${pkg_name}-${pkg_version}.${pkg_rev}/${1}"
	echo "mkdir -p \"${__pkg_cwd}\"" >> "${pkg_import}"
	echo "if [ \$? -ne 0 ]; then" >> "${pkg_import}"
	echo "	error \"impossible to create directory ${__pkg_cwd}\"" >> "${pkg_import}"
	echo "	exit 1" >> "${pkg_import}"
	echo "fi" >> "${pkg_import}"
	echo "install -m 0755 -d \"\${RPM_BUILD_ROOT}/${__pkg_ori_cwd}/\"" >> "${pkg_installscript}"
}

#
# \brief Add a file
# \param $1 the file to add, relative to the `src` folder
# \param $2 Optional target filename
#
pkg_add_file()
{
	local file="${root}/../../../$1"
	if [ ! -f "${file}" ]; then
		error "pkg_add_file: impossible to find ${file}"
		exit 1
	fi
	local target="${2}"
	if [ "${target}" = "" ]; then
		target=$(basename "${1}")
	fi
	((pkg_filecount++))
	echo "${__pkg_ori_cwd}/${target}" >> "${pkg_filelist}"
	echo "install -m 0644 \"${file}\" \"\${RPM_BUILD_ROOT}/${__pkg_ori_cwd}/${target}\"" >> "${pkg_installscript}"
}

#
# \brief Add a file as an executable
# \param $1 the file to add, relative to the `src` folder
# \param $2 Optional target filename
#
pkg_add_exec()
{
	local file="${root}/../../../$1"
	if [ ! -f "${file}" ]; then
		error "pkg_add_file: impossible to find ${file}"
		exit 1
	fi
	local target="${2}"
	if [ "${target}" = "" ]; then
		target=$(basename "${1}")
	fi
	((pkg_filecount++))
	${prgm_chrpath} --delete "${file}"
	echo "${__pkg_ori_cwd}/${target}" >> "${pkg_filelist}"
	echo "install -m 0755 \"${file}\" \"\${RPM_BUILD_ROOT}/${__pkg_ori_cwd}/${target}\"" >> "${pkg_installscript}"
}



pkg_build()
{
	log "package ${pkg_name}: ${pkg_foldercount} folders"
	log "package ${pkg_name}: ${pkg_filecount} files..."
	chmod 750 "${pkg_import}"
	"${pkg_import}"

	tar=$(cd "${rpmtopdir}/SOURCES" && tar czvf "${pkg_name}-${pkg_version}.${pkg_rev}.tar.gz" \
		"${pkg_name}-${pkg_version}.${pkg_rev}")
	rm -rf "${rpmtopdir}/SOURCES/${pkg_name}-${pkg_version}.${pkg_rev}"

	log "package ${pkg_name}: please wait while building..."
	r=$(cat "${pkg_spec}" | sed "s/%%VERSION%%/${pkg_version}.${pkg_rev}/g" \
		| sed "s/%%SOURCE%%/${pkg_name}-${pkg_version}.${pkg_rev}.tar.gz/g" \
		> "${rpmtopdir}/SPECS/${pkg_name}.spec")

	echo "" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"
	echo "" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"
	echo "%install" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"
	cat "${pkg_installscript}" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"

	echo "" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"
	echo "" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"
	echo "%files" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"
	cat "${pkg_filelist}" >> "${rpmtopdir}/SPECS/${pkg_name}.spec"

	cd "${rpmtopdir}" && rpmbuild -ba "SPECS/${pkg_name}.spec" > //dev/null 2> /dev/null

	rm -f "${pkg_filelist}"
	rm -f "${pkg_import}"
	rm -f "${pkg_installscript}"

	# copying rpm
	$(cd "${rpmtopdir}/RPMS" && find . -name '*debuginfo*' -exec rm -f '{}' \;)
	local list=$(cd "${rpmtopdir}/RPMS" && find . -name '*.rpm')
	IFS="
"
	for i in "${list}"; do
		log "package ${pkg_name}: $(basename "${i}")"
		cp "${rpmtopdir}/RPMS/$i" "${root}"
	done
}






main()
{
	notice "--- Antares v${antares_version_hi}.${antares_version_lo}.${antares_rev} RPM Builder"
	log "${antares_publisher} - ${antares_website}"
	log
	log "launched from ${root}"

	check_program prgm_rpm rpm
	check_program prgm_setuptree rpmdev-setuptree
	check_program prgm_wipetree  rpmdev-wipetree
	check_program prgm_chrpath   chrpath

	# launchung setuptree, to create missing files if they don't exist
	"${prgm_setuptree}"

	rpmtopdir=$("${prgm_rpm}" --eval "%{_topdir}")
	if [ ! -d "${rpmtopdir}" ]; then
		error "rpmbuild: the folder '${rpmtopdir}' does not exist"
		exit 1
	fi
	log "rpmtopdir ${rpmtopdir}"
	log # for beauty

	local ver="${antares_version_hi}.${antares_version_lo}"

	pkg_init rte-antares "${ver}" "${antares_rev}" \
		"${antares_publisher}" \
		"${antares_website}"
	pkg_cwd "/usr/bin"
	pkg_add_exec "solver/antares-${ver}-solver"
	pkg_add_exec "solver/antares-${ver}-solver-swap"
	pkg_add_exec "analyzer/antares-${ver}-analyzer"
	pkg_add_exec "tools/finder/antares-${ver}-study-finder"
	pkg_add_exec "tools/batchrun/antares-${ver}-batchrun"
	pkg_add_exec "tools/config/antares-${ver}-config"
	pkg_add_exec "tools/updater/antares-${ver}-study-updater"

	pkg_cwd "/etc/antares/pkgs"
	echo "/usr/bin/antares-${ver}-solver" > "${root}/pkginfo-solver"
	pkg_add_file "distrib/unix/rpm/pkginfo-solver" "solver-${ver}"
	echo "/usr/bin/antares-${ver}-solver-swap" > "${root}/pkginfo-solver-swap"
	pkg_add_file "distrib/unix/rpm/pkginfo-solver-swap" "solver-swap-${ver}"
	echo "/usr/bin/antares-${ver}-analyzer" > "${root}/pkginfo-analyzer"
	pkg_add_file "distrib/unix/rpm/pkginfo-analyzer" "analyzer-${ver}"
	echo "/usr/bin/antares-${ver}-config" > "${root}/pkginfo-config"
	pkg_add_file "distrib/unix/rpm/pkginfo-config" "config-${ver}"

	pkg_build

	rm -rf "${rpmbuild}"
	${prgm_setuptree}
	log done
}


main
