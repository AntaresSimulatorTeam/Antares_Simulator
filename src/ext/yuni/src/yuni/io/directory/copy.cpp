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
#include "../io.h"
#include "../directory.h"
#include "info.h"
#include "../file.h"



namespace Yuni
{
namespace IO
{
namespace Directory
{


	namespace // anonymous
	{

		struct InfoItem
		{
			bool isFile;
			uint64  size;
			String filename;
		};
		typedef std::vector<InfoItem> List;

	} // anonymous namespace



	bool Copy(const AnyString& src, const AnyString& dst, bool recursive, bool overwrite,
		const IO::Directory::CopyOnUpdateBind& onUpdate)
	{
		// normalize paths
		String fsrc;
		IO::Normalize(fsrc, src);
		if (fsrc.empty())
			return false;

		String fdst;
		IO::Normalize(fdst, dst);

		// Adding the target folder, to create it if required
		if (not onUpdate(cpsGatheringInformation, fdst, fdst, 0, 1))
			return false;

		if (not IO::Directory::Create(fdst))
			return false;

		// The list of files to copy
		List list;
		list.reserve(512);
		// the total number of bytes to copy
		uint64 totalSize = 0;


		// get the complete list of all files to copy and all folders to create
		{
			IO::Directory::Info info(fsrc);
			if (recursive)
			{
				const IO::Directory::Info::recursive_iterator& end = info.recursive_end();
				for (IO::Directory::Info::recursive_iterator i = info.recursive_begin(); i != end; ++i)
				{
					list.resize(list.size() + 1);
					InfoItem& info = list.back();
					info.filename = i.filename();
					info.isFile   = i.isFile();
					totalSize += i.size();
					if (not onUpdate(cpsGatheringInformation, *i, *i, 0, list.size()))
						return false;
				}
			}
			else
			{
				const IO::Directory::Info::iterator& end = info.end();
				for (IO::Directory::Info::iterator i = info.begin(); i != end; ++i)
				{
					list.resize(list.size() + 1);
					InfoItem& info = list.back();
					info.filename = i.filename();
					info.isFile   = i.isFile();
					totalSize += i.size();

					if (not onUpdate(cpsGatheringInformation, i.filename(), i.filename(), 0, list.size()))
						return false;
				}
			}
		}

		if (list.empty())
			return true;


		// A temporary buffer for copying files' contents
		// 16k seems to be a good choice (better than smaller block size when used
		// in Virtual Machines)
		const uint bufferSize = 16384;
		char* const buffer = new (std::nothrow) char[bufferSize];
		if (YUNI_UNLIKELY(nullptr == buffer))
			return false;

		uint64 current = 0;
		// A temporary string
		String tmp;
		tmp.reserve(1024);

		// Streams : in the worst scenario, the last file to copy will be closed
		// at the end of this routine
		// Stream on the source file
		IO::File::Stream fromFile;
		// Stream on the target file
		IO::File::Stream toFile;


		// reduce overhead brought by `onUpdate`
		enum { maxSkip = 6 };
		uint skip = (uint) maxSkip;
		// result
		bool success = true;

		const List::const_iterator end = list.end();
		for (List::const_iterator i = list.begin(); i != end; ++i)
		{
			// alias to the current information block
			const InfoItem& info = *i;

			// Address of the target file
			tmp = fdst; // without any OS-dependant separator
			if (fsrc.size() < info.filename.size())
				tmp.append(info.filename.c_str() + fsrc.size(), info.filename.size() - fsrc.size());

			if (not info.isFile)
			{
				// The target file is actually a folder - must be created before copying its content
				if (not onUpdate(cpsCopying, info.filename, tmp, current, totalSize) or not IO::Directory::Create(tmp))
				{
					success = false;
					break;
				}
			}
			else
			{
				// The target file is a real file (and not a folder)
				// Checking first for overwritting
				if (not overwrite and IO::Exists(tmp))
					continue;

				// Try to open the source file
				// The previous opened source file will be closed here
				if (fromFile.open(info.filename, IO::OpenMode::read))
				{
					// Try to open for writing the target file
					// The previous opened target file will be closed here
					if (toFile.open(tmp, IO::OpenMode::write | IO::OpenMode::truncate))
					{
						// reading the whole source file
						uint64 numRead;
						while ((numRead = fromFile.read(buffer, bufferSize)) > 0)
						{
							// progression
							current += numRead;

							// Trying to copy the block which has just been read
							if (numRead != toFile.write((const char*)buffer, numRead))
							{
								success = false;
								break;
							}

							// Notify the user from time to time about the progression
							if (0 == --skip)
							{
								if (not onUpdate(cpsCopying, info.filename, tmp, current, totalSize))
								{
									success = false;
									break;
								}
								skip = (uint) maxSkip;
							}
						} // read
					}
					else
					{
						success = false;
						break;
					}
				}
				else
				{
					success = false;
					break;
				}
			}
		}

		delete[] buffer;

		return success;
	}





} // namespace Directory
} // namespace IO
} // namespace Yuni

