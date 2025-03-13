#include "main.h"

#ifndef LSG_EXIF_H
#define LSG_EXIF_H

// https://www.media.mit.edu/pia/Research/deepview/exif.html
// https://exiftool.org/TagNames/EXIF.html

enum LSG_IFD_DataType
{
	LSG_IFD_DATA_TYPE_NONE,
	LSG_IFD_DATA_TYPE_UINT8,       // uint8_t     | 1 byte
	LSG_IFD_DATA_TYPE_STRING,      // uint8_t[]   | 1 byte
	LSG_IFD_DATA_TYPE_UINT16,      // uint16_t    | 2 bytes
	LSG_IFD_DATA_TYPE_UINT32,      // uint32_t    | 4 bytes
	LSG_IFD_DATA_TYPE_URATIONAL64, // urational64 | 8 bytes
	LSG_IFD_DATA_TYPE_INT8,        // int8_t      | 1 byte
	LSG_IFD_DATA_TYPE_UNDEFINED,
	LSG_IFD_DATA_TYPE_INT16,       // int16_t     | 2 bytes
	LSG_IFD_DATA_TYPE_INT32,       // int32_t     | 4 bytes
	LSG_IFD_DATA_TYPE_RATIONAL64,  // rational64  | 8 bytes
	LSG_IFD_DATA_TYPE_FLOAT,       // float       | 4 bytes
	LSG_IFD_DATA_TYPE_DOUBLE       // double      | 8 bytes
};

struct LSG_Rational
{
	int32_t numerator   = 0;
	int32_t denominator = 0;
};

struct LSG_URational
{
	uint32_t numerator   = 0;
	uint32_t denominator = 0;
};

struct LSG_IFD_DataValue
{
	int32_t       integer    = 0;
	LSG_Rational  rational   = {};
	std::string   string     = "";
	uint32_t      uinteger   = 0;
	LSG_URational urational  = {};
	uint32_t      nrOfValues = 0;
};

struct LSG_IFD
{
	uint8_t           data[12]  = {};
	LSG_IFD_DataType  dataType  = LSG_IFD_DATA_TYPE_NONE;
	LSG_IFD_DataValue dataValue = {};
	uint16_t          tagID     = 0;
};

class LSG_Exif
{
private:
	LSG_Exif()  {}
	~LSG_Exif() {}

private:
	static FILE* file;
	static bool  isByteOrderIntel;
	static long  offsetHeader;
	static long  offsetSubIFD;

public:
	static LSG_ExifData         Get(const std::string& filePath);
	static LSG_ImageOrientation GetOrientation(const LSG_ExifTags& tags);

private:
	static void          addTags(LSG_ExifTags& tags);
	static LSG_IFD       getIFD();
	static long          getOffsetIFD();
	static uint16_t      getNrOfDirectories();
	static SDL_Surface*  getThumbnail();
	static std::string   getValueString(const LSG_IFD& ifd);
	static LSG_Rational  getValueRational(const LSG_IFD& ifd);
	static LSG_URational getValueURational(const LSG_IFD& ifd);
	static bool          isExif();
	static bool          isMarker();
	static bool          isValid();
	static void          seekToHeader();
};

#endif
