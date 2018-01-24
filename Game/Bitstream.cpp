#include "Diablo2.hpp"

/*
 *	Create the stream.
 */
Bitstream::Bitstream(void)
{
	// Init members
	m_lpStream = NULL;
	m_dwStreamLen = 0;
	m_dwStreamOffset = 0;
	m_dwCurrentPosition = 0;
	m_CurrentBit = 0;
}

/*
 *	Free the stream.
 */
Bitstream::~Bitstream(void)
{
	// Free stream
	if (m_lpStream != NULL)
	{
		free(m_lpStream);
		m_lpStream = NULL;
		m_dwStreamLen = 0;
	}
}

/*
 *	Copy another bitstream
 *	@author	eezstreet
 */
void Bitstream::CopyFrom(Bitstream* pBits, size_t offset, size_t len)
{
	m_lpStream = pBits->m_lpStream;
	m_CurrentBit = pBits->m_CurrentBit + offset;
	m_dwStreamLen = pBits->m_dwStreamLen;
	m_dwStreamOffset = len;
	m_dwCurrentPosition = pBits->m_dwCurrentPosition + (offset / 8);
}

/*
 *	Copy all remaining bits from one bitstream into another
 *	@author	eezstreet
 */
void Bitstream::CopyAllFrom(Bitstream* pBits, size_t offset)
{
	m_lpStream = pBits->m_lpStream;
	m_CurrentBit = pBits->m_CurrentBit + offset;
	m_dwStreamLen = pBits->m_dwStreamLen;
	m_dwStreamOffset = pBits->m_dwStreamOffset - offset;
	m_dwCurrentPosition = pBits->m_dwCurrentPosition + (offset / 8);
}

/*
 *	Internal - write a single bit into the bitstream.
 */
void Bitstream::_WriteBit(BYTE value, bool bSigned)
{
	// Check for current bit offset
	if ((m_CurrentBit % 8) == 0)
	{
		// Check for valid stream
		if (m_lpStream == NULL)
		{
			m_lpStream = (BYTE*)malloc(sizeof(BYTE));
			m_dwStreamLen = 1;
			m_lpStream[m_dwStreamLen - 1] = 0;
			m_CurrentBit = 0;
			m_dwStreamOffset = 0;
		}
		else
		{
			m_CurrentBit = 0;
			m_dwStreamLen++;
			m_lpStream = (BYTE*)realloc(m_lpStream, m_dwStreamLen * sizeof(BYTE));
			m_lpStream[m_dwStreamLen - 1] = 0;
		}
	}

	// Write single BIT
	if (bSigned)
	{
		m_lpStream[m_dwStreamLen - 1] |= ((value & 0x80) >> m_CurrentBit);
	}
	else
	{
		m_lpStream[m_dwStreamLen - 1] |= (value >> m_CurrentBit);
	}
	
	m_dwStreamOffset++;
	m_dwCurrentPosition = m_dwStreamOffset - 1;
	m_CurrentBit++;
}

/*
 *	Internal - read a single bit from the bitstream.
 */
void Bitstream::_ReadBit(BYTE& value)
{
	// Check for valid position
	value = 0x00;
	if (m_dwCurrentPosition < m_dwStreamOffset)
	{
		// Read single BIT
		DWORD currentByte = m_dwCurrentPosition >> 3;
		BYTE currentBit = (BYTE)(m_dwCurrentPosition % 8);
		value = ((BYTE)(m_lpStream[currentByte] << currentBit) >> 7);
		m_dwCurrentPosition = D2_max<DWORD>(0, D2_min<DWORD>(m_dwStreamOffset - 1, m_dwCurrentPosition + 1));
	}
}

/*
 *	Write a bit into the bitstream.
 */
void Bitstream::WriteBit(BYTE value, bool bSigned)
{
	// Write single BIT
	BYTE bitValue = ((value & 0x01) << 7);
	_WriteBit(bitValue, bSigned);
}

/*
 *	Read a single bit from the bitstream.
 */
void Bitstream::ReadBit(BYTE& value)
{
	// Read single BIT
	_ReadBit(value);
}

/*
 *	Write a single byte into the bitstream.
 */
void Bitstream::WriteByte(BYTE value, bool bSigned)
{
	// Write single BYTE
	BYTE currentOffset = 0;
	BYTE mask = 0x00;
	BYTE bitValue = 0;
	for (long i = 0; i<8; i++)
	{
		bitValue = ((value & mask) << currentOffset);
		_WriteBit(bitValue, bSigned);
		mask = mask >> 1;
		currentOffset++;
	}
}

/*
 *	Read a single byte from the bitstream.
 */
void Bitstream::ReadByte(BYTE& value)
{
	// Read single BYTE
	value = 0x00;
	BYTE currentOffset = 7;
	BYTE bitValue;
	for (long i = 0; i<8; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
}

/*
 *	Write a single word (2 bytes) into the bitstream.
 */
void Bitstream::WriteWord(WORD value, bool bSigned)
{
	// Write single WORD
	BYTE currentOffset = 0;
	WORD mask = 0x8000;
	BYTE bitValue = 0;
	for (long i = 0; i<16; i++)
	{
		bitValue = (BYTE)(((value & mask) << currentOffset) >> 8);
		_WriteBit(bitValue, bSigned);
		mask = mask >> 1;
		currentOffset++;
	}
}

/*
 *	Read a single word (2 bytes) from the bitstream.
 */
void Bitstream::ReadWord(WORD& value)
{
	// Read single WORD
	value = 0x0000;
	BYTE currentOffset = 15;
	BYTE bitValue;
	for (long i = 0; i<16; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
}

/*
 *	Write a single DWORD (4 bytes) into the stream.
 */
void Bitstream::WriteDWord(DWORD value, bool bSigned)
{
	// Write single DWORD
	BYTE currentOffset = 0;
	DWORD mask = 0x80000000;
	BYTE bitValue = 0;
	for (long i = 0; i<32; i++)
	{
		bitValue = (BYTE)(((value & mask) << currentOffset) >> 24);
		_WriteBit(bitValue, bSigned);
		mask = mask >> 1;
		currentOffset++;
	}
}

/*
 *	Read a single DWORD (4 bytes) from the stream.
 */
void Bitstream::ReadDWord(DWORD& value)
{
	// Read single DWORD
	value = 0x00000000;
	BYTE currentOffset = 31;
	BYTE bitValue;
	for (long i = 0; i<32; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
}

/*
 *	Write some data into the bitstream.
 */
void Bitstream::WriteData(BYTE* lpData, long nLen, bool bSigned)
{
	// Check for valid data
	if (lpData != NULL)
	{
		// Write variable-length data
		for (long i = 0; i<nLen; i++)
		{
			// Write single BYTE
			WriteByte(lpData[i], bSigned);
		}
	}
}

/*
 *	Read some data from the bitstream.
 */
void Bitstream::ReadData(BYTE* lpData, long nLen)
{
	// Check for valid data
	if (lpData != NULL)
	{
		// Read variable-length data
		for (long i = 0; i<nLen; i++)
		{
			// Read single BYTE
			ReadByte(lpData[i]);
		}
	}
}

/*
 *	Write some data into the bitstream.
 */
void Bitstream::WriteData(WORD* lpData, long nLen, bool bSigned)
{
	// Check for valid data
	if (lpData != NULL)
	{
		// Write variable-length data
		for (long i = 0; i<nLen; i++)
		{
			// Write single WORD
			WriteWord(lpData[i], bSigned);
		}
	}
}

/*
 *	Read some data from the bitstream.
 */
void Bitstream::ReadData(WORD* lpData, long nLen)
{
	// Check for valid data
	if (lpData != NULL)
	{
		// Read variable-length data
		for (long i = 0; i<nLen; i++)
		{
			// Read single WORD
			ReadWord(lpData[i]);
		}
	}
}

/*
 *	Write some data into the bitstream.
 */
void Bitstream::WriteData(DWORD* lpData, long nLen, bool bSigned)
{
	// Check for valid data
	if (lpData != NULL)
	{
		// Write variable-length data
		for (long i = 0; i<nLen; i++)
		{
			// Write single DWORD
			WriteDWord(lpData[i], bSigned);
		}
	}
}

/*
 *	Read some data from the bitstream.
 */
void Bitstream::ReadData(DWORD* lpData, long nLen)
{
	// Check for valid data
	if (lpData != NULL)
	{
		// Read variable-length data
		for (long i = 0; i<nLen; i++)
		{
			// Read single DWORD
			ReadDWord(lpData[i]);
		}
	}
}

/*
 *	Write some bits to the bitstream.
 */
void Bitstream::WriteBits(DWORD value, long nLen, bool bSigned)
{
	// Write single BITs
	long _nLen = D2_max<long>(0, D2_min<long>(32, nLen));
	BYTE currentOffset = (BYTE)(_nLen - 1);
	DWORD mask = (0x00000001 << (_nLen - 1));
	BYTE bitValue = 0;
	for (long i = 0; i<_nLen; i++)
	{
		bitValue = ((BYTE)((value & mask) >> currentOffset) << 7);
		_WriteBit(bitValue, bSigned);
		mask = mask >> 1;
		currentOffset--;
	}
}

/*
 *	Reads some bits.
 *	The output is stored in the parameter `value`
 */
void Bitstream::ReadBits(DWORD& value, long nLen)
{
	// Read single BITs
	value = 0x00000000;
	long _nLen = D2_max<long>(0, D2_min<long>(32, nLen));
	BYTE currentOffset = 31;
	BYTE bitValue;
	for (long i = 0; i<_nLen; i++)
	{
		_ReadBit(bitValue);
		value |= (bitValue << currentOffset);
		currentOffset--;
	}
}

/*
 *	Read some bits into a pointer.
 *	The output is stored in the parameter `value`
 *	@author	eezstreet
 */
void Bitstream::ReadBits(void* value, long nLen)
{
	DWORD* pValue = (DWORD*)value;

	*pValue = 0x00000000;
	long _nLen = D2_max<long>(0, D2_min<long>(32, nLen));
	BYTE currentOffset = 31;
	BYTE bitValue;
	for (long i = 0; i<_nLen; i++)
	{
		_ReadBit(bitValue);
		*pValue |= (bitValue << currentOffset);
		currentOffset--;
	}
}

/*
 *	Loads a chunk of memory into the bitstream.
 */
void Bitstream::LoadStream(BYTE* lpStream, long nLen)
{
	// Check for valid memory buffer
	if (lpStream != NULL)
	{
		// Free stream
		if (m_lpStream != NULL)
		{
			free(m_lpStream);
			m_lpStream = NULL;
			m_dwStreamLen = 0;
		}

		// Read stream from memory buffer
		m_dwStreamLen = nLen;
		m_lpStream = (BYTE*)malloc(m_dwStreamLen * sizeof(BYTE));
		memcpy(m_lpStream, lpStream, m_dwStreamLen * sizeof(BYTE));
		m_dwStreamOffset = m_dwStreamLen << 3;
		m_dwCurrentPosition = m_dwStreamOffset - 1;
		m_CurrentBit = 0;
	}
}

/*
 *	Writes the stream to the memory buffer.
 */
void Bitstream::SaveStream(BYTE* lpStream)
{
	// Check for valid memory buffer
	if (lpStream != NULL)
	{
		// Check for valid stream
		if (m_lpStream != NULL)
		{
			// Write stream to memory buffer
			memcpy(lpStream, m_lpStream, m_dwStreamLen * sizeof(BYTE));
		}
	}
}

/*
 *	Sets the current stream position.
 */
void Bitstream::SetCurrentPosition(DWORD dwCurrentPosition)
{
	m_dwCurrentPosition = D2_max<DWORD>(0, D2_min<DWORD>(m_dwStreamOffset - 1, dwCurrentPosition));
}