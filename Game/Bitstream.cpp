#include "Diablo2.hpp"

/*
 *	Creates a new blank bitstream.
 *	@author	eezstreet
 */
Bitstream::Bitstream()
{
	pStream = nullptr;
	dwTotalStreamSizeBits = 0;
	dwTotalStreamSizeBytes = 0;
	bExternalStorage = false;
	dwStreamStartByte = 0;
	dwStreamStartBit = 0;
}

/*
 *	Deletes the bitstream.
 *	@author	eezstreet
 */
Bitstream::~Bitstream()
{
	FreeInternalStreamSource();
}

/*
 *	Retrieves the data in a bitstream.
 *	The outSize argument is filled with the size of the bitstream.
 *	@author	eezstreet
 */
BYTE* Bitstream::GetHeldData(size_t& outSize)
{
	outSize = dwTotalStreamSizeBytes;
	return pStream;
}

/*
 *	Load in a stream using external memory storage.
 *	The external source is not modified and needs to be freed manually when the stream is destroyed.
 *	@author	eezstreet
 */
void Bitstream::LoadStream(BYTE* pNewStream, size_t dwNewSizeBytes)
{
	FreeInternalStreamSource();

	bExternalStorage = true;
	pStream = pNewStream;
	dwTotalStreamSizeBits = dwNewSizeBytes * 8;
	dwTotalStreamSizeBytes = dwNewSizeBytes;
	dwStreamStartByte = 0;
	dwStreamStartBit = 0;
	
#ifndef BIG_ENDIAN
	dwReadBit = dwCurrentByte = 0;
#endif
}

/*
 *	"Split" this stream from another stream. It inherits the data but uses a different offset.
 *	@author	eezstreet
 */
void Bitstream::SplitFrom(Bitstream* pSplitStream, size_t dwSplitStreamSizeBits)
{
	DWORD dwBitsHanging = (dwSplitStreamSizeBits % 8);
	DWORD dwBytes = (dwSplitStreamSizeBits - dwBitsHanging) / 8;
	FreeInternalStreamSource();
	
	// Copy from the other bitstream
	bExternalStorage = true;
	dwReadBit = pSplitStream->dwReadBit;
	dwCurrentByte = pSplitStream->dwCurrentByte;
	if (dwReadBit == 0)
	{	// Send the bitstream back by one byte so we don't encounter errors
		dwCurrentByte--;
	}
	dwTotalStreamSizeBits = pSplitStream->dwTotalStreamSizeBits;
	dwTotalStreamSizeBytes = pSplitStream->dwTotalStreamSizeBytes;
	pStream = pSplitStream->pStream;

	// Advance the other bitstream
	pSplitStream->dwCurrentByte += dwBytes;
	pSplitStream->dwReadBit += dwBitsHanging;
	while (pSplitStream->dwReadBit >= 8)
	{
		pSplitStream->dwCurrentByte++;
		pSplitStream->dwReadBit -= 8;
	}

	dwStreamStartBit = dwReadBit;
	dwStreamStartByte = dwCurrentByte;
}

/*
 *	Set the current stream position.
 *	@author	eezstreet
 */
void Bitstream::SetCurrentPosition(DWORD dwPosition, DWORD dwBitOffset)
{
	dwCurrentByte = dwPosition;
	dwReadBit = dwBitOffset;
}

/*
 *	"Rewind" the stream back to its original start.
 *	When you use this on a split stream, it will rewind back to the split start.
 *	When you use this on any other stream, it will rewind all the way back to the beginning.
 *	@author	eezstreet
 */
void Bitstream::Rewind()
{
	dwReadBit = dwStreamStartBit;
	dwCurrentByte = dwStreamStartByte;
}

/*
 *	Read a byte from the stream.
 *	@author	eezstreet
 */
// Overload 1
void Bitstream::ReadByte(BYTE& outByte)
{
	DWORD bits = ReadBits(8);
	
	outByte = (BYTE)bits;
}

// Overload 2
void Bitstream::ReadByte(BYTE* outByte)
{
	DWORD bits = ReadBits(8);

	*outByte = (BYTE)bits;
}

/*
 *	Read a word from the stream.
 *	@author	eezstreet
 */
// Overload 1
void Bitstream::ReadWord(WORD& outWord)
{
	DWORD bits = ReadBits(16);

	outWord = (WORD)bits;
}

// Overload 2
void Bitstream::ReadWord(WORD* outWord)
{
	DWORD bits = ReadBits(16);

	*outWord = (WORD)bits;
}

/*
 *	Read a doubleword from the stream.
 *	@author	eezstreet
 */
// Overload 1
void Bitstream::ReadDWord(DWORD& outDWord)
{
	DWORD bits = ReadBits(32);

	outDWord = bits;
}

// Overload 2
void Bitstream::ReadDWord(DWORD* outDWord)
{
	DWORD bits = ReadBits(32);

	*outDWord = bits;
}

/*
 *	Read bits from the stream (public). If a negative number is used it will read a signed number.
 *	@author	eezstreet
 */
// Overload 1
void Bitstream::ReadBits(BYTE& outBits, int bitCount)
{
	BYTE bits = ReadBits(bitCount);

	outBits = bits;
}

// Overload 2
void Bitstream::ReadBits(WORD& outBits, int bitCount)
{
	WORD bits = ReadBits(bitCount);

	outBits = bits;
}

// Overload 3
void Bitstream::ReadBits(DWORD& outBits, int bitCount)
{
	DWORD bits = ReadBits(bitCount);

	outBits = bits;
}

// Overload 4
void Bitstream::ReadBits(BYTE* outBits, int bitCount)
{
	BYTE bits = ReadBits(bitCount);

	*outBits = bits;
}

// Overload 5
void Bitstream::ReadBits(WORD* outBits, int bitCount)
{
	WORD bits = ReadBits(bitCount);

	*outBits = bits;
}

// Overload 6
void Bitstream::ReadBits(DWORD* outBits, int bitCount)
{
	DWORD bits = ReadBits(bitCount);

	*outBits = bits;
}

// Overload 7
void Bitstream::ReadBits(void* outBits, size_t outBitsSize, int bitCount)
{
	switch (outBitsSize)
	{
		case 1:
			{
				BYTE nBits = ReadBits(bitCount);
				*(BYTE*)outBits = nBits;
			}
			break;
		case 2:
			{
				WORD wBits = ReadBits(bitCount);
				*(WORD*)outBits = wBits;
			}
			break;
		case 4:
			{
				DWORD dwBits = ReadBits(bitCount);
				*(DWORD*)outBits = dwBits;
			}
			break;
		case 8:
			{
				QWORD qwBits = ReadBits(bitCount);
				*(QWORD*)outBits = qwBits;
			}
			break;
	}
}

/*
 *	Helper function: converts from an unsigned number to a twos complement number.
 *	@author	eezstreet/SVR
 */
void Bitstream::ConvertFormat(long* dwOutBits, int bitCount)
{
	long dwValue;
	if (dwOutBits == nullptr)
	{
		return;
	}
	dwValue = *dwOutBits;

	if (bitCount < 32 && (dwValue & (1 << (bitCount - 1))))
	{
		dwValue |= ~((1 << bitCount) - 1);
	}

	*dwOutBits = dwValue;
}

/*
 *	Get the number of bits remaining to be read.
 *	@author	eezstreet
 */
size_t Bitstream::GetRemainingReadBits()
{
	return dwTotalStreamSizeBits - ((dwCurrentByte * 8) + dwReadBit);
}

/*
 *	Read bits from the stream (private). If a negative number is used it will read a signed number.
 *	@author	id Software / eezstreet
 */
int Bitstream::ReadBits(int numBits) 
{
	int		value;
	int		valueBits;
	int		get;
	int		fraction;
	bool	sgn;

	if (numBits == 0)
	{
		return 0;
	}

	Log_ErrorAssert(pStream != nullptr, 0);
	Log_ErrorAssert(numBits >= -31 && numBits <= 32, 0);

	value = 0;
	valueBits = 0;

	if (numBits < 0) {
		numBits = -numBits;
		sgn = true;
	}
	else {
		sgn = false;
	}

	// check for overflow
	if (numBits > GetRemainingReadBits()) {
		return -1;
	}

	while (valueBits < numBits) {
		if (dwReadBit == 0) {
			dwCurrentByte++;
		}
		get = 8 - dwReadBit;
		if (get >(numBits - valueBits)) {
			get = numBits - valueBits;
		}
		fraction = pStream[dwCurrentByte - 1];
		fraction >>= dwReadBit;
		fraction &= (1 << get) - 1;
		value |= fraction << valueBits;

		valueBits += get;
		dwReadBit = (dwReadBit + get) & 7;
	}

	if (sgn) {
		if (value & (1 << (numBits - 1))) {
			value |= -1 ^ ((1 << numBits) - 1);
		}
	}

	Log_ErrorAssert(dwCurrentByte <= dwTotalStreamSizeBytes, 0);

	return value;
}

/*
 *	Reads a big chunk of continuous data from the stream
 *	@author	eezstreet
 */
void Bitstream::ReadData(void* data, size_t dataSize)
{
	size_t i;
	for (i = 0; i < dataSize; i++)
	{
		ReadByte((BYTE*)data);
	}
}

/*
 *	Frees the memory that this bitstream is holding onto, if any
 *	@author	eezstreet
 */
void Bitstream::FreeInternalStreamSource()
{
	if (pStream != nullptr && !bExternalStorage)
	{
		free(pStream);
	}
}