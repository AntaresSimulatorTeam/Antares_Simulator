
#
# \brief Print a message to the standard output
#
log()
{
	echo "[`date`][infos] " "$*"
}

#
# \brief Print a notice message
#
notice()
{
	echo "[`date`][1;37m[1m[notic] " "$*" "[0m"
}


#
# \brief Print an error to the standard error output
#
error()
{
	echo "[`date`][1;31m[error][0m " "$*" > /dev/stderr
}


#
# \brief Check if a program exists
# \param $1 output variable name
# \param $2 program
# \param $3 'noabort' to ignore errors
#
check_program()
{
	local result="${1}"
	local prgm="${2}"

	log "checking ${prgm}"
	local w=$(which "${prgm}" 2>/dev/null)

	if [ "${w}" = "" ]; then
		log "checking ${prgm}: <not found>"
		if [ ! "${3}" = "noabort" ]; then
			error "impossible to find the program \"${prgm}\""
			exit 1
		fi
	else
		log "checking ${prgm}: ${w}"
		eval $result="'${w}'"
	fi
}
