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
#include "md5.h"
#include "../../../core/system/windows.hdr.h"
#include "../../../io/file/stream.h"



//! Constants for the Private::MD5::transform routine
//@{
#undef BYTE_ORDER	/* 1 = big-endian, -1 = little-endian, 0 = unknown */
#ifdef ARCH_IS_BIG_ENDIAN
#  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
#else
#  define BYTE_ORDER 0
#endif

#define T_MASK ((MD5TypeUInt32)~0)
#define T1 /* 0xd76aa478 */ (T_MASK ^ 0x28955b87)
#define T2 /* 0xe8c7b756 */ (T_MASK ^ 0x173848a9)
#define T3	0x242070db
#define T4 /* 0xc1bdceee */ (T_MASK ^ 0x3e423111)
#define T5 /* 0xf57c0faf */ (T_MASK ^ 0x0a83f050)
#define T6	0x4787c62a
#define T7 /* 0xa8304613 */ (T_MASK ^ 0x57cfb9ec)
#define T8 /* 0xfd469501 */ (T_MASK ^ 0x02b96afe)
#define T9	0x698098d8
#define T10 /* 0x8b44f7af */ (T_MASK ^ 0x74bb0850)
#define T11 /* 0xffff5bb1 */ (T_MASK ^ 0x0000a44e)
#define T12 /* 0x895cd7be */ (T_MASK ^ 0x76a32841)
#define T13	0x6b901122
#define T14 /* 0xfd987193 */ (T_MASK ^ 0x02678e6c)
#define T15 /* 0xa679438e */ (T_MASK ^ 0x5986bc71)
#define T16	0x49b40821
#define T17 /* 0xf61e2562 */ (T_MASK ^ 0x09e1da9d)
#define T18 /* 0xc040b340 */ (T_MASK ^ 0x3fbf4cbf)
#define T19	0x265e5a51
#define T20 /* 0xe9b6c7aa */ (T_MASK ^ 0x16493855)
#define T21 /* 0xd62f105d */ (T_MASK ^ 0x29d0efa2)
#define T22	0x02441453
#define T23 /* 0xd8a1e681 */ (T_MASK ^ 0x275e197e)
#define T24 /* 0xe7d3fbc8 */ (T_MASK ^ 0x182c0437)
#define T25	0x21e1cde6
#define T26 /* 0xc33707d6 */ (T_MASK ^ 0x3cc8f829)
#define T27 /* 0xf4d50d87 */ (T_MASK ^ 0x0b2af278)
#define T28	0x455a14ed
#define T29 /* 0xa9e3e905 */ (T_MASK ^ 0x561c16fa)
#define T30 /* 0xfcefa3f8 */ (T_MASK ^ 0x03105c07)
#define T31	0x676f02d9
#define T32 /* 0x8d2a4c8a */ (T_MASK ^ 0x72d5b375)
#define T33 /* 0xfffa3942 */ (T_MASK ^ 0x0005c6bd)
#define T34 /* 0x8771f681 */ (T_MASK ^ 0x788e097e)
#define T35	0x6d9d6122
#define T36 /* 0xfde5380c */ (T_MASK ^ 0x021ac7f3)
#define T37 /* 0xa4beea44 */ (T_MASK ^ 0x5b4115bb)
#define T38	0x4bdecfa9
#define T39 /* 0xf6bb4b60 */ (T_MASK ^ 0x0944b49f)
#define T40 /* 0xbebfbc70 */ (T_MASK ^ 0x4140438f)
#define T41	0x289b7ec6
#define T42 /* 0xeaa127fa */ (T_MASK ^ 0x155ed805)
#define T43 /* 0xd4ef3085 */ (T_MASK ^ 0x2b10cf7a)
#define T44	0x04881d05
#define T45 /* 0xd9d4d039 */ (T_MASK ^ 0x262b2fc6)
#define T46 /* 0xe6db99e5 */ (T_MASK ^ 0x1924661a)
#define T47	0x1fa27cf8
#define T48 /* 0xc4ac5665 */ (T_MASK ^ 0x3b53a99a)
#define T49 /* 0xf4292244 */ (T_MASK ^ 0x0bd6ddbb)
#define T50	0x432aff97
#define T51 /* 0xab9423a7 */ (T_MASK ^ 0x546bdc58)
#define T52 /* 0xfc93a039 */ (T_MASK ^ 0x036c5fc6)
#define T53	0x655b59c3
#define T54 /* 0x8f0ccc92 */ (T_MASK ^ 0x70f3336d)
#define T55 /* 0xffeff47d */ (T_MASK ^ 0x00100b82)
#define T56 /* 0x85845dd1 */ (T_MASK ^ 0x7a7ba22e)
#define T57	0x6fa87e4f
#define T58 /* 0xfe2ce6e0 */ (T_MASK ^ 0x01d3191f)
#define T59 /* 0xa3014314 */ (T_MASK ^ 0x5cfebceb)
#define T60	0x4e0811a1
#define T61 /* 0xf7537e82 */ (T_MASK ^ 0x08ac817d)
#define T62 /* 0xbd3af235 */ (T_MASK ^ 0x42c50dca)
#define T63	0x2ad7d2bb
#define T64 /* 0xeb86d391 */ (T_MASK ^ 0x14792c6e)
//@}




namespace Yuni
{
namespace Hash
{
namespace Checksum
{


	namespace // anonymous
	{

		typedef unsigned char MD5TypeByte; // 8-bit byte
		typedef uint32 MD5TypeUInt32;      // 32-bit word


		/*!
		* \brief Define the state of the MD5 Algorithm
		*/
		struct MD5TypeState
		{
			//! Message length in bits, lsw first
			MD5TypeUInt32  count[2];
			//! Digest buffer
			MD5TypeUInt32  abcd[4];
			//! Accumulate block
			MD5TypeByte  buf[64];
		};



		void md5process(MD5TypeState* pms, const MD5TypeByte* data /*[64]*/)
		{
			MD5TypeUInt32
				a = pms->abcd[0], b = pms->abcd[1],
				c = pms->abcd[2], d = pms->abcd[3];
			MD5TypeUInt32 t;
			# if BYTE_ORDER > 0
			// Define storage only for big-endian CPUs
			MD5TypeUInt32 X[16];
			# else
			// Define storage for little-endian or both types of CPUs
			MD5TypeUInt32 xbuf[16];
			const MD5TypeUInt32* X;
			# endif

			{
				# if BYTE_ORDER == 0
				// Determine dynamically whether this is a big-endian or
				// little-endian machine, since we can use a more efficient
				// algorithm on the latter.
				static const int w = 1;

				if (*((const MD5TypeByte*)&w)) /* dynamic little-endian */
				# endif
				# if BYTE_ORDER <= 0		/* little-endian */
				{
					// On little-endian machines, we can process properly aligned
					// data without copying it.
					if (!((data - (const MD5TypeByte *)0) & 3))
					{
						// data are properly aligned
						X = reinterpret_cast<const MD5TypeUInt32 *>(data);
					}
					else
					{
						// not aligned
						YUNI_MEMCPY(xbuf, 64, data, 64);
						X = xbuf;
					}
				}
				# endif
				# if BYTE_ORDER == 0
				else                   // dynamic big-endian
				# endif
				# if BYTE_ORDER >= 0   // big-endian
				{
					// On big-endian machines, we must arrange the bytes in the
					// right order.
					const MD5TypeByte* xp = data;

				#  if BYTE_ORDER == 0
					X = xbuf;       // (dynamic only)
				#  else
				#	define xbuf X   // (static only)
				#  endif
					for (uint i = 0; i != 16; ++i, xp += 4)
						xbuf[i] = static_cast<MD5TypeUInt32>(xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24));
				}
				#endif
			}

			# define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

			// Round 1
			// Let [abcd k s i] denote the operation
			// a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s)
			# define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
			# define SET(a, b, c, d, k, s, Ti) \
					t = a + F(b,c,d) + X[k] + Ti;\
					a = ROTATE_LEFT(t, s) + b
			// Do the following 16 operations.
			SET(a, b, c, d,  0,  7,  T1);
			SET(d, a, b, c,  1, 12,  T2);
			SET(c, d, a, b,  2, 17,  T3);
			SET(b, c, d, a,  3, 22,  T4);
			SET(a, b, c, d,  4,  7,  T5);
			SET(d, a, b, c,  5, 12,  T6);
			SET(c, d, a, b,  6, 17,  T7);
			SET(b, c, d, a,  7, 22,  T8);
			SET(a, b, c, d,  8,  7,  T9);
			SET(d, a, b, c,  9, 12, T10);
			SET(c, d, a, b, 10, 17, T11);
			SET(b, c, d, a, 11, 22, T12);
			SET(a, b, c, d, 12,  7, T13);
			SET(d, a, b, c, 13, 12, T14);
			SET(c, d, a, b, 14, 17, T15);
			SET(b, c, d, a, 15, 22, T16);
			# undef SET

			// Round 2
			// Let [abcd k s i] denote the operation
			// a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s)
			# define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
			# define SET(a, b, c, d, k, s, Ti)\
					t = a + G(b,c,d) + X[k] + Ti;\
					a = ROTATE_LEFT(t, s) + b
			// Do the following 16 operations
			SET(a, b, c, d,  1,  5, T17);
			SET(d, a, b, c,  6,  9, T18);
			SET(c, d, a, b, 11, 14, T19);
			SET(b, c, d, a,  0, 20, T20);
			SET(a, b, c, d,  5,  5, T21);
			SET(d, a, b, c, 10,  9, T22);
			SET(c, d, a, b, 15, 14, T23);
			SET(b, c, d, a,  4, 20, T24);
			SET(a, b, c, d,  9,  5, T25);
			SET(d, a, b, c, 14,  9, T26);
			SET(c, d, a, b,  3, 14, T27);
			SET(b, c, d, a,  8, 20, T28);
			SET(a, b, c, d, 13,  5, T29);
			SET(d, a, b, c,  2,  9, T30);
			SET(c, d, a, b,  7, 14, T31);
			SET(b, c, d, a, 12, 20, T32);
			# undef SET

			// Round 3
			// Let [abcd k s t] denote the operation
			// a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s)
			# define H(x, y, z) ((x) ^ (y) ^ (z))
			# define SET(a, b, c, d, k, s, Ti)\
					t = a + H(b,c,d) + X[k] + Ti;\
					a = ROTATE_LEFT(t, s) + b
			// Do the following 16 operations
			SET(a, b, c, d,  5,  4, T33);
			SET(d, a, b, c,  8, 11, T34);
			SET(c, d, a, b, 11, 16, T35);
			SET(b, c, d, a, 14, 23, T36);
			SET(a, b, c, d,  1,  4, T37);
			SET(d, a, b, c,  4, 11, T38);
			SET(c, d, a, b,  7, 16, T39);
			SET(b, c, d, a, 10, 23, T40);
			SET(a, b, c, d, 13,  4, T41);
			SET(d, a, b, c,  0, 11, T42);
			SET(c, d, a, b,  3, 16, T43);
			SET(b, c, d, a,  6, 23, T44);
			SET(a, b, c, d,  9,  4, T45);
			SET(d, a, b, c, 12, 11, T46);
			SET(c, d, a, b, 15, 16, T47);
			SET(b, c, d, a,  2, 23, T48);
			# undef SET

			// Round 4
			// Let [abcd k s t] denote the operation
			// a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s)
			# define I(x, y, z) ((y) ^ ((x) | ~(z)))
			# define SET(a, b, c, d, k, s, Ti)\
					t = a + I(b,c,d) + X[k] + Ti;\
					a = ROTATE_LEFT(t, s) + b
			// Do the following 16 operations
			SET(a, b, c, d,  0,  6, T49);
			SET(d, a, b, c,  7, 10, T50);
			SET(c, d, a, b, 14, 15, T51);
			SET(b, c, d, a,  5, 21, T52);
			SET(a, b, c, d, 12,  6, T53);
			SET(d, a, b, c,  3, 10, T54);
			SET(c, d, a, b, 10, 15, T55);
			SET(b, c, d, a,  1, 21, T56);
			SET(a, b, c, d,  8,  6, T57);
			SET(d, a, b, c, 15, 10, T58);
			SET(c, d, a, b,  6, 15, T59);
			SET(b, c, d, a, 13, 21, T60);
			SET(a, b, c, d,  4,  6, T61);
			SET(d, a, b, c, 11, 10, T62);
			SET(c, d, a, b,  2, 15, T63);
			SET(b, c, d, a,  9, 21, T64);
			# undef SET

			// Then perform the following additions. (That is increment each
			// of the four registers by the value it had before this block
			// was started.)
			pms->abcd[0] += a;
			pms->abcd[1] += b;
			pms->abcd[2] += c;
			pms->abcd[3] += d;
		}


		void md5ImplInit(MD5TypeState* pms)
		{
			pms->count[0] = pms->count[1] = 0;
			pms->abcd[0]  = 0x67452301;
			pms->abcd[1]  = /*0xefcdab89*/ T_MASK ^ 0x10325476;
			pms->abcd[2]  = /*0x98badcfe*/ T_MASK ^ 0x67452301;
			pms->abcd[3]  = 0x10325476;
		}


		void md5ImplAppend(MD5TypeState* pms, const MD5TypeByte* data, uint nbytes)
		{
			const MD5TypeByte* p = data;
			uint left = nbytes;
			uint offset = (pms->count[0] >> 3) & 63;
			MD5TypeUInt32 nbits = (MD5TypeUInt32)(nbytes << 3);

			if (nbytes <= 0)
				return;

			// Update the message length
			pms->count[1] += nbytes >> 29;
			pms->count[0] += nbits;
			if (pms->count[0] < nbits)
				++(pms->count[1]);

			// Process an initial partial block
			if (offset)
			{
				const uint copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

				YUNI_MEMCPY(pms->buf + offset, sizeof(pms->buf) - offset, p, static_cast<size_t>(copy));
				if (offset + copy < 64)
					return;
				p += copy;
				left -= copy;
				md5process(pms, pms->buf);
			}

			// Process full blocks
			for (; left >= 64; p += 64, left -= 64)
				md5process(pms, p);

			// Process a final partial block
			if (left)
				YUNI_MEMCPY(pms->buf, sizeof(pms->buf), p, static_cast<size_t>(left));
		}



		void md5ImplFinish(MD5TypeState* pms, MD5TypeByte digest[16])
		{
			static const MD5TypeByte pad[64] =
			{
				0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			MD5TypeByte data[8];

			// Save the length before padding
			for (int i = 0; i < 8; ++i)
				data[i] = (MD5TypeByte)(pms->count[i >> 2] >> ((i & 3) << 3));
			// Pad to 56 bytes mod 64
			md5ImplAppend(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
			// Append the length
			md5ImplAppend(pms, data, 8);
			for (int i = 0; i < 16; ++i)
				digest[i] = (MD5TypeByte)(pms->abcd[i >> 2] >> ((i & 3) << 3));
		}


		void md5DigestToString(String& s, MD5TypeByte digest[16])
		{
			s.format("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
				digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7],
				digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15]);
		}

	} // anonymous namespace








	const String& MD5::fromRawData(const void* rawdata, uint64 size)
	{
		pValue.clear();
		if (!rawdata)
			return pValue;
		if (AutoDetectNullChar == size)
			size = strlen((const char*) rawdata);
		if (!size)
			return pValue;

		MD5TypeState state;
		MD5TypeByte digest[16];

		md5ImplInit(&state);
		md5ImplAppend(&state, static_cast<const MD5TypeByte*>(rawdata), static_cast<uint>(size));
		md5ImplFinish(&state, digest);
		md5DigestToString(pValue, digest);
		return pValue;
	}


	const String& MD5::fromFile(const String& filename)
	{
		pValue.clear();
		IO::File::Stream stream;
		if (stream.open(filename))
		{
			MD5TypeState state;
			MD5TypeByte digest[16];

			md5ImplInit(&state);

			char buffer[1024];
			uint64 len = 0u;

			while (0 != (len = stream.read((char*)buffer, 1024u)))
				md5ImplAppend(&state, reinterpret_cast<const MD5TypeByte*>(buffer), static_cast<uint>(len));

			md5ImplFinish(&state, digest);
			md5DigestToString(pValue, digest);
		}
		return pValue;
	}





} // namespace Checksum
} // namespace Hash
} // namespace Yuni
