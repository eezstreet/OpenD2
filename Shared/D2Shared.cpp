#include "D2Shared.hpp"
#include <cstdlib>
#include <string>

//////////////////////////////////////////////////
//
// Library functions

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

	for (int i = 0; i < dwLen; i++)
	{
		c = szString[i];
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash % dwMaxHashSize;
}