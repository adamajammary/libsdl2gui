#include "LSG_Exif.h"

FILE* LSG_Exif::file             = nullptr;
bool  LSG_Exif::isByteOrderIntel = false;
long  LSG_Exif::offsetHeader     = 0;
long  LSG_Exif::offsetGPSInfo    = 0;
long  LSG_Exif::offsetSubIFD     = 0;

void LSG_Exif::addTags(LSG_ExifTags& tags)
{
	auto nrOfDirectories = LSG_Exif::getNrOfDirectories();
		
	for (uint16_t i = 0; i < nrOfDirectories; i++)
	{
        auto ifd = LSG_Exif::getIFD();

		if (!ifd.dataValue.string.empty())
			tags[ifd.tagID] = ifd.dataValue.string;

		switch (ifd.tagID) {
			case LSG_EXIF_TAG_ID_OFFSET_GPS_INFO: LSG_Exif::offsetGPSInfo = ifd.dataValue.uinteger; break;
			case LSG_EXIF_TAG_ID_OFFSET_SUB_IFD:  LSG_Exif::offsetSubIFD  = ifd.dataValue.uinteger; break;
			default: break;
		}
	}
}

LSG_ExifData LSG_Exif::Get(const std::string& filePath)
{
	#if defined _windows
		LSG_Exif::file = _wfopen(LSG_Text::ToWide(filePath).c_str(), L"rb");
	#else
		LSG_Exif::file = std::fopen(filePath.c_str(), "rb");
	#endif

	if (!LSG_Exif::file)
		return {};

	LSG_Exif::seekToHeader();

	if (!LSG_Exif::isValid())
		return {};

	LSG_ExifData data = {};

	// IFD0 (main image)

	LSG_Exif::offsetSubIFD = 0;

	long offsetIFD1 = 0;
	auto offsetIFD0 = LSG_Exif::getOffsetIFD();

	if (offsetIFD0)
	{
		std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + offsetIFD0), SEEK_SET);

		LSG_Exif::addTags(data.tags);

		offsetIFD1 = LSG_Exif::getOffsetIFD();
	}

	// GPSInfo

	if (LSG_Exif::offsetGPSInfo)
	{
		std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + LSG_Exif::offsetGPSInfo), SEEK_SET);

		LSG_Exif::addTags(data.gps);
	}

	// SubIFD

	if (LSG_Exif::offsetSubIFD)
	{
		std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + LSG_Exif::offsetSubIFD), SEEK_SET);

		LSG_Exif::addTags(data.tags);
	}

	// IFD1 (thumbnail image)

	if (offsetIFD1)
	{
		std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + offsetIFD1), SEEK_SET);

		data.thumbnail = LSG_Exif::getThumbnail();
	}

	std::fclose(LSG_Exif::file);

	LSG_Exif::file = nullptr;

	return data;
}

double LSG_Exif::getDouble(const LSG_Rational& rational)
{
	return (rational.denominator ? ((double)rational.numerator / (double)rational.denominator) : 0.0);
}

double LSG_Exif::getDouble(const std::string& rational)
{
	if (rational.empty())
		return 0.0;

	LSG_Rational r;

	auto result = std::sscanf(rational.c_str(), "%d/%d", &r.numerator, &r.denominator);

	return (result == 2 ? LSG_Exif::getDouble(r) : 0.0);

}

LSG_GPS LSG_Exif::GetGPS(const LSG_ExifTags& gps)
{
	auto latitude  = (gps.contains(LSG_EXIF_TAG_ID_GPS_LATITUDE)  ? gps.at(LSG_EXIF_TAG_ID_GPS_LATITUDE)  : "");
    auto longitude = (gps.contains(LSG_EXIF_TAG_ID_GPS_LONGITUDE) ? gps.at(LSG_EXIF_TAG_ID_GPS_LONGITUDE) : "");
	auto altitude  = (gps.contains(LSG_EXIF_TAG_ID_GPS_ALTITUDE)  ? gps.at(LSG_EXIF_TAG_ID_GPS_ALTITUDE)  : "");

	auto latitudeRef  = (gps.contains(LSG_EXIF_TAG_ID_GPS_LATITUDE_REF)  ? gps.at(LSG_EXIF_TAG_ID_GPS_LATITUDE_REF)  : "");
    auto longitudeRef = (gps.contains(LSG_EXIF_TAG_ID_GPS_LONGITUDE_REF) ? gps.at(LSG_EXIF_TAG_ID_GPS_LONGITUDE_REF) : "");

	LSG_GPS coordinates = {
		.latitude  = LSG_Exif::getGPSCoordinate(latitude,  latitudeRef),
		.longitude = LSG_Exif::getGPSCoordinate(longitude, longitudeRef),
		.altitude  = LSG_Exif::getDouble(altitude)
	};

	return coordinates;
}

LSG_GPSCoordinate LSG_Exif::getGPSCoordinate(const std::string& rational, const std::string& ref)
{
	if (rational.empty())
		return {};

	LSG_Rational d, m, s;

	auto result = std::sscanf(rational.c_str(), "%d/%d;%d/%d;%d/%d", &d.numerator, &d.denominator, &m.numerator, &m.denominator, &s.numerator, &s.denominator);

	if (result < 6)
		return {};

	LSG_GPSCoordinate coordinate = {
		.degrees = LSG_Exif::getDouble(d),
		.minutes = LSG_Exif::getDouble(m),
		.seconds = LSG_Exif::getDouble(s)
	};

	if ((ref == "S") || (ref == "W"))
	{
		coordinate.degrees = -coordinate.degrees;
		coordinate.minutes = -coordinate.minutes;
		coordinate.seconds = -coordinate.seconds;
	}

	coordinate.decimal = (coordinate.degrees + (coordinate.minutes / 60.0) + (coordinate.seconds / 3600.0));

	return coordinate;
}

LSG_IFD LSG_Exif::getIFD()
{
	LSG_IFD ifd = {};

	std::fread(&ifd.data, 1, 12, LSG_Exif::file);

	ifd.tagID      = LSG_Bytes::ToUInt(ifd.data[0], ifd.data[1], LSG_Exif::isByteOrderIntel);
	ifd.dataType   = (LSG_IFD_DataType)LSG_Bytes::ToUInt(ifd.data[2], ifd.data[3], LSG_Exif::isByteOrderIntel);
	ifd.nrOfValues = LSG_Bytes::ToUInt(ifd.data[4], ifd.data[5], ifd.data[6], ifd.data[7],  LSG_Exif::isByteOrderIntel);

	switch (ifd.dataType) {
	case LSG_IFD_DATA_TYPE_STRING:
		ifd.dataValue.string = LSG_Exif::getValueString(ifd);
		break;
	case LSG_IFD_DATA_TYPE_UINT8:
		for (uint32_t i = 0; i < ifd.nrOfValues; i++) {
			ifd.dataValue.uinteger = ifd.data[8 + i];
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.uinteger));
		}
		break;
	case LSG_IFD_DATA_TYPE_UINT16:
		for (uint32_t i = 0; i < ifd.nrOfValues; i += 2) {
			ifd.dataValue.uinteger = LSG_Bytes::ToUInt(ifd.data[8 + i], ifd.data[9 + i], LSG_Exif::isByteOrderIntel);
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.uinteger));
		}
		break;
	case LSG_IFD_DATA_TYPE_UINT32:
		ifd.dataValue.uinteger = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);
		ifd.dataValue.string   = std::to_string(ifd.dataValue.uinteger);
		break;
	case LSG_IFD_DATA_TYPE_URATIONAL64:
		ifd.dataValue.urationals = LSG_Exif::getValueURationals(ifd);
		ifd.dataValue.urational  = ifd.dataValue.urationals[0];

		for (const auto& urational : ifd.dataValue.urationals)
			ifd.dataValue.strings.push_back(std::format("{}/{}", urational.numerator, urational.denominator));
		break;
	case LSG_IFD_DATA_TYPE_INT8:
		for (uint32_t i = 0; i < ifd.nrOfValues; i++) {
			ifd.dataValue.integer = (int8_t)ifd.data[8 + i];
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.integer));
		}
	break;
	case LSG_IFD_DATA_TYPE_INT16:
		for (uint32_t i = 0; i < ifd.nrOfValues; i += 2) {
			ifd.dataValue.integer = LSG_Bytes::ToInt(ifd.data[8 + i], ifd.data[9 + i], LSG_Exif::isByteOrderIntel);
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.integer));
		}
	break;
	case LSG_IFD_DATA_TYPE_INT32:
		ifd.dataValue.integer = LSG_Bytes::ToInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);
		ifd.dataValue.string  = std::to_string(ifd.dataValue.integer);
		break;
	case LSG_IFD_DATA_TYPE_RATIONAL64:
		ifd.dataValue.rationals = LSG_Exif::getValueRationals(ifd);
		ifd.dataValue.rational  = ifd.dataValue.rationals[0];

		for (const auto& rational : ifd.dataValue.rationals)
			ifd.dataValue.strings.push_back(std::format("{}/{}", rational.numerator, rational.denominator));
		break;
	case LSG_IFD_DATA_TYPE_FLOAT:
	case LSG_IFD_DATA_TYPE_DOUBLE:
		break;
	default:
		break;
	}

	if (ifd.dataValue.strings.size() > 1)
		ifd.dataValue.string = LSG_Text::Join(ifd.dataValue.strings, ";");
	else if (!ifd.dataValue.strings.empty())
		ifd.dataValue.string = ifd.dataValue.strings[0];

	return ifd;
}

long LSG_Exif::getOffsetIFD()
{
	uint8_t ifd[4];

	std::fread(&ifd, 1, 4, LSG_Exif::file);

	return LSG_Bytes::ToUInt(ifd[0], ifd[1], ifd[2], ifd[3], LSG_Exif::isByteOrderIntel);
}

LSG_ImageOrientation LSG_Exif::GetOrientation(const LSG_ExifTags& tags)
{
	if (tags.empty() || !tags.contains(LSG_EXIF_TAG_ID_ORIENTATION))
		return {};

	auto dataValue = std::atoi(tags.at(LSG_EXIF_TAG_ID_ORIENTATION).c_str());

	if ((dataValue < 1) || (dataValue > 8))
		return {};

	LSG_ImageOrientation orientation = {};

	switch (dataValue) {
	case 1: // Horizontal (normal)
		break;
	case 2: // Mirror horizontal
		orientation.flip = SDL_FLIP_HORIZONTAL;
		break;
	case 3: // Rotate 180
		orientation.rotation = 180.0;
		break;
	case 4: // Mirror vertical
		orientation.flip = SDL_FLIP_VERTICAL;
		break;
	case 5: // Mirror horizontal and rotate 270 CW
		orientation.flip     = SDL_FLIP_HORIZONTAL;
		orientation.rotation = 270.0;
		break;
	case 6: // Rotate 90 CW
		orientation.rotation = 90.0;
		break;
	case 7: // Mirror horizontal and rotate 90 CW
		orientation.flip     = SDL_FLIP_HORIZONTAL;
		orientation.rotation = 90.0;
		break;
	case 8: // Rotate 270 CW
		orientation.rotation = 270.0;
		break;
	default:
		break;
	}

	return orientation;
}

uint16_t LSG_Exif::getNrOfDirectories()
{
	uint8_t nrOfIFDs[2];

	std::fread(&nrOfIFDs, 1, 2, LSG_Exif::file);

	return LSG_Bytes::ToUInt(nrOfIFDs[0], nrOfIFDs[1], LSG_Exif::isByteOrderIntel);
}

SDL_Surface* LSG_Exif::getThumbnail()
{
	uint16_t compression = 0;
	uint32_t dataOffset  = 0;
	uint32_t dataSize    = 0;

	auto nrOfDirectories = LSG_Exif::getNrOfDirectories();

	for (uint16_t i = 0; i < nrOfDirectories; i++)
	{
		auto ifd = LSG_Exif::getIFD();

		switch (ifd.tagID) {
			case LSG_EXIF_TAG_ID_THUMB_COMPRESSION: compression = ifd.dataValue.uinteger; break;
			case LSG_EXIF_TAG_ID_THUMB_JPEG_OFFSET: dataOffset  = ifd.dataValue.uinteger; break;
			case LSG_EXIF_TAG_ID_THUMB_JPEG_SIZE:   dataSize    = ifd.dataValue.uinteger; break;
			default: break;
		}
	}

	if (!dataOffset || !dataSize || (compression != 6))
		return nullptr;

	std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + dataOffset), SEEK_SET);

	auto pixels = (uint8_t*)std::malloc(dataSize);

	std::fread(pixels, 1, dataSize, LSG_Exif::file);

	auto thumbnail = IMG_LoadJPG_RW(SDL_RWFromConstMem(pixels, dataSize));

	std::free(pixels);

	return thumbnail;
}

std::string LSG_Exif::getValueString(const LSG_IFD& ifd)
{
	auto buffer = (char*)std::malloc(ifd.nrOfValues);

	if (!buffer)
		return "";

	if (ifd.nrOfValues > 4)
	{
		auto position = std::ftell(LSG_Exif::file);
		auto offset   = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);

		std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + offset), SEEK_SET);

		std::fread(buffer, 1, ifd.nrOfValues, LSG_Exif::file);

		std::fseek(LSG_Exif::file, position, SEEK_SET);
	} else {
		std::memcpy(buffer, &ifd.data[8], ifd.nrOfValues);
	}

	auto value = std::string(buffer);

	std::free(buffer);

	return value;
}

LSG_Rationals LSG_Exif::getValueRationals(const LSG_IFD& ifd)
{
	LSG_Rationals rationals;

	auto urationals = LSG_Exif::getValueURationals(ifd);

	for (const auto& urational : urationals)
		rationals.push_back({ (int32_t)urational.numerator, (int32_t)urational.denominator });

	return rationals;
}

LSG_URationals LSG_Exif::getValueURationals(const LSG_IFD& ifd)
{
	auto position = std::ftell(LSG_Exif::file);
	auto offset   = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);

	std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + offset), SEEK_SET);

	LSG_URationals values;

	for (uint32_t i = 0; i < ifd.nrOfValues; i++)
	{
		uint8_t bytes[8];

		std::fread(&bytes, 1, 8, LSG_Exif::file);

		values.push_back({
			.numerator   = LSG_Bytes::ToUInt(bytes[0], bytes[1], bytes[2], bytes[3], LSG_Exif::isByteOrderIntel),
			.denominator = LSG_Bytes::ToUInt(bytes[4], bytes[5], bytes[6], bytes[7], LSG_Exif::isByteOrderIntel),
		});
	}

	std::fseek(LSG_Exif::file, position, SEEK_SET);

	return values;
}

bool LSG_Exif::isExif()
{
	char exifString[6] = {};

	std::fread(&exifString, 1, 6, LSG_Exif::file);

	bool isExif = (std::strncmp(exifString, "Exif\0\0", 6) == 0);

	LSG_Exif::offsetHeader = std::ftell(LSG_Exif::file);

	char byteOrder[2] = {};

	std::fread(&byteOrder, 1, 2, LSG_Exif::file); // "II" (Intel/Little endian) or "MM" (Motorola/Big endian)

	LSG_Exif::isByteOrderIntel = (std::strncmp(byteOrder, "II", 2) == 0);

	return isExif;
}

bool LSG_Exif::isMarker()
{
	uint8_t tiffMarker[2];

	std::fread(&tiffMarker, 1, 2, LSG_Exif::file);

	return (LSG_Bytes::ToUInt(tiffMarker[0], tiffMarker[1], LSG_Exif::isByteOrderIntel) == 0x2A);  // Exif (0x2A)
}

bool LSG_Exif::isValid()
{
	std::fseek(LSG_Exif::file, 2, SEEK_CUR); // Skip section size

	return (LSG_Exif::isExif() && LSG_Exif::isMarker());
}

void LSG_Exif::seekToHeader()
{
	std::rewind(LSG_Exif::file);

	int c1, c2;

	do {
		c1 = std::fgetc(LSG_Exif::file);
		c2 = (c1 != EOF ? std::fgetc(LSG_Exif::file) : EOF);

		if ((c1 == 0xFF) && (c2 == 0xE1)) // EXIF header (0xFFE1)
			break;
	} while ((c1 != EOF) && (c2 != EOF));
}
