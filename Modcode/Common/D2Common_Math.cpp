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