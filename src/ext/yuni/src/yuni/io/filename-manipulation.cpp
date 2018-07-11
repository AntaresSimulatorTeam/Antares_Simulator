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
#include "filename-manipulation.h"


namespace Yuni
{
namespace IO
{

	template<class StringT>
	static inline void NormalizeImpl(StringT& out, const AnyString& input, bool replaceSlashes)
	{
		switch (input.size())
		{
			case 0:
				{
					out.clear();
					return;
				}
			case 1:
				{
					out = input;
					if (replaceSlashes)
					{
						# ifdef YUNI_OS_WINDOWS
						out.replace('/', '\\');
						# else
						out.replace('\\', '/');
						# endif
					}
					return;
				}
		}

		// From here, we have at least 2 chars

		// Counting slashes
		uint slashes = 0;
		// An index, used at different places
		uint i = 0;
		// We will keep the position of the character after the first slash. It improves
		// a bit performances for relative filenames
		uint start = 0;

		for (; i != input.size(); ++i)
		{
			if (input[i] == '/' or input[i] == '\\')
			{
				slashes = 1;
				start = ++i;
				break;
			}
		}

		if (!slashes)
		{
			// Nothing to normalize
			out = input;
			if (replaceSlashes)
			{
				# ifdef YUNI_OS_WINDOWS
				out.replace('/', '\\');
				# else
				out.replace('\\', '/');
				# endif
			}
			return;
		}

		for (; i < input.size(); ++i)
		{
			if (input[i] == '/' or input[i] == '\\')
				++slashes;
		}

		// Initializing the output, and reserving the memory to avoid as much as possible calls to realloc
		// In the most cases, the same size than the input is the most appropriate value
		out.reserve(input.size());
		// Copying the begining of the input
		out.assign(input, start);

		// Detecting absolute paths.
		// We only know that we have at least 2 chars, and we can not assume that the input
		// is zero-terminated.
		// For performance reasons (to reduce the calls to malloc/free when pushing an element),
		// we will skip the begining if the path is absolute.
		bool isAbsolute = false;

		// Performing checks only if the first slash is near by the begining.
		if (start < 4)
		{
			if (input[1] == ':' and input.size() >= 2 and (input[2] == '\\' or input[2] == '/'))
			{
				// We have an Windows-style path, and it is absolute
				isAbsolute = true;
			}
			else
			{
				// We have an Unix-style path
				if (input[0] == '/' or input[0] == '\\')
					isAbsolute = true;
			}
		}

		// The last known good position
		uint cursor = start;
		// The number of non-relative folders, used when the path is not absolute
		// This value is used to keep the relative segments at the begining
		uint realFolderCount = 0;

		// The stack
		// PreAllocating the stack, to speed up the algoritm by avoiding numerous
		// calls to malloc/free
		struct Stack
		{
		public:
			void operator () (uint c, uint l)
			{
				cursor = c;
				length = l;
			}
		public:
			uint cursor;
			uint length;
		};
		Stack* stack = new Stack[slashes + 1]; // Ex: path/to/somewhere/on/my/hdd
		// Index on the stack
		uint count = 0;

		for (i = start; i < input.size(); ++i)
		{
			// Detecting the end of a segment
			if (input[i] == '/' or input[i] == '\\')
			{
				switch (i - cursor)
				{
					case 0:
						// A single slash. Nothing to do
						break;
					case 1:
						{
							// not the current folder `./`
							if (input[cursor] != '.')
							{
								stack[count++](cursor, 2);
								++realFolderCount;
							}
							break;
						}
					case 2:
						{
							// double dot segments
							if (input[cursor] == '.' and input[cursor + 1] == '.')
							{
								if (isAbsolute)
								{
									if (count)
										--count;
								}
								else
								{
									if (realFolderCount)
									{
										--count;
										--realFolderCount;
									}
									else
										stack[count++](cursor, 3);
								}
								break;
							}
							// * break *
							// we have a real folder, so `break` _must_ not
							// be used here.
						}
					default:
						{
							// We have encountered a standard segment
							stack[count++](cursor, i - cursor + 1);
							++realFolderCount;
						}
				}
				// Positioning the cursor to the next character
				cursor = i + 1;
			}
		}

		// Special case : The last segment is a double dot segment
		if (cursor < input.size() and input.size() - cursor == 2)
		{
			if (input[cursor] == '.' and input[cursor + 1] == '.')
			{
				if (isAbsolute)
				{
					if (count)
						--count;
				}
				else
				{
					if (realFolderCount)
						--count;
					else
						stack[count++](cursor, 2);
				}

				cursor = input.size();
			}
		}

		// Pushing all stored segments
		if (count)
		{
			for (uint j = 0; j != count; ++j)
				out.append(input.c_str() + stack[j].cursor, stack[j].length);
		}

		// Releasing the memory
		delete[] stack;

		// But it may remain a final segment
		// We know for sure that it can not be a double dot segment
		if (cursor < input.size())
		{
			if (!(input.size() - cursor == 1 and input[cursor] == '.'))
				out.append(input.c_str() + cursor, input.size() - cursor);
		}
		// Removing the trailing slash
		if (out.size() > 3)
			out.removeTrailingSlash();

		if (replaceSlashes)
		{
			# ifdef YUNI_OS_WINDOWS
			out.replace('/', '\\');
			# else
			out.replace('\\', '/');
			# endif
		}
	}




	void Normalize(String& out, const AnyString& in, bool replaceSlashes)
	{
		NormalizeImpl(out, in, replaceSlashes);
	}


	void Normalize(Clob& out, const AnyString& in, bool replaceSlashes)
	{
		NormalizeImpl(out, in, replaceSlashes);
	}


	bool IsAbsolute(const AnyString& filename)
	{
		if (not filename.empty())
		{
			// UNIX Style
			char c = filename[0];
			if (c == '/' or c == '\\')
				return true;

			// Windows Style
			if (filename.size() >= 2 and filename[1] == ':')
			{
				if (String::IsAlpha(c))
				{
					if (filename.size() == 2)
						return true;
					// obviously strictly greater than 2 (see >= 2 before)
					char d = filename[2];
					if (d == '\\' or d == '/')
						return true;
				}
			}
		}
		return false;
	}



	template<class StringT>
	static inline void ExtractFilePathImpl(StringT& out, const AnyString& path, bool systemDependant)
	{
		AnyString::size_type pos = (systemDependant)
			? path.find_last_of(IO::Constant<char>::Separator)
			: path.find_last_of(IO::Constant<char>::AllSeparators);
		if (AnyString::npos == pos)
			out.clear();
		else
			out.assign(path, pos);
	}


	void ExtractFilePath(String& out, const AnyString& path, bool systemDependant)
	{
		ExtractFilePathImpl(out, path, systemDependant);
	}


	void ExtractFilePath(Clob& out, const AnyString& path, bool systemDependant)
	{
		ExtractFilePathImpl(out, path, systemDependant);
	}


	template<class StringT>
	static inline void ExtractFileNameImpl(StringT& out, const AnyString& path, bool systemDependant)
	{
		AnyString::size_type pos = (systemDependant)
			? path.find_last_of(IO::Constant<char>::Separator)
			: path.find_last_of(IO::Constant<char>::AllSeparators);
		if (AnyString::npos == pos)
			out.clear();
		else
			out.assign(path.c_str() +  pos + 1);
	}


	void ExtractFileName(String& out, const AnyString& path, bool systemDependant)
	{
		ExtractFileNameImpl(out, path, systemDependant);
	}

	void ExtractFileName(Clob& out, const AnyString& path, bool systemDependant)
	{
		ExtractFileNameImpl(out, path, systemDependant);
	}



	template<class StringT>
	static inline void ExtractFilePathAndNameImpl(StringT& path, StringT& name, const AnyString& filename, bool systemDependant)
	{
		AnyString::size_type pos = (systemDependant)
			? filename.find_last_of(IO::Constant<char>::Separator)
			: filename.find_last_of(IO::Constant<char>::AllSeparators);

		if (AnyString::npos == pos)
		{
			path.clear();
			name.clear();
		}
		else
		{
			path.assign(filename, pos);
			name.assign(filename.c_str() +  pos + 1);
		}
	}

	void ExtractFilePathAndName(String& path, String& name, const AnyString& filename, bool systemDependant)
	{
		ExtractFilePathAndNameImpl(path, name, filename, systemDependant);
	}

	void ExtractFilePathAndName(Clob& path, Clob& name, const AnyString& filename, bool systemDependant)
	{
		ExtractFilePathAndNameImpl(path, name, filename, systemDependant);
	}




	template<class StringT>
	static inline void ExtractAbsoluteFilePathImpl(StringT& out, const AnyString& path, bool systemDependant)
	{
		String tmp;
		if (IsAbsolute(path))
		{
			ExtractFilePath(tmp, path, systemDependant);
		}
		else
		{
			String absolute;
			MakeAbsolute(absolute, path);
			ExtractFilePath(tmp, absolute, systemDependant);
		}
		Normalize(out, tmp);
	}


	void ExtractAbsoluteFilePath(String& out, const AnyString& path, bool systemDependant)
	{
		ExtractAbsoluteFilePathImpl(out, path, systemDependant);
	}


	void ExtractAbsoluteFilePath(Clob& out, const AnyString& path, bool systemDependant)
	{
		ExtractAbsoluteFilePathImpl(out, path, systemDependant);
	}



	template<class StringT>
	static inline void CanonicalizeImpl(StringT& out, const AnyString& in, const AnyString& rootpath)
	{
		if (IsAbsolute(in))
		{
			Normalize(out, in);
		}
		else
		{
			String tmp;
			if (rootpath.empty())
				MakeAbsolute(tmp, in);
			else
				MakeAbsolute(tmp, in, rootpath);
			Normalize(out, tmp);
		}
	}


	void Canonicalize(String& out, const AnyString& in, const AnyString& rootpath)
	{
		CanonicalizeImpl(out, in, rootpath);
	}


	void Canonicalize(Clob& out, const AnyString& in, const AnyString& rootpath)
	{
		CanonicalizeImpl(out, in, rootpath);
	}





} // namespace IO
} // namespace Yuni

