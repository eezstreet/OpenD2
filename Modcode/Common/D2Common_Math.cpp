#include "D2Common.hpp"

typedef void(*BitwiseFunction)(DWORD* pIn, DWORD pOperand);

/*
 *	Math function tables
 *	These get called by a wide variety of D2Common functions
 */

// OR
static void Math_Or(DWORD* pIn, DWORD pOperand)
{
	*pIn |= pOperand;
}

// AND
static void Math_And(DWORD* pIn, DWORD pOperand)
{
	*pIn &= pOperand;
}

// XOR
static void Math_Xor(DWORD* pIn, DWORD pOperand)
{
	*pIn ^= pOperand;
}

// MOV
static void Math_Mov(DWORD* pIn, DWORD pOperand)
{
	*pIn = pOperand;
}

// MOVZ
static void Math_Movz(DWORD* pIn, DWORD pOperand)
{
	if (*pIn == 0)
	{
		*pIn = pOperand;
	}
}

// ANDNOT
static void Math_AndNot(DWORD* pIn, DWORD pOperand)
{
	*pIn &= ~pOperand;
}

static BitwiseFunction MathFunctions[] =
{
	Math_Or,
	Math_And,
	Math_Xor,
	Math_Mov,
	Math_Movz,
	Math_AndNot,
};

/*
 *	Performs the specified math function on a variable.
 *	@author	eezstreet
 */
void Math_Perform(D2MathFunc func, DWORD* pIn, DWORD pOperand)
{
	MathFunctions[func](pIn, pOperand);
}

/**
 *	Gets the next number in the seed sequence.
 */
uint16_t Seed_Next(uint16_t& seed)
{
	seed ^= seed << 7;
	seed ^= seed >> 9;
	seed ^= seed << 8;
	return seed;
}

uint32_t Seed_Next(uint32_t& seed)
{
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	return seed;
}

uint64_t Seed_Next(uint64_t& seed)
{
	seed ^= seed << 13;
	seed ^= seed >> 7;
	seed ^= seed << 17;
	return seed;
}

template<typename T>
int Seed_RangeTemplated(T& seed, int min, int max)
{
	uint16_t seeded = Seed_Next(seed);
	int range = max - min;
	if (range == 0)
		range = 1;
	int randomNum = seeded % range;
	randomNum += min;
	if (randomNum > max)
	{
		randomNum = max;
	}
	else if (randomNum < min)
	{
		randomNum = min;
	}
	return randomNum;
}

int Seed_Range(uint16_t& seed, int min, int max)
{
	return Seed_RangeTemplated(seed, min, max);
}

int Seed_Range(uint32_t& seed, int min, int max)
{
	return Seed_RangeTemplated(seed, min, max);
}

int Seed_Range(uint64_t& seed, int min, int max)
{
	return Seed_RangeTemplated(seed, min, max);
}

int Seed_Range(int& seed, int min, int max)
{
	return Seed_Range((uint64_t&)seed, min, max);
}