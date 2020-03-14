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
		D2Lib::strncpyz(dest, src, destLen);
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
		handle keyHandle;
		bool bFull;

		if (!Contains(key, &keyHandle, &bFull))
		{
			if (bFull)
			{
				// hash map is full...should return an error
			}

			if (!Equals(key, keys[keyHandle]))
			{
				Copy(keys[keyHandle], key, L);
			}
		}

		return values[keyHandle];
	}

	V& operator[](const handle& handle)
	{
		return values[handle];
	}

	V* GetPointerTo(const handle& handle)
	{
		return &values[handle];
	}

	void Insert(const handle& handle, const K* key, const V& value)
	{
		values[handle] = value;
		Copy(keys[handle], key, L);
	}

	void Erase(const K* key)
	{
		DWORD hashVal = Hash(key);
		DWORD attempts = 1;

		while (!Equals(key, keys[hashVal]) && attempts < M)
		{
			if (keys[hashVal][0] == '\0')
			{
				return; // not found
			}
			hashVal++;
			hashVal %= M;
			attempts++;
		}

		if (attempts < M)
		{
			keys[hashVal][0] = '\0';
			values[hashVal] = (V)0;
		}
	}

	bool Contains(const K* key, handle* outHandle = nullptr, bool* bFull = nullptr)
	{
		DWORD hashVal = Hash(key);
		DWORD attempts = 1;

		if (bFull)
		{
			*bFull = false;
		}

		while (!Equals(key, keys[hashVal]) && attempts < M)
		{
			if (keys[hashVal][0] == '\0')
			{
				if (outHandle)
				{
					*outHandle = (handle)hashVal;
				}
				return false;
			}
			hashVal++;
			hashVal %= M;
			attempts++;
		}

		if (attempts >= M)
		{
			if (bFull)
			{
				*bFull = true;
			}
			return false;
		}

		if (outHandle)
		{
			*outHandle = hashVal;
		}
		return true;
	}

	handle NextFree(const K* key)
	{
		handle nextHandle;
		bool bFull;

		Contains(key, &nextHandle, &bFull);
		
		if (bFull)
		{
			return INVALID_HANDLE;
		}

		return nextHandle;
	}

	handle NextFree(const K* key, bool& bAlreadyInUse)
	{
		handle nextHandle;
		bool bFull;

		if (Contains(key, &nextHandle, &bFull))
		{
			bAlreadyInUse = true;
		}

		if (bFull)
		{
			return INVALID_HANDLE;
		}

		Copy(keys[nextHandle], key, L);
		return nextHandle;
	}
};
