#pragma once
#include "D2Shared.hpp"

template<typename K, typename V, const int M = 128, const int L = 64>
class HashMap
{
private:
	K keys[M][L];
	V values[M];

	const DWORD Hash(const char16_t* str)
	{
		return D2Lib::qstrhash(str, L, M);
	}

	const DWORD Hash(const char* str)
	{
		return D2Lib::strhash(str, L, M);
	}

	const bool Equals(const char16_t* A, const char16_t* B)
	{
		return !D2Lib::qstricmp(A, B);
	}

	const bool Equals(const char* A, const char* B)
	{
		return !D2Lib::stricmp(A, B);
	}

	const void Copy(char* dest, const char* src, const int destLen)
	{
		return D2Lib::strncpyz(dest, src, destLen);
	}

	const void Copy(char16_t* dest, const char16_t* src, const int destLen)
	{
                D2Lib::qstrncpyz(dest, src, destLen);
	}
public:
	HashMap()
	{
		memset(keys, 0, sizeof(K) * M * L);
	}

	V& operator[](const K* key)
	{
		DWORD hashVal = Hash(key);
		DWORD attempts = 1;

		while (!Equals(key, keys[hashVal]) && attempts < M)
		{
			if (keys[hashVal][0] == '\0')
			{
				break;
			}
			hashVal++;
			hashVal %= M;
			attempts++;
		}

		if (!Equals(key, keys[hashVal]))
		{
			Copy(keys[hashVal], key, L);
		}

		if (attempts >= M)
		{
			// TODO: throw an error here?
		}

		return values[hashVal];
	}

	V& operator[](const handle& handle)
	{
		return values[handle];
	}

	void Insert(const handle& handle, const K* key, const V& value)
	{
		values[handle] = value;
		Copy(keys[handle], key, L);
	}

	handle NextFree(const K* key)
	{
		DWORD hashVal = Hash(key);
		DWORD attempts = 1;

		while (!Equals(key, keys[hashVal]) && attempts < M)
		{
			if (keys[hashVal][0] == '\0')
			{
				break;
			}
			hashVal++;
			hashVal %= M;
			attempts++;
		}

		if (attempts >= M)
		{
			return INVALID_HANDLE;
		}

		return (handle)hashVal;
	}

	handle NextFree(const K* key, bool& bAlreadyInUse)
	{
		DWORD hashVal = Hash(key);
		DWORD attempts = 1;

		while (!Equals(key, keys[hashVal]) && attempts < M)
		{
			if (keys[hashVal][0] == '\0')
			{
				break;
			}
			hashVal++;
			hashVal %= M;
			attempts++;
		}

		if (Equals(key, keys[hashVal]))
		{
			bAlreadyInUse = true;
		}

		if (attempts >= M)
		{
			return INVALID_HANDLE;
		}

		Copy(keys[hashVal], key, L);

		return (handle)hashVal;
	}
};