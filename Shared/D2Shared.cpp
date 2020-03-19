#include "D2Shared.hpp"
#include <cstdio>
#include <cstdarg>
#include <string>
#include <time.h>

namespace D2Lib
{
	//////////////////////////////////////////////////
	//
	// String Functions

	/*
	*	Compares two strings for case-insensitive equality, with a limited length.
	*/
	int stricmpn(const char* s1, const char* s2, int n)
	{
		int		c1, c2;

		do {
			c1 = *s1++;
			c2 = *s2++;

			if (!n--) {
				return 0;		// strings are equal until end point
			}

			if (c1 != c2) {
				if (c1 >= 'a' && c1 <= 'z')
				{
					if (c1 - 'a' == c2 - 'A')
					{
						continue; // not actually different (case insensitivity)
					}
				}
				return c1 < c2 ? -1 : 1;
			}
		} while (c1);

		return 0;		// strings are equal
	}

	/*
	*	Compares two strings for case-insensitive inequality, with an unlimited length.
	*	@return:
	*		-1 if s1 (first argument) comes before s2 lexicographically
	*		0 if the strings are lexicographically identical
	*		1 if s1 (first argument) comes after s2 lexicographically
	*/
	int stricmp(const char* s1, const char* s2)
	{
		return (s1 && s2) ? stricmpn(s1, s2, 99999) : -1;
	}

	/*
	*	Safe strncpy that assures a null terminator has been placed
	*/
	void strncpyz(char *dest, const char *src, int destsize)
	{
		if (!dest)
		{
			return;
		}

		if (!src)
		{
			return;
		}

		if (destsize < 1)
		{
			return;
		}

		if (dest != src)//Fix crash on OSX when using 10.9 SDK or newer
		{
			strncpy(dest, src, destsize - 1);
		}

		dest[destsize - 1] = 0;
	}

	/*
	*	Returns a hash value from 0-dwMaxHashSize, used for hash tables.
	*	Do not use for MPQ-related hash functions, those are special!
	*/
	DWORD strhash(const char* szString, size_t dwLen, size_t dwMaxHashSize)
	{
		DWORD hash = 0;
		int c;
		size_t stringLen = strlen(szString);

		if (dwLen > stringLen || dwLen == 0)
		{
			dwLen = stringLen;
		}

		for (int i = 0; i < dwLen; i++)
		{
			c = szString[i];
			hash = c + (hash << 6) + (hash << 16) - hash;
		}

		return hash % dwMaxHashSize;
	}

	/*
	 *	File Name Bottom Level Directory (FNBLD)
	 *	Gets the file name from a directory.
	 *	e.g., fnbld("data/global/excel/levels.txt") -> "levels.txt"
	 */
	char* fnbld(char* szFileName)
	{
		char* fn = szFileName;
		while (*szFileName)
		{
			if (*szFileName == '/' || *szFileName == '\\')
			{
				fn = szFileName + 1;
			}
			szFileName++;
		}
		return fn;
	}

	/*
	 *	File Name Bottom Level Directory with Safe Buffer (FNBLDB)
	 *	Gets the file name from a directory.
	 *	The input string is not modified.
	 *	e.g., fnbldb("data/global/excel/levels.txt") -> "levels.txt"
	 */
	char* fnbldb(char* szFileName)
	{
		static char szStaticBuffer[MAX_D2PATH_ABSOLUTE];
		strncpyz(szStaticBuffer, szFileName, MAX_D2PATH_ABSOLUTE);
		return fnbld(szStaticBuffer);
	}

	/*
	 *	File Name Extension (FNEXT)
	 *	Gets the extension of a given file path.
	 *	e.g., fnext("data/global/excel/levels.txt") -> ".txt"
	 */
	char* fnext(char* szFileName)
	{
		char* szCurrent = szFileName;
		while (*szCurrent != '\0' && strchr(szCurrent + 1, '.') != nullptr)
		{
			szCurrent++;
		}

		return szCurrent;
	}

	/*
	 *	File Name, Extension Stripped (FNEXTSTR)
	 *	Gets the file path without any extension.
	 *	e.g., fnextstr("data/global/excel/levels.txt") -> "data/global/excel/levels"
	 */
	char* fnextstr(char* szFileName)
	{
		char* szExtensionString = fnext(szFileName);
		*szExtensionString = '\0';
		return szFileName;
	}

	/*
	 *	File Name, Extension Stripped with Safe Buffer (FNEXTSTRB)
	 *	Gets the file path without any extension.
	 *	The input string is not modified.
	 *	e.g., fnextstrb("data/global/excel/levels.txt") -> "data/global/excel/levels"
	 */
	char* fnextstrb(char* szFileName)
	{
		static char szStaticBuffer[MAX_D2PATH_ABSOLUTE];
		strncpyz(szStaticBuffer, szFileName, MAX_D2PATH_ABSOLUTE);
		return fnextstr(szStaticBuffer);
	}

	/*
	*	Compares two strings for inequality, char16_t version.
	*	Case-insensitive. Searches up to a fixed length of characters.
	*/
	int qstricmpn(const char16_t* s1, const char16_t* s2, int n)
	{
		int		c1, c2;

		do {
			c1 = *s1++;
			c2 = *s2++;

			if (!n--) {
				return 0;		// strings are equal until end point
			}

			if (c1 != c2) {
				if (c1 >= 'a' && c1 <= 'z')
				{
					if (c1 - 'a' == c2 - 'A')
					{
						continue; // not actually different (case insensitivity)
					}
				}
				return c1 < c2 ? -1 : 1;
			}
		} while (c1);

		return 0;		// strings are equal
	}

	/*
	*	Compares two strings for inequality, char16_t version. Unlimited length, case insensitive.
	*/
	int qstricmp(const char16_t* s1, const char16_t* s2)
	{
		return qstricmpn(s1, s2, 99999);
	}

	/*
	*	Compares two strings for inequality. char16_t version.
	*	Case sensitive. Searches up to a fixed length of characters.
	*/
	int qstrcmpn(const char16_t* s1, const char16_t* s2, int n)
	{
		int		c1, c2;

		do {
			c1 = *s1++;
			c2 = *s2++;

			if (!n--) {
				return 0;		// strings are equal until end point
			}

			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		} while (c1);

		return 0;		// strings are equal
	}

	/*
	*	Compares two strings for inequality. char16_t version.
	*	Case sensitive. Unlimited length.
	*/
	int qstrcmp(const char16_t* s1, const char16_t* s2)
	{
		return qstrcmpn(s1, s2, 99999);
	}

	/*
	*	Safe string copy for char16_t
	*/
	size_t qstrncpyz(char16_t* dest, const char16_t* src, size_t destLen)
	{
		size_t len = 0;

		if (!dest)
		{
			return 0;
		}

		if (!src)
		{
			return 0;
		}

		if (destLen == 0)
		{
			return 0;
		}

		while (len < destLen - 1 && src[len] > 0)
		{
			dest[len] = src[len];
			len++;
		}
		dest[len++] = 0;
		return len;
	}

	/*
	*	Finds the length of a char16_t string.
	*/
	size_t qstrlen(const char16_t* s1)
	{
		size_t len = 0;
		char16_t* s2 = (char16_t*)s1;
		while (*s2++) len++;
		return len;
	}

	/*
	*	Reverses a char16_t string.
	*	@author	eezstreet
	*/
	void qstrverse(char16_t* s, int start, size_t len)
	{
		char16_t temp;

		temp = s[start];
		s[start] = s[len - start];
		s[len - start] = temp;
		if (start == len / 2)
		{
			return;
		}
		qstrverse(s, start + 1, len);
	}

	/*
	*	Converts an integer into a char16_t string.
	*	@author	eezstreet
	*/
	char16_t* qnitoa(int number, char16_t* buffer, size_t bufferLen, int base, size_t& written)
	{
		bool bNegative = false;
		written = 0;

		if (number == 0)
		{
			qstrncpyz(buffer, u"0", bufferLen);
			written++;
			return buffer;
		}

		if (number < 0 && base == 10)
		{
			bNegative = true;
			number = -number;
		}

		while (number != 0 && written < bufferLen)
		{
			int rem = number % base;
			buffer[written++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
			number = number / base;
		}

		// Append - to the end
		if (bNegative)
		{
			if (written < bufferLen)
			{
				buffer[written++] = '-';
			}
			else
			{
				buffer[bufferLen - 1] = '-';
			}
		}

		// Append null terminator to the end
		if (written < bufferLen)
		{
			buffer[written] = '\0';
		}
		else
		{
			buffer[bufferLen - 1] = '\0';
		}

		// Reverse the string
		qstrverse(buffer, 0, written - 1);

		return buffer;
	}

	/*
	*	Safe strchr for char16_t.
	*	Returns a substring from the first instance of the found specified character
	*/
	char16_t* qstrchr(char16_t* str, char16_t chr)
	{
		char16_t* p = str;
		while (p && *p && *p != chr)
		{
			p++;
		}

		if (!p || !(*p) || *p != chr)
		{
			return nullptr;
		}
		return p;
	}

	/*
	*	Pared-down format string handling. Only supports %%, %d and %s, but it uses char16_t to handle those.
	*	@author	eezstreet
	*/
	int qvsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, va_list args)
	{
		char16_t* p = (char16_t*)format;
		char16_t* next = qstrchr(p, '%');
		char16_t* o = buffer;
		size_t rem = bufferCount;
		int intArg;
		char16_t* strArg;

		while (next != nullptr && rem > 0)
		{
			size_t len = (next - p < rem) ? next - p : rem;
			qstrncpyz(o, p, len);
			o += len;
			rem -= next - p;

			p = next;
			p++;
			switch (*p)
			{
			case '%':
				*o = *p;
				o++; p++;
				rem--;
				break;
			case 'd':
			case 'i':
				intArg = va_arg(args, int);
				len = 0;
				qnitoa(intArg, o, rem, 10, len);
				o += len - 1;
				rem -= len;
				p++;
				break;
			case 's':
				strArg = va_arg(args, char16_t*);
				len = qstrlen(strArg);
				qstrncpyz(o, strArg, rem);
				rem -= len;
				o += len;
				p++;
				break;
			}

			next = qstrchr(p, '%');
		}

		qstrncpyz(o, p, rem);

		return bufferCount - rem;
	}

	/*
	*	Pared-down format string handling. Only supports %%, %d and %s, but uses char16_t to handle those.
	*	@author	eezstreet
	*/
	int qsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, ...)
	{
		va_list args;
		int out;

		va_start(args, format);
		out = qvsnprintf(buffer, bufferCount, format, args);
		va_end(args);

		return out;
	}

	/*
	*	Converts a char string to a char16_t string.
	*/
	size_t qmbtowc(char16_t* dest, size_t destLen, const char* src)
	{
		size_t len = 0;
		while (len < destLen && src[len] > 0)
		{
			dest[len] = src[len];
			len++;
		}
		dest[len++] = 0;
		return len;
	}

	/*
	*	Converts a char16_t string to char string.
	*	Note that any non-representable characters will be transformed into code point 1.
	*/
	size_t qwctomb(char* dest, size_t destLen, const char16_t* src)
	{
		size_t len = 0;
		while (len < destLen && src[len] > 0)
		{
			if (src[len] >= 256)
			{
				dest[len] = 1;
			}
			else
			{
				dest[len] = (char)src[len];
			}
			len++;
		}
		dest[len++] = 0;
		return len;
	}

	/*
	*	Hashes a char16_t. Based on SVR's code. Compatible with TBL files for lookup.
	*	Note that this uses a different algorithm than D2Lib::strhash.
	*/
	DWORD qstrhash(char16_t* str, size_t dwLen, DWORD dwMaxHashSize)
	{
		DWORD hash = 0;
		char16_t curChar;
		DWORD carry;

		while (curChar = *str++)
		{
			hash *= 0x10;
			hash += curChar;
			carry = hash & 0xF0000000;
			if (carry != 0)
			{
				carry /= 0x01000000;
				hash &= 0x0FFFFFFF;
				hash ^= carry;
			}
		}

		hash %= dwMaxHashSize;
		return hash;
	}

	//////////////////////////////////////////////////
	//
	// Random Number Functions

#define D2SEED_MAGIC	(QWORD)0x6AC690C5

	/*
	*	Generates a random number, given a seed.
	*	The seed is regenerated in the process.
	*	@author	eezstreet
	*/
	DWORD srand(D2Seed* pSeed)
	{
		QWORD mul = (pSeed->dwHiSeed + D2SEED_MAGIC) * pSeed->dwLoSeed;
		pSeed->dwHiSeed = HIDWORD(mul);
		pSeed->dwLoSeed = LODWORD(mul);
		return pSeed->dwLoSeed;
	}

	/*
	*	Generates a random number, without a seed.
	*	@author	eezstreet
	*/
	DWORD rand()
	{
		::srand(time(nullptr));
		return ::rand();
	}

	/*
	*	Generate a random number, with a maximum value, given a seed.
	*	The seed is regenerated in the process.
	*	@author	eezstreet
	*/
	DWORD smrand(D2Seed* pSeed, DWORD dwMax)
	{
		return srand(pSeed) % dwMax;
	}

	/*
	*	Generate a random number, with a maximum value, without using a seed.
	*	@author	eezstreet
	*/
	DWORD mrand(DWORD dwMax)
	{
		return rand() % dwMax;
	}

	/*
	*	Generate a random number, between two values (inclusive to both the minimum and maximum), given a seed.
	*	The seed is regenerated in the process.
	*	@author	eezstreet
	*/
	DWORD srrand(D2Seed* pSeed, DWORD dwMin, DWORD dwMax)
	{
		return smrand(pSeed, dwMax - dwMin) + dwMin;
	}

	/*
	*	Generate a random number, between the two values (inclusive to both the minimum and the maximum), without a seed.
	*	@author	eezstreet
	*/
	DWORD rrand(DWORD dwMin, DWORD dwMax)
	{
		return mrand(dwMax - dwMin) + dwMin;
	}

	/*
	*	Copies a random seed.
	*	@author	eezstreet
	*/
	void seedcopy(D2Seed* pDest, D2Seed* pSrc)
	{
		pDest->dwHiSeed = pSrc->dwHiSeed;
		pDest->dwLoSeed = pSrc->dwLoSeed;
	}

	/*
	*	Returns a random boolean value
	*	@author	eezstreet
	*/
	bool sbrand(D2Seed* pSeed)
	{
		return srand(pSeed) & 1;
	}

	//////////////////////////////////////////////////
	//
	// Math Functions
};
