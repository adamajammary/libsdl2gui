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

using LSG_Rationals  = std::vector<LSG_Rational>;
using LSG_URationals = std::vector<LSG_URational>;

struct LSG_IFD_DataValue
{
	int32_t        integer    = 0;
	LSG_Rational   rational   = {};
	LSG_Rationals  rationals  = {};
	std::string    string     = "";
	LSG_Strings    strings    = {};
	uint32_t       uinteger   = 0;
	LSG_URational  urational  = {};
	LSG_URationals urationals = {};
};

struct LSG_IFD
{
	uint8_t           data[12]   = {};
	LSG_IFD_DataType  dataType   = LSG_IFD_DATA_TYPE_NONE;
	LSG_IFD_DataValue dataValue  = {};
	uint32_t          nrOfValues = 0;
	uint16_t          tagID      = 0;
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
	static long  offsetGPSInfo;
	static long  offsetSubIFD;

public:
	static LSG_ExifData         Get(const std::string& filePath);
	static LSG_GPS              GetGPS(const LSG_ExifTags& gps);
	static LSG_ImageOrientation GetOrientation(const LSG_ExifTags& tags);

private:
	static void              addTags(LSG_ExifTags& tags);
	static void              close();
	static double            getDouble(const LSG_Rational& rational);
	static double            getDouble(const std::string& rational);
	static LSG_GPSCoordinate getGPSCoordinate(const std::string& rational, const std::string& ref);
	static LSG_IFD           getIFD();
	static long              getOffsetIFD();
	static uint16_t          getNrOfDirectories();
	static SDL_Surface*      getThumbnail();
	static std::string       getValueString(const LSG_IFD& ifd);
	static LSG_Rationals     getValueRationals(const LSG_IFD& ifd);
	static LSG_URationals    getValueURationals(const LSG_IFD& ifd);
	static bool              isExif();
	static bool              isMarker();
	static bool              isValid();
	static void              seekToHeader();
};

#endif
