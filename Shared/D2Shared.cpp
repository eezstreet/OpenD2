#include "D2Shared.hpp"
#include <cstdio>
#include <cstdarg>
#include <string>
#include <time.h>

//////////////////////////////////////////////////
//
// String Functions

/*
 *	Compares two strings for case-insensitive equality, with a limited length.
 */
int D2_stricmpn(char* s1, char* s2, int n)
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
int D2_stricmp(char* s1, char* s2)
{
	return (s1 && s2) ? D2_stricmpn(s1, s2, 99999) : -1;
}

/*
 *	Safe strncpy that assures a null terminator has been placed
 */
void D2_strncpyz(char *dest, const char *src, int destsize) 
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
DWORD D2_strhash(char* szString, size_t dwLen, size_t dwMaxHashSize)
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
 *	Compares two strings for inequality, char16_t version.
 *	Case-insensitive. Searches up to a fixed length of characters.
 */
int D2_qstricmpn(char16_t* s1, char16_t* s2, int n)
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
int D2_qstricmp(char16_t* s1, char16_t* s2)
{
	return D2_qstricmpn(s1, s2, 99999);
}

/*
 *	Compares two strings for inequality. char16_t version.
 *	Case sensitive. Searches up to a fixed length of characters.
 */
int D2_qstrcmpn(char16_t* s1, char16_t* s2, int n)
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
int D2_qstrcmp(char16_t* s1, char16_t* s2)
{
	return D2_qstrcmpn(s1, s2, 99999);
}

/*
 *	Safe string copy for char16_t
 */
size_t D2_qstrncpyz(char16_t* dest, char16_t* src, size_t destLen)
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

	while (len <= destLen && src[len] > 0)
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
size_t D2_qstrlen(char16_t* s1)
{
	size_t len = 0;
	while (*s1++) len++;
	return len;
}

/*
 *	Reverses a char16_t string.
 *	@author	eezstreet
 */
char16_t* D2_qstrverse(char16_t* s)
{
	size_t len = D2_qstrlen(s);

	for (size_t i = len-1; i > 0; i++)
	{
		char16_t character = s[i];
		s[i] = s[i - 1];
		s[i - 1] = character;
	}

	return s;
}

/*
 *	Converts an integer into a char16_t string.
 *	@author	eezstreet
 */
char16_t* D2_qnitoa(int number, char16_t* buffer, size_t bufferLen, int base, size_t& written)
{
	bool bNegative = false;
	written = 0;

	if (number == 0)
	{
		D2_qstrncpyz(buffer, u"0", bufferLen);
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
		written++;
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
	D2_qstrverse(buffer);

	return buffer;
}

/*
 *	Safe strchr for char16_t.
 *	Returns a substring from the first instance of the found specified character
 */
char16_t* D2_qstrchr(char16_t* str, char16_t chr)
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
int D2_qvsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, va_list args)
{
	char16_t* p = (char16_t*)format;
	char16_t* next = D2_qstrchr(p, '%');
	char16_t* o = buffer;
	size_t rem = bufferCount;
	int intArg;
	char16_t* strArg;

	while (next != nullptr && rem > 0)
	{
		size_t len = (next - p < rem) ? next - p : rem;
		D2_qstrncpyz(o, p, len);
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
				D2_qnitoa(intArg, o, rem, 10, len);
				o += len - 1;
				rem -= len;
				p++;
				break;
			case 's':
				strArg = va_arg(args, char16_t*);
				len = D2_qstrlen(strArg);
				D2_qstrncpyz(o, strArg, rem);
				rem -= len;
				o += len;
				p++;
				break;
		}

		next = D2_qstrchr(p, '%');
	}

	D2_qstrncpyz(o, p, rem);

	return bufferCount - rem;
}

/*
 *	Pared-down format string handling. Only supports %%, %d and %s, but uses char16_t to handle those.
 *	@author	eezstreet
 */
int D2_qsnprintf(char16_t* buffer, size_t bufferCount, const char16_t* format, ...)
{
	va_list args;
	int out;

	va_start(args, format);
	out = D2_qvsnprintf(buffer, bufferCount, format, args);
	va_end(args);

	return out;
}

/*
 *	Converts a char string to a char16_t string.
 */
size_t D2_qmbtowc(char16_t* dest, size_t destLen, char* src)
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
size_t D2_qwctomb(char* dest, size_t destLen, char16_t* src)
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
			dest[len] = src[len];
		}
		len++;
	}
	dest[len++] = 0;
	return len;
}

/*
 *	Hashes a char16_t. Based on SVR's code. Compatible with TBL files for lookup.
 *	Note that this uses a different algorithm than D2_strhash.
 */
DWORD D2_qstrhash(char16_t* str, size_t dwLen, DWORD dwMaxHashSize)
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
DWORD D2_srand(D2Seed* pSeed)
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
DWORD D2_rand()
{
	srand(time(nullptr));
	return rand();
}

/*
 *	Generate a random number, with a maximum value, given a seed.
 *	The seed is regenerated in the process.
 *	@author	eezstreet
 */
DWORD D2_smrand(D2Seed* pSeed, DWORD dwMax)
{
	return D2_srand(pSeed) % dwMax;
}

/*
 *	Generate a random number, with a maximum value, without using a seed.
 *	@author	eezstreet
 */
DWORD D2_mrand(DWORD dwMax)
{
	return D2_rand() % dwMax;
}

/*
 *	Generate a random number, between two values (inclusive to both the minimum and maximum), given a seed.
 *	The seed is regenerated in the process.
 *	@author	eezstreet
 */
DWORD D2_srrand(D2Seed* pSeed, DWORD dwMin, DWORD dwMax)
{
	return D2_smrand(pSeed, dwMax - dwMin) + dwMin;
}

/*
 *	Generate a random number, between the two values (inclusive to both the minimum and the maximum), without a seed.
 *	@author	eezstreet
 */
DWORD D2_rrand(DWORD dwMin, DWORD dwMax)
{
	return D2_mrand(dwMax - dwMin) + dwMin;
}

/*
 *	Copies a random seed.
 *	@author	eezstreet
 */
void D2_seedcopy(D2Seed* pDest, D2Seed* pSrc)
{
	pDest->dwHiSeed = pSrc->dwHiSeed;
	pDest->dwLoSeed = pSrc->dwLoSeed;
}

/*
 *	Returns a random boolean value
 *	@author	eezstreet
 */
bool D2_sbrand(D2Seed* pSeed)
{
	return D2_srand(pSeed) & 1;
}

//////////////////////////////////////////////////
//
// Math Functions
