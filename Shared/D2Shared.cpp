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
void D2_strncpyz(char *dest, const char *src, int destsize) {
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
 *	"Pops" a single token from a string.
 *	A token can consist of a single word, or a section in quotes.
 *	Likewise, C-style comments are parsed out.
 */
static	char	d2_token[MAX_TOKEN_CHARS];
static	int		d2_lines;

const char *SkipWhitespace(const char *data, bool *hasNewLines) {
	int c;

	while ((c = *data) <= ' ') {
		if (!c) {
			return nullptr;
		}
		if (c == '\n') {
			d2_lines++;
			*hasNewLines = true;
		}
		data++;
	}

	return data;
}

char *D2_ParseExt(const char **data_p, bool allowLineBreaks)
{
	int c = 0, len;
	bool hasNewLines = false;
	const char *data;

	data = *data_p;
	len = 0;
	d2_token[0] = 0;

	// make sure incoming data is valid
	if (!data)
	{
		*data_p = nullptr;
		return d2_token;
	}

	while (1)
	{
		// skip whitespace
		data = SkipWhitespace(data, &hasNewLines);
		if (!data)
		{
			*data_p = nullptr;
			return d2_token;
		}
		if (hasNewLines && !allowLineBreaks)
		{
			*data_p = data;
			return d2_token;
		}

		c = *data;

		// skip double slash comments
		if (c == '/' && data[1] == '/')
		{
			data += 2;
			while (*data && *data != '\n') {
				data++;
			}
		}
		// skip /* */ comments
		else if (c == '/' && data[1] == '*')
		{
			data += 2;
			while (*data && (*data != '*' || data[1] != '/'))
			{
				data++;
			}
			if (*data)
			{
				data += 2;
			}
		}
		else
		{
			break;
		}
	}

	// handle quoted strings
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c == '\"' || !c)
			{
				d2_token[len] = 0;
				*data_p = (char *)data;
				return d2_token;
			}
			if (len < MAX_TOKEN_CHARS)
			{
				d2_token[len] = c;
				len++;
			}
		}
	}

	// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			d2_token[len] = c;
			len++;
		}
		data++;
		c = *data;
		if (c == '\n')
		{
			d2_lines++;
		}
	} while (c>32);	// Only parse printable characters.

	if (len == MAX_TOKEN_CHARS)
	{
		//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	}
	d2_token[len] = 0;

	*data_p = (char *)data;
	return d2_token;
}