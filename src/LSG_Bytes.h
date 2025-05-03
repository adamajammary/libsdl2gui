#include "main.h"

#ifndef LSG_BYTES_H
#define LSG_BYTES_H

class LSG_Bytes
{
private:
	LSG_Bytes()  {}
	~LSG_Bytes() {}

public:
	static int16_t  ToInt(uint8_t  a, uint8_t b, bool littleEndian);
	static uint16_t ToUInt(uint8_t a, uint8_t b, bool littleEndian);

	static int32_t  ToInt(uint8_t  a, uint8_t b, uint8_t c, uint8_t d, bool littleEndian);
	static uint32_t ToUInt(uint8_t a, uint8_t b, uint8_t c, uint8_t d, bool littleEndian);
};

#endif
