/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/

/* which implementation */
#if defined(__TOS_WIN__) || defined(__WIN32__) || defined(_WIN64) || defined(_WIN32)
#	define MEMSTAT_IMPL_WINDOWS
#else
#	if defined(__linux) || defined(linux) || defined(__linux__)
#		define MEMSTAT_IMPL_LINUX
#	else
#		define MEMSTAT_IMPL_FALLBACK
#		warning Memstat: Implementation manquante pour cette platform
#	endif
#endif

/* 32 or 64 bits ? */
#if defined(__IA64__) || defined(_IA64) || defined(__amd64__) || defined(__x86_64__) || defined(_M_IA64) || defined(_WIN64)
#	 define OS_64
#else
#	 define OS_32
#endif


#ifdef MEMSTAT_IMPL_LINUX
#	include <sys/sysinfo.h>
#	include <stdio.h>
#	include <stdlib.h>
#endif

#ifdef MEMSTAT_IMPL_WINDOWS
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN 1
#	endif
#	ifndef _WIN32_WINNT /* currently mingw does not define, mingw64 does */
#		define _WIN32_WINNT 0x0500     /* for MEMORYSTATUSEX */
#	endif
#	include <windows.h>
#	include <stdio.h>
#	include <stdlib.h>
#	define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif


// Pour eviter tout conflit, il faut que ces includes restent apres
// les precedents defines
#include "memstats.h"
#include <assert.h>
#include <string.h>

/*!
** \brief Valeur par defaut pour la quantite total de memoire
**
** Cette constante est utilisee dans le cas ou il est impossible
** d'obtenir des informations sur la memoire.
*/
# define MEMORY_STATS_DEFAULT_TOTAL_MEMORY  1024 * 1024 * 1024 /* 1Go */

/*!
** \brief Valeur par defaut pour la quantite de memoire disponible
**
** Cette constante est utilisee dans le cas ou il est impossible
** d'obtenir des informations sur la memoire.
*/
# define MEMORY_STATS_DEFAULT_FREE_MEMORY   1024 * 1024 * 1024 /* 1Go */






#ifdef MEMSTAT_IMPL_FALLBACK

int MemoireStatistiques(MemoireInfo* info)
{
	/* assert */
	assert(NULL != info && "Meminfo: The given pointer must not be null");
	/* valeurs par default, implementation manquante */
	info->total = MEMORY_STATS_DEFAULT_TOTAL_MEMORY;
	info->libre = MEMORY_STATS_DEFAULT_FREE_MEMORY;
}

size_t MemoireDisponible()
{
	return MEMORY_STATS_DEFAULT_FREE_MEMORY;
}

size_t MemoireTotal()
{
	return MEMORY_STATS_DEFAULT_TOTAL_MEMORY;
}

# endif /* Fallback */






#ifdef MEMSTAT_IMPL_LINUX

/*!
** \brief Lit tous les caracters jusqu'au retour chariot ou fin de fichier
*/
static int fgetline(FILE* fp, char* s, int maxlen)
{
	int i = 0;
	char c;

	while ((c = fgetc(fp)) != EOF)
	{
		if (c == '\n')
		{
			*s = '\0';
			return i;
		}
		if (i >= maxlen)
			return i;

		*s++ = c;
		++i;
	}

	return (!i) ? EOF : i;
}


static size_t readvalue(char* line)
{
	/*
	** Exemple de contenu de /proc/meminfo :
	**
	** MemTotal:      1929228 kB
	** MemFree:         12732 kB
	** Buffers:         72176 kB
	** Cached:        1076572 kB
	** SwapCached:     151412 kB
	** Active:        1491184 kB
	** Inactive:       190832 kB
	** HighTotal:           0 kB
	** HighFree:            0 kB
	** LowTotal:      1929228 kB
	** LowFree:         12732 kB
	** SwapTotal:     2096472 kB
	** SwapFree:      1732964 kB
	** Dirty:             736 kB
	** Writeback:           0 kB
	** AnonPages:      512004 kB
	** Mapped:         702148 kB
	** Slab:           154320 kB
	** PageTables:      34712 kB
	** NFS_Unstable:        0 kB
	** Bounce:              0 kB
	** CommitLimit:   3061084 kB
	** Committed_AS:  1357596 kB
	** VmallocTotal: 34359738367 kB
	** VmallocUsed:    263492 kB
	** VmallocChunk: 34359474679 kB
	** HugePages_Total:     0
	** HugePages_Free:      0
	** HugePages_Rsvd:      0
	** Hugepagesize:     2048 kB
	*/

	const char* first;

	/* Suppression de tous les espaces du debut */
	while (*line == ' ' && *line != '\0')
		++line;
	first = line;

	/* On recherche la fin du nombre */
	while (*line != ' ' && *line != '\0')
		++line;
	/* On marque la fin du nombre comme etant une fin de chaine pour
	** conversion */
	*line = '\0';

	# ifdef OS_32
	return (size_t) atol(first) * 1024u;
	# else
	return (size_t) atoll(first) * 1024u;
	# endif
}



int MemoireStatistiques(MemoireInfo* info)
{
	char line[90];
	size_t tailleLibre = 0;
	size_t tailleCache = 0;
	size_t tailleBuffer = 0;
	int remains = 8;
	FILE *fd;

	/* La seule facon valide de recuperer ces informations est de lire dans
	** /proc/meminfo.
	** La routine sysconf (3) renvoie effectivement la memoire libre et la
	** memoire libre et la memoire totale mais ne prend pas en compte la
	** memoire consommee par le cache.
	*/
	if ((fd = fopen("/proc/meminfo", "r")))
	{
		info->total = 0;
		while (EOF != fgetline(fd, line, (int) sizeof(line)))
		{
			if (!strncmp("MemTotal:", line, (size_t) 9))
			{
				info->total = readvalue(line + 10);
				if (!(remains >> 1))
					break;
			}
			if (!strncmp("MemFree:", line, (size_t) 8))
			{
				tailleLibre = readvalue(line + 9);
				if (!(remains >> 1))
					break;
			}
			if (!strncmp("Cached:", line, (size_t) 7))
			{
				tailleCache = readvalue(line + 8);
				if (!(remains >> 1))
					break;
			}
			if (!strncmp("Buffers:", line, (size_t) 8))
			{
				tailleCache = readvalue(line + 9);
				if (!(remains >> 1))
					break;
			}
		}

		info->libre = tailleLibre + tailleCache + tailleBuffer;

		fclose(fd);
		return 0;
	}

	/* Erreur, utilisation des valeurs par defaut */
	info->total = MEMORY_STATS_DEFAULT_TOTAL_MEMORY;
	info->libre = MEMORY_STATS_DEFAULT_FREE_MEMORY;
	return 1;
}

size_t MemoireDisponible()
{
	MemoireInfo info;
	MemoireStatistiques(&info);
	return info.libre;
}

size_t MemoireTotale()
{
	/* Appel system direct, plus rapide que de parser /proc/meminfo */
	struct sysinfo s;
	return (!sysinfo(&s)) ? (s.mem_unit * s.totalram) : MEMORY_STATS_DEFAULT_TOTAL_MEMORY;
}

# endif /* Linux */




#ifdef MEMSTAT_IMPL_WINDOWS

int MemoireStatistiques(MemoireInfo* info)
{
	// Getting informations from the system
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (GlobalMemoryStatusEx(&statex))
	{
		info->total = (size_t) statex.ullTotalPhys;
		info->libre = (size_t) statex.ullAvailPhys;
		return 0;
	}

	/* Erreur, utilisation des valeurs par defaut */
	info->total = (size_t) MEMORY_STATS_DEFAULT_TOTAL_MEMORY;
	info->libre = (size_t) MEMORY_STATS_DEFAULT_FREE_MEMORY;
	return 1;
}

size_t MemoireDisponible()
{
	// Getting informations from the system
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	return (GlobalMemoryStatusEx(&statex))
		? (size_t) statex.ullAvailPhys
		: (size_t) MEMORY_STATS_DEFAULT_FREE_MEMORY;
}

size_t MemoireTotale()
{
	// Getting informations from the system
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	return (GlobalMemoryStatusEx(&statex))
		? (size_t) statex.ullTotalPhys
		: (size_t) MEMORY_STATS_DEFAULT_TOTAL_MEMORY;
}

#endif /* Windows */

