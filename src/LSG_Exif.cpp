#include "LSG_Exif.h"

FILE* LSG_Exif::file             = nullptr;
bool  LSG_Exif::isByteOrderIntel = false;
long  LSG_Exif::offsetHeader     = 0;
long  LSG_Exif::offsetSubIFD     = 0;

void LSG_Exif::addTags(LSG_ExifTags& tags)
{
	auto nrOfDirectories = LSG_Exif::getNrOfDirectories();
		
	for (uint16_t i = 0; i < nrOfDirectories; i++)
	{
        auto ifd = LSG_Exif::getIFD();

		if (!ifd.dataValue.string.empty())
			tags[ifd.tagID] = ifd.dataValue.string;

		if (ifd.tagID == LSG_EXIF_TAG_ID_OFFSET_SUB_IFD)
			LSG_Exif::offsetSubIFD = ifd.dataValue.uinteger;
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
	
	// Exif SubIFD

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

LSG_IFD LSG_Exif::getIFD()
{
	LSG_IFD ifd = {};

	std::fread(&ifd.data, 1, 12, LSG_Exif::file);

	ifd.tagID                = LSG_Bytes::ToUInt(ifd.data[0], ifd.data[1], LSG_Exif::isByteOrderIntel);
	ifd.dataType             = (LSG_IFD_DataType)LSG_Bytes::ToUInt(ifd.data[2], ifd.data[3], LSG_Exif::isByteOrderIntel);
	ifd.dataValue.nrOfValues = LSG_Bytes::ToUInt(ifd.data[4], ifd.data[5], ifd.data[6], ifd.data[7],  LSG_Exif::isByteOrderIntel);

	switch (ifd.dataType) {
	case LSG_IFD_DATA_TYPE_STRING:
		ifd.dataValue.string = LSG_Exif::getValueString(ifd);
		break;
	case LSG_IFD_DATA_TYPE_UINT8:
		ifd.dataValue.uinteger = ifd.data[8];
		ifd.dataValue.string   = std::to_string(ifd.dataValue.uinteger);
		break;
	case LSG_IFD_DATA_TYPE_UINT16:
		ifd.dataValue.uinteger = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], LSG_Exif::isByteOrderIntel);
		ifd.dataValue.string   = std::to_string(ifd.dataValue.uinteger);
		break;
	case LSG_IFD_DATA_TYPE_UINT32:
		ifd.dataValue.uinteger = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);
		ifd.dataValue.string   = std::to_string(ifd.dataValue.uinteger);
		break;
	case LSG_IFD_DATA_TYPE_URATIONAL64:
		ifd.dataValue.urational = LSG_Exif::getValueURational(ifd);
		ifd.dataValue.string    = std::format("{}/{}", ifd.dataValue.urational.numerator, ifd.dataValue.urational.denominator);
		break;
	case LSG_IFD_DATA_TYPE_INT8:
		ifd.dataValue.integer = (int8_t)ifd.data[8];
		ifd.dataValue.string  = std::to_string(ifd.dataValue.integer);
	break;
	case LSG_IFD_DATA_TYPE_INT16:
		ifd.dataValue.integer = LSG_Bytes::ToInt(ifd.data[8], ifd.data[9], LSG_Exif::isByteOrderIntel);
		ifd.dataValue.string  = std::to_string(ifd.dataValue.integer);
	break;
	case LSG_IFD_DATA_TYPE_INT32:
		ifd.dataValue.integer = LSG_Bytes::ToInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);
		ifd.dataValue.string  = std::to_string(ifd.dataValue.integer);
		break;
	case LSG_IFD_DATA_TYPE_RATIONAL64:
		ifd.dataValue.rational = LSG_Exif::getValueRational(ifd);
		ifd.dataValue.string   = std::format("{}/{}", ifd.dataValue.rational.numerator, ifd.dataValue.rational.denominator);
		break;
	case LSG_IFD_DATA_TYPE_FLOAT:
	case LSG_IFD_DATA_TYPE_DOUBLE:
		break;
	default:
		break;
	}

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
	auto buffer = (char*)std::malloc(ifd.dataValue.nrOfValues);

	if (!buffer)
		return "";

	if (ifd.dataValue.nrOfValues > 4)
	{
		auto position = std::ftell(LSG_Exif::file);
		auto offset   = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);

		std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + offset), SEEK_SET);

		std::fread(buffer, 1, ifd.dataValue.nrOfValues, LSG_Exif::file);

		std::fseek(LSG_Exif::file, position, SEEK_SET);
	} else {
		std::memcpy(buffer, &ifd.data[8], ifd.dataValue.nrOfValues);
	}

	auto value = std::string(buffer);

	std::free(buffer);

	return value;
}

LSG_Rational LSG_Exif::getValueRational(const LSG_IFD& ifd)
{
	auto urational = LSG_Exif::getValueURational(ifd);

	LSG_Rational rational = {
		(int32_t)urational.numerator,
		(int32_t)urational.denominator
	};

	return rational;
}

LSG_URational LSG_Exif::getValueURational(const LSG_IFD& ifd)
{
	auto position = std::ftell(LSG_Exif::file);
	auto offset   = LSG_Bytes::ToUInt(ifd.data[8], ifd.data[9], ifd.data[10], ifd.data[11], LSG_Exif::isByteOrderIntel);

	std::fseek(LSG_Exif::file, (LSG_Exif::offsetHeader + offset), SEEK_SET);

	uint8_t bytes[8];

	std::fread(&bytes, 1, 8, LSG_Exif::file);

	LSG_URational value = {
		.numerator   = LSG_Bytes::ToUInt(bytes[0], bytes[1], bytes[2], bytes[3], LSG_Exif::isByteOrderIntel),
		.denominator = LSG_Bytes::ToUInt(bytes[4], bytes[5], bytes[6], bytes[7], LSG_Exif::isByteOrderIntel),
	};

	std::fseek(LSG_Exif::file, position, SEEK_SET);

	return value;
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
