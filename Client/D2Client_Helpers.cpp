#include "D2Client.hpp"

// Assorted helper functions

/*
 *	Determines if the character class is male or female.
 *	@author	eezstreet
 */
bool Client_classMale(int nCharClass)
{
	switch (nCharClass)
	{
	case D2CLASS_AMAZON:
	case D2CLASS_ASSASSIN:
	case D2CLASS_SORCERESS:
		return false;
	default:
		return true;
	}
}

/*
 *	Returns the localized name of a character class.
 *	@author	eezstreet
 */
char16_t* Client_className(int nCharClass)
{
	switch (nCharClass)
	{
		case D2CLASS_AMAZON:
		default:
			return engine->TBL_FindStringFromIndex(4011);
		case D2CLASS_ASSASSIN:
			return engine->TBL_FindStringFromIndex(4013);
		case D2CLASS_BARBARIAN:
			return engine->TBL_FindStringFromIndex(4007);
		case D2CLASS_DRUID:
			return engine->TBL_FindStringFromIndex(4012);
		case D2CLASS_NECROMANCER:
			return engine->TBL_FindStringFromIndex(4009);
		case D2CLASS_PALADIN:
			return engine->TBL_FindStringFromIndex(4008);
		case D2CLASS_SORCERESS:
			return engine->TBL_FindStringFromIndex(4010);
	}
}