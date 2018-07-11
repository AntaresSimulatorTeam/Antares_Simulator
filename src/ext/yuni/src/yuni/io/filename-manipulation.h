/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "io.h"



namespace Yuni
{
namespace IO
{

	/*!
	** \brief Extract the path part of a filename
	**
	** The path part will be extracted according the system-(un)dependant path-separator
	** \code
	**    String outputString;
	**	  IO::ExtractFilePath(outputString, "/tmp/foo.txt");
	**    std::cout << outputString << std::endl; // writes `/tmp`
	** \endcode
	**
	** \param[out] out The output string
	** \param[in] path The filename
	** \param[in] systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractFilePath(String& out, const AnyString& path, bool systemDependant = false);

	/*!
	** \brief Extract the path part of a filename
	**
	** The path part will be extracted according the system-(un)dependant path-separator
	** \code
	**    String outputString;
	**	  IO::ExtractFilePath(outputString, "/tmp/foo.txt");
	**    std::cout << outputString << std::endl; // writes `/tmp`
	** \endcode
	**
	** \param[out] out The output string
	** \param[in] path The filename
	** \param[in] systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractFilePath(Clob& out, const AnyString& path, bool systemDependant = false);



	/*!
	** \brief Extract the path part of a filename and make it absolute
	**
	** The path part will be extracted according the system-(un)dependant path-separator
	** \code
	**    String outputString;
	**	  IO::ExtractAbsoluteFilePath(outputString, "../foo.txt");
	**    std::cout << outputString << std::endl;
	** \endcode
	**
	** \param[out] out The output string
	** \param[in] path The filename
	** \param[in] systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractAbsoluteFilePath(String& out, const AnyString& path, bool systemDependant = false);

	/*!
	** \brief Extract the path part of a filename and make it absolute
	**
	** The path part will be extracted according the system-(un)dependant path-separator
	** \code
	**    String outputString;
	**	  IO::ExtractAbsoluteFilePath(outputString, "../foo.txt");
	**    std::cout << outputString << std::endl;
	** \endcode
	**
	** \param[out] out The output string
	** \param[in] path The filename
	** \param[in] systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractAbsoluteFilePath(Clob& out, const AnyString& path, bool systemDependant = false);




	/*!
	** \brief Extract the bare file name
	**
	** The file name will be extracted according the last occurence
	** of the system-dependant path-separator (if systemDependant = true)
	**
	** \param[out] out        The bare filename from the original one
	** \param path            The original filename
	** \param systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractFileName(String& out, const AnyString& path, bool systemDependant = true);

	/*!
	** \brief Extract the bare file name
	**
	** The file name will be extracted according the last occurence
	** of the system-dependant path-separator (if systemDependant = true)
	**
	** \param[out] out        The bare filename from the original one
	** \param path            The original filename
	** \param systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractFileName(Clob& out, const AnyString& path, bool systemDependant = true);

	/*!
	** \brief Extract the path part and the bare file name
	**
	** \param[out] path       The path from the original one
	** \param[out] name       The bare filename from the original one
	** \param filename        The original filename
	** \param systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractFilePathAndName(String& path, String& name, const AnyString& filename, bool systemDependant = true);

	/*!
	** \brief Extract the path part and the bare file name
	**
	** \param[out] path       The path from the original one
	** \param[out] name       The bare filename from the original one
	** \param filename        The original filename
	** \param systemDependant Consider only the system-dependant path-separator
	*/
	void ExtractFilePathAndName(Clob& path, Clob& name, const AnyString& filename, bool systemDependant = true);


	/*!
	** \brief Extract the bare file name without its extension
	**
	** The file name will be extracted according the last occurence
	** of the system-dependant path-separator (if systemDependant = true).
	**
	** \param[out] out        The bare file name without its extension
	** \param path            The original file name
	** \param systemDependant Consider only the system-dependant path-separator
	*/
	template<class StringT>
	void ExtractFileNameWithoutExtension(StringT& out, const AnyString& path, bool systemDependant = true);


	/*!
	** \brief Extract the extention of a file name
	**
	** \param out Variable where the result will be appended
	** \param filename The original filename
	** \param dot True to include the dot when extracting the extension
	** \param clear True to clear `out` before processing
	** \return True if an extension has been found
	*/
	template<class StringT>
	bool ExtractExtension(StringT& out, const AnyString& filename, bool dot = true, bool clear = true);


	/*!
	** \brief Get if a path is absolute
	**
	** \param filename The path or the filename to test
	** \return True if the given filename is an absolute path, false otherwise (or empty)
	*/
	bool IsAbsolute(const AnyString& filename);


	/*!
	** \brief Get if a path is relative
	**
	** \param filename The path or the filename to test
	** \return True if the given filename is an absolute path, false otherwise (or empty)
	*/
	bool IsRelative(const AnyString& filename);


	/*!
	** \brief Make a path absolute
	**
	** The current directory will be used when the given path is not absolute.
	** \param[out] out         The variable where to write the result
	** \param      filename    The filename to make absolute
	** \param      clearBefore True to clean \p out before
	*/
	template<class StringT>
	void MakeAbsolute(StringT& out, const AnyString& filename, bool clearBefore = true);

	/*!
	** \brief Make a path absolute
	**
	** The current directory will be used when the given path is not absolute.
	** \param[out] out         The variable where to write the result
	** \param      path        The filename to make absolute
	** \param      currentPath A custom current path to use if the filename is not absolute
	** \param      clearBefore True to clean \p out before
	*/
	template<class StringT>
	void MakeAbsolute(StringT& out, const AnyString& path, const AnyString& currentPath, bool clearBefore = true);


	/*!
	** \brief Replace the extension
	**
	** \code
	** std::string s = "file.avi";
	** Yuni::IO::ReplaceExtension(s, ".mpeg");
	** std::cout << s << std::endl; // file.mpeg
	** \endcode
	**
	** \param[in,out] filename     The original filename
	** \param         newExtension The new extension (dot included, ex: `.ota`)
	** \return True if the extension has been replaced (means `found and replaced`)
	*/
	template<class StringT1, class StringT2>
	bool ReplaceExtension(StringT1& filename, const StringT2& newExtension);



	/*!
	** \brief Simplifies a path by removing navigation elements such as "." and ".." to produce a direct, well-formed path
	**
	** The input can be a Windows-style or a Unix-style path, with mixed slashes and anti-slashes.
	** This routine removes dot segments (`.` and `..`) from a given filename (when
	** possible).
	** Any final slash will be removed.
	**
	** \bug The relative filenames like "C:..\\folder1\\folder2" are not handled properly
	**
	** \param[out] out            A string (any class compliant to std::string) where to write the result
	** \param      in             A path/filename to normalize
	** \param      replaceSlashes True to replace slashes according the local OS conventions. False to keep
	**                            as it.
	*/
	void Normalize(String& out, const AnyString& in, bool replaceSlashes = true);


	/*!
	** \brief Simplifies a path by removing navigation elements such as "." and ".." to produce a direct, well-formed path
	**
	** The input can be a Windows-style or a Unix-style path, with mixed slashes and anti-slashes.
	** This routine removes dot segments (`.` and `..`) from a given filename (when
	** possible).
	** Any final slash will be removed.
	**
	** \bug The relative filenames like "C:..\\folder1\\folder2" are not handled properly
	**
	** \param[out] out            A string (any class compliant to std::string) where to write the result
	** \param      in             A path/filename to normalize
	** \param      replaceSlashes True to replace slashes according the local OS conventions. False to keep
	**                            as it.
	*/
	void Normalize(Clob& out, const AnyString& in, bool replaceSlashes = true);


	/*!
	** \brief Make the path absolute, and simplifies it by removing navigation elements such as "." and ".."
	**
	** \see Normalize()
	** \param[out] out        A string (any class compliant to std::string) where to write the result
	** \param      in         A path/filename to normalize
	** \param      rootpath   A root path to take to make the path absolute (current directory if empty)
	*/
	void Canonicalize(String& out, const AnyString& in, const AnyString& rootpath = nullptr);

	/*!
	** \brief Make the path absolute, and simplifies it by removing navigation elements such as "." and ".."
	**
	** \see Normalize()
	** \param[out] out        A string (any class compliant to std::string) where to write the result
	** \param      in         A path/filename to normalize
	** \param      rootpath   A root path to take to make the path absolute (current directory if empty)
	*/
	void Canonicalize(Clob& out, const AnyString& in, const AnyString& rootpath = nullptr);





} // namespace IO
} // namespace Yuni

# include "filename-manipulation.hxx"

