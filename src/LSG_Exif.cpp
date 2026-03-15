#include "LSG_Exif.h"

void LSG_Exif::addTags(LSG_ExifTags& tags, LSG_ExifState& state)
{
	auto nrOfDirectories = LSG_Exif::getNrOfDirectories(state);
		
	for (uint16_t i = 0; i < nrOfDirectories; i++)
	{
        auto ifd = LSG_Exif::getIFD(state);

		if (!ifd.dataValue.string.empty())
			tags[ifd.tagID] = ifd.dataValue.string;

		switch (ifd.tagID) {
			case LSG_EXIF_TAG_ID_OFFSET_GPS_INFO: state.offsetGPSInfo = ifd.dataValue.uinteger; break;
			case LSG_EXIF_TAG_ID_OFFSET_SUB_IFD:  state.offsetSubIFD  = ifd.dataValue.uinteger; break;
			default: break;
		}
	}
}

LSG_ExifData LSG_Exif::Get(const std::string& filePath)
{
	LSG_ExifState state = {};

	#if defined _windows
		state.file = _wfopen(LSG_Text::ToWide(filePath).c_str(), L"rb");
	#else
		state.file = std::fopen(filePath.c_str(), "rb");
	#endif

	if (!state.file)
		return {};

	LSG_Exif::seekToHeader(state.file);

	if (!LSG_Exif::isValid(state)) {
		std::fclose(state.file);
		return {};
	}

	LSG_ExifData data = {};

	// IFD0 (main image)

	long offsetIFD1 = 0;
	auto offsetIFD0 = LSG_Exif::getOffsetIFD(state);

	if (offsetIFD0)
	{
		std::fseek(state.file, (state.offsetHeader + offsetIFD0), SEEK_SET);

		LSG_Exif::addTags(data.tags, state);

		offsetIFD1 = LSG_Exif::getOffsetIFD(state);
	}

	// GPSInfo

	if (state.offsetGPSInfo)
	{
		std::fseek(state.file, (state.offsetHeader + state.offsetGPSInfo), SEEK_SET);

		LSG_Exif::addTags(data.gps, state);
	}

	// SubIFD

	if (state.offsetSubIFD)
	{
		std::fseek(state.file, (state.offsetHeader + state.offsetSubIFD), SEEK_SET);

		LSG_Exif::addTags(data.tags, state);
	}

	// IFD1 (thumbnail image)

	if (offsetIFD1)
	{
		std::fseek(state.file, (state.offsetHeader + offsetIFD1), SEEK_SET);

		data.thumbnail = LSG_Exif::getThumbnail(state);
	}

	std::fclose(state.file);

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

	auto result = std::sscanf(
		rational.c_str(),
		"%d/%d;%d/%d;%d/%d",
		&d.numerator,
		&d.denominator,
		&m.numerator,
		&m.denominator,
		&s.numerator,
		&s.denominator
	);

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

LSG_IFD LSG_Exif::getIFD(LSG_ExifState& state)
{
	LSG_IFD ifd = {};

	if (std::fread(&ifd.data, 1, 12, state.file) < 12)
		return ifd;

	ifd.tagID      = LSG_Bytes::ToUInt(ifd.data[0], ifd.data[1], state.isByteOrderIntel);
	ifd.dataType   = (LSG_IFD_DataType)LSG_Bytes::ToUInt(ifd.data[2], ifd.data[3], state.isByteOrderIntel);
	ifd.nrOfValues = LSG_Bytes::ToUInt(ifd.data[4], ifd.data[5], ifd.data[6], ifd.data[7], state.isByteOrderIntel);

	switch (ifd.dataType) {
	case LSG_IFD_DATA_TYPE_STRING:
		ifd.dataValue.string = LSG_Exif::getValueString(ifd, state);
		break;
	case LSG_IFD_DATA_TYPE_UINT8:
		for (uint32_t i = 0; i < ifd.nrOfValues; i++) {
			ifd.dataValue.uinteger = ifd.data[8 + i];
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.uinteger));
		}
		break;
	case LSG_IFD_DATA_TYPE_UINT16:
		for (uint32_t i = 0; i < ifd.nrOfValues; i += 2) {
			ifd.dataValue.uinteger = LSG_Bytes::ToUInt(ifd.data[8 + i], ifd.data[9 + i], state.isByteOrderIntel);
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.uinteger));
		}
		break;
	case LSG_IFD_DATA_TYPE_UINT32:
		ifd.dataValue.uinteger = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], state.isByteOrderIntel);
		ifd.dataValue.string   = std::to_string(ifd.dataValue.uinteger);
		break;
	case LSG_IFD_DATA_TYPE_URATIONAL64:
		ifd.dataValue.urationals = LSG_Exif::getValueURationals(ifd, state);
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
			ifd.dataValue.integer = LSG_Bytes::ToInt(ifd.data[8 + i], ifd.data[9 + i], state.isByteOrderIntel);
			ifd.dataValue.strings.push_back(std::to_string(ifd.dataValue.integer));
		}
	break;
	case LSG_IFD_DATA_TYPE_INT32:
		ifd.dataValue.integer = LSG_Bytes::ToInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], state.isByteOrderIntel);
		ifd.dataValue.string  = std::to_string(ifd.dataValue.integer);
		break;
	case LSG_IFD_DATA_TYPE_RATIONAL64:
		ifd.dataValue.rationals = LSG_Exif::getValueRationals(ifd, state);
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

long LSG_Exif::getOffsetIFD(LSG_ExifState& state)
{
	uint8_t ifd[4];

	if (std::fread(&ifd, 1, 4, state.file) < 4)
		return 0;

	return LSG_Bytes::ToUInt(ifd[0], ifd[1], ifd[2], ifd[3], state.isByteOrderIntel);
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

uint16_t LSG_Exif::getNrOfDirectories(LSG_ExifState& state)
{
	uint8_t nrOfIFDs[2];

	if (std::fread(&nrOfIFDs, 1, 2, state.file) < 2)
		return 0;

	return LSG_Bytes::ToUInt(nrOfIFDs[0], nrOfIFDs[1], state.isByteOrderIntel);
}

SDL_Surface* LSG_Exif::getThumbnail(LSG_ExifState& state)
{
	uint16_t compression = 0;
	uint32_t dataOffset  = 0;
	uint32_t dataSize    = 0;

	auto nrOfDirectories = LSG_Exif::getNrOfDirectories(state);

	for (uint16_t i = 0; i < nrOfDirectories; i++)
	{
		auto ifd = LSG_Exif::getIFD(state);

		switch (ifd.tagID) {
			case LSG_EXIF_TAG_ID_THUMB_COMPRESSION: compression = ifd.dataValue.uinteger; break;
			case LSG_EXIF_TAG_ID_THUMB_JPEG_OFFSET: dataOffset  = ifd.dataValue.uinteger; break;
			case LSG_EXIF_TAG_ID_THUMB_JPEG_SIZE:   dataSize    = ifd.dataValue.uinteger; break;
			default: break;
		}
	}

	if (!dataOffset || !dataSize || (compression != 6))
		return nullptr;

	std::fseek(state.file, (state.offsetHeader + dataOffset), SEEK_SET);

	auto pixels = (uint8_t*)std::malloc(dataSize);

	SDL_Surface* thumbnail = nullptr;

	if (std::fread(pixels, 1, dataSize, state.file) == dataSize)
		thumbnail = IMG_LoadJPG_RW(SDL_RWFromConstMem(pixels, dataSize));

	std::free(pixels);

	return thumbnail;
}

std::string LSG_Exif::getValueString(const LSG_IFD& ifd, LSG_ExifState& state)
{
	auto buffer = (char*)std::malloc(ifd.nrOfValues);

	if (!buffer)
		return "";

	size_t bufferSize = ifd.nrOfValues;

	if (ifd.nrOfValues > 4)
	{
		auto position = std::ftell(state.file);
		auto offset   = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], state.isByteOrderIntel);

		std::fseek(state.file, (state.offsetHeader + offset), SEEK_SET);

		bufferSize = std::fread(buffer, 1, ifd.nrOfValues, state.file);

		std::fseek(state.file, position, SEEK_SET);
	} else {
		std::memcpy(buffer, &ifd.data[8], ifd.nrOfValues);
	}

	auto value = (bufferSize == ifd.nrOfValues ? std::string(buffer) : "");

	std::free(buffer);

	return value;
}

LSG_Rationals LSG_Exif::getValueRationals(const LSG_IFD& ifd, LSG_ExifState& state)
{
	LSG_Rationals rationals;

	auto urationals = LSG_Exif::getValueURationals(ifd, state);

	for (const auto& urational : urationals)
		rationals.push_back({ (int32_t)urational.numerator, (int32_t)urational.denominator });

	return rationals;
}

LSG_URationals LSG_Exif::getValueURationals(const LSG_IFD& ifd, LSG_ExifState& state)
{
	auto position = std::ftell(state.file);
	auto offset   = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], state.isByteOrderIntel);

	std::fseek(state.file, (state.offsetHeader + offset), SEEK_SET);

	LSG_URationals values;

	for (uint32_t i = 0; i < ifd.nrOfValues; i++)
	{
		uint8_t bytes[8];

		if (std::fread(&bytes, 1, 8, state.file) < 8) {
			values.clear();
			break;
		}

		values.push_back({
			.numerator   = LSG_Bytes::ToUInt(bytes[0], bytes[1], bytes[2], bytes[3], state.isByteOrderIntel),
			.denominator = LSG_Bytes::ToUInt(bytes[4], bytes[5], bytes[6], bytes[7], state.isByteOrderIntel),
		});
	}

	std::fseek(state.file, position, SEEK_SET);

	return values;
}

bool LSG_Exif::isExif(LSG_ExifState& state)
{
	char exifString[6] = {};

	if (std::fread(&exifString, 1, 6, state.file) < 6)
		return false;

	bool isExif = (std::strncmp(exifString, "Exif\0\0", 6) == 0);

	state.offsetHeader = std::ftell(state.file);

	// "II" (Intel/Little endian) or "MM" (Motorola/Big endian)

	char byteOrder[2] = {};

	if (std::fread(&byteOrder, 1, 2, state.file) < 2) {
		state.offsetHeader = 0;
		return false;
	}

	state.isByteOrderIntel = (std::strncmp(byteOrder, "II", 2) == 0);

	return isExif;
}

bool LSG_Exif::isMarker(LSG_ExifState& state)
{
	uint8_t tiffMarker[2];

	if (std::fread(&tiffMarker, 1, 2, state.file) < 2)
		return false;

	// Exif (0x2A)

	auto marker = LSG_Bytes::ToUInt(tiffMarker[0], tiffMarker[1], state.isByteOrderIntel);

	return (marker == 0x2A);
}

bool LSG_Exif::isValid(LSG_ExifState& state)
{
	std::fseek(state.file, 2, SEEK_CUR); // Skip section size

	return (LSG_Exif::isExif(state) && LSG_Exif::isMarker(state));
}

void LSG_Exif::seekToHeader(FILE* file)
{
	std::rewind(file);

	int c1, c2;

	do {
		c1 = std::fgetc(file);
		c2 = (c1 != EOF ? std::fgetc(file) : EOF);

		if ((c1 == 0xFF) && (c2 == 0xE1)) // EXIF header (0xFFE1)
			break;
	} while ((c1 != EOF) && (c2 != EOF));
}
