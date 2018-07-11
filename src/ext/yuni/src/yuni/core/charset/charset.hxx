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
#include "charset.h"



namespace Yuni
{
namespace Charset
{

	template <typename SrcStringT, typename DtsStringT>
	bool Converter::convert(SrcStringT& source, DtsStringT& destination, bool isLastBuffer)
	{
		// Since the source and destination length may not be the same,
		// and the algorithm to predict the destination size depends on
		// the conversion performed, and since we want to perform complete
		// conversions, we require an expandable string as destination.
		YUNI_STATIC_ASSERT(DtsStringT::expandable == 1, CharsetConverterRequiresExpandableBuffers);
		enum
		{
			DSTSZ = sizeof(typename DtsStringT::Type),
			SRCSZ = sizeof(typename SrcStringT::Type),
		};

		/*
		{
			String dbg;
			dbg.appendFormat("SRC[@%p, capa = %d, size = %d]\n",
							 source.data(),
							 source.capacityInBytes(),
							 source.sizeInBytes());
			dbg.appendFormat("DST[@%p, capa = %d, size = %d]\n",
							 destination.data(),
							 destination.capacityInBytes(),
							 destination.sizeInBytes());

			std::cout << dbg;
		}
		*/

		/*
		** Reserve at least the exact sufficient space.
		** We take the size of the source in bytes and we resize the destination
		** so that it contains at least the same number of bytes plus its original
		** size.
		*/
		destination.reserve(destination.size()
							+ (source.sizeInBytes() / DSTSZ) + 1);

		/*
		{
			// Debug this.
			std::cout << "Reserving => " 
				<< (source.sizeInBytes() / DSTSZ) + 1
				<< " more elements, for "
				<<  source.sizeInBytes()
				<< " more bytes \n";


			String dbg;
			dbg.appendFormat("SRC[@%p, capa = %d, size = %d]\n",
							 source.data(),
							 source.capacityInBytes(),
							 source.sizeInBytes());
			dbg.appendFormat("DST[@%p, capa = %d, size = %d]\n",
							 destination.data(),
							 destination.capacityInBytes(),
							 destination.sizeInBytes());

			std::cout << dbg;
		}
		*/


		// The source data buffer is taken as-is.
		char* srcData = reinterpret_cast<char *>(source.data());
		size_t srcSize = source.sizeInBytes();



		bool retry;
		do
		{
			retry = false;

			/*
			** We must take a pointer to the position just after the current
			** content of the buffer.
			**
			** We also need to compute the remaining size in the buffer:
			**  = total capacity - current size
			*/
			char* dstData = reinterpret_cast<char*>(destination.data()) + destination.sizeInBytes();
			size_t dstSize = destination.capacityInBytes() - destination.sizeInBytes();

			/*
			{
				String dbg;

				dbg.appendFormat("Before iconv() : SRC[@%p, remain = %d] DST[@%p, remain = %d]\n",
								 srcData, srcSize, dstData, dstSize);
				std::cout << dbg;
			}
			*/

			// Try a conversion pass.
			size_t result = IconvWrapper(pContext, &srcData, &srcSize, &dstData, &dstSize);

			/*
			{
				String dbg;

				dbg.appendFormat("After iconv()  : SRC[@%p, remain = %d] DST[@%p, remain = %d]\n",
								 srcData, srcSize, dstData, dstSize);
				std::cout << dbg;
			}
			*/

			/*
			** Next, resize the destination buffer to contain every character put in it
			** by iconv(). We resize it to the converted length.
			** Beware of the Memory buffer data type. Incomplete elements will be truncated,
			** so don't store UTF-8 in wchar_ts.
			**
			** This will probably destroy the destination.data(), so we must reset it at the beginning.
			*/
			destination.resize((dstData - reinterpret_cast<char *>(destination.data())) / DSTSZ);


			if ((size_t)-1 == result)
			{
				// Now we will treat the common error cases.
				switch (errno)
				{
					case E2BIG:
						/*
						** The destination buffer is too small, we must augment it.
						** For the time being, we augment the buffer by the size of
						** the original source buffer.
						**
						** The destination has been resized, so the new size() includes
						** the already converted data.
						*/
						destination.reserve(destination.size()
											+ (source.sizeInBytes() / DSTSZ) + 1);
						retry = true;
						break;
					case EILSEQ:
						// Fixme: handle illegal sequences
						break;
					case EINVAL:
						// Fixme: handle incomplete sequences differently depending
						// on the fact that this is the last part of the conversion
						break;
					default:
						// Fixme: handle unknown error codes
						break;
				}
			}
		} while (retry);

		/*
		** Consume the fully converted elements from the source buffer.
		** The incompletely consumed elements should not be problematic, i think.
		** They would result from incoherences in the input types and encodings
		** (utf8 string in wchar_t memory buffer for example).
		** Anyways, this is a lead in case you have a strange bug.
		*/
		source.consume((srcData - reinterpret_cast<char *>(source.data())) / SRCSZ);

		return true;
	}


	inline int Converter::lastError() const
	{
		return pLastError;
	}


	inline bool Converter::valid() const
	{
		return ((void*)-1 != pContext);
	}




} // namespace Charset
} // namespace Yuni
