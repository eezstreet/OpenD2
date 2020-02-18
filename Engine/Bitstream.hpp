#pragma once
#include "../Shared/D2Shared.hpp"

/*
 *	Bitstreams are used for both DCCs and networking.
 *	Based partially on id Tech 4's bitstreams.
 *	@author	eezstreet
 */
class Bitstream
{
public:
	Bitstream();
	~Bitstream();

	void LoadStream(BYTE* pNewStream, size_t dwStreamSizeBytes);
	void SplitFrom(Bitstream* pSplitStream, size_t dwSplitStreamSizeBits);

	void SetCurrentPosition(DWORD dwPosition, DWORD dwBitOffset = 0);

	void ReadByte(BYTE& outByte);
	void ReadWord(WORD& outWord);
	void ReadDWord(DWORD& outWord);
	void ReadBits(BYTE& outBits, int bitCount);
	void ReadBits(WORD& outBits, int bitCount);
	void ReadBits(DWORD& outBits, int bitCount);
	void ReadByte(BYTE* outByte);
	void ReadWord(WORD* outWord);
	void ReadDWord(DWORD* outWord);
	void ReadBits(BYTE* outBits, int bitCount);
	void ReadBits(WORD* outBits, int bitCount);
	void ReadBits(DWORD* outBits, int bitCount);
	void ReadBits(void* outBits, size_t outSize, int bitCount);
	void ReadData(void* data, size_t outSize);

	// Helper function - convert from unsigned to 2C
	void ConvertFormat(long* dwOutBits, int bitCount);

	void Rewind();

	size_t GetRemainingReadBits();

	BYTE* GetHeldData(size_t& outSize);

private:
	int ReadBits(int bitsCount);
	void FreeInternalStreamSource();

	bool bExternalStorage;
	BYTE* pStream;
	size_t dwStreamStartByte;
	size_t dwStreamStartBit;
	size_t dwTotalStreamSizeBytes;
	size_t dwTotalStreamSizeBits;
	size_t dwCurrentByte;
	size_t dwReadBit;
};