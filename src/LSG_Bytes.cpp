#include "LSG_Bytes.h"

int16_t LSG_Bytes::ToInt(uint8_t a, uint8_t b, bool littleEndian)
{
	return (int16_t)LSG_Bytes::ToUInt(a, b, littleEndian);
}

uint16_t LSG_Bytes::ToUInt(uint8_t a, uint8_t b, bool littleEndian)
{
	return (uint16_t)(littleEndian ? ((b << 8) | a) : ((a << 8) | b));
}

int32_t LSG_Bytes::ToInt(uint8_t a, uint8_t b, uint8_t c, uint8_t d, bool littleEndian)
{
	return (int32_t)LSG_Bytes::ToUInt(a, b, c, d, littleEndian);
}

uint32_t LSG_Bytes::ToUInt(uint8_t a, uint8_t b, uint8_t c, uint8_t d, bool littleEndian)
{
	if (littleEndian)
		return (uint32_t)((d << 24) | (c << 16) | (b << 8) | a);

	return (uint32_t)((a << 24) | (b << 16) | (c << 8) | d);
}
